/*

servo_reader.c

This module measures the pulse duration on the individual servo inputs.
It uses the interrupt-on-change function to capture edges of the servo pulses.
TIMER1 is used to measure time; it must be set to increment in 1us intervals.

*/
#include "processor.h"
#include <stdint.h>

#define STARTUP_MODE_NEUTRAL 4


#define NUM_AVERAGES 4
static uint8_t tune_wait;
static uint16_t avg[NUM_AVERAGES];

uint8_t channel_data[4];
extern struct {
    unsigned locked : 1;
    unsigned new_data : 1;
} flags;


#define TIMING_OFFSET 0x20
#define SYNC_VALUE_LIMIT 0x880
#define SYNC_VALUE_NOMINAL 0xa40

// We defind how many channels we have, and which bit of PORTA each channel is.
// Then we calculate an AND mask for all channels.
#define NUM_CHANNELS 3
#define ST_BIT_NUMBER 2
#define TH_BIT_NUMBER 4
#define CH3_BIT_NUMBER 5

#define CHANNEL_MASK ((1 << ST_BIT_NUMBER) | (1 << TH_BIT_NUMBER) | (1 << CH3_BIT_NUMBER)) 


// Each time any of the channel port changes, an interrupt is generated. The
// interrupt captures the current time and stores it, along with the current
// state of the servo channels. 
// After all channels have been seen the main-loop function processes the edges
// and calculates the pulse durations
static struct {
    uint16_t time;
    uint8_t port_state;
} timestamps[NUM_CHANNELS * 2];
static uint8_t timestamp_count;    
static uint8_t channel_flags;    

static uint8_t wait_for_all_channels_inactive;    

static uint16_t st;
static uint16_t st_center;
static uint16_t st_epl;
static uint16_t st_epr;

static uint16_t th;
static uint16_t th_center;
static uint16_t th_epl;
static uint16_t th_epr;

static uint16_t ch3;
static uint8_t last_ch3;

#define CH3_EP0 (1000 >> 4)
#define CH3_EP1 (2000 >> 4)
#define CH3_CENTER ((CH3_EP1 - CH3_EP0) / 2) + CH3_EP0
#define CH3_HYSTERESIS 3
    

/****************************************************************************
 This function must be called once at start-up.
 ****************************************************************************/
void Init_servo_reader(void) 
{
    // Start values for center and endpoints for steering and throttle
    st_center = 1500;
    st_epl = 1250;
    st_epr = 1750;

    th_center = 1500;
    th_epl = 1250;
    th_epr = 1750;

    // Enable interrupt-on-change for both edges on all channels
    IOCAP = CHANNEL_MASK;     
    IOCAN = CHANNEL_MASK;

    // Start measuring channels only if currently all channels are inactive
    wait_for_all_channels_inactive = 1;
}


/*****************************************************************************
 This function must be called by the interrupt handler
 
 The interrupt function and servo_read_all_channels() use the IOCIE bit as
 flag whether measurement is in progress.
 *****************************************************************************/
void servo_reader_interrupt(void) __interrupt 0
{
    uint8_t porta_temp;

    union {
        struct {
            uint8_t tl;
            uint8_t th;
        };
        uint16_t t;
    } t;


    // Retrieve TIMER1 value safely by briefly stopping the timer while reading
    // it. The potential small error introduced is neglectible and constant.
    TMR1ON = 0;     
    t.th = TMR1H;
    t.tl = TMR1L;
    TMR1ON = 1;     


    // It is important go clear the flags first and then read the PORT value.
    // If it is done the other way around we may have the unlikely situation
    // that the port changed right after reading the port, and we would never
    // see that change in the port. 
    // If we read the port first then worst case we process the interrupt
    // but the flag is already in the previous measurement.
    IOCAF &= ~CHANNEL_MASK;
    porta_temp = PORTA & CHANNEL_MASK;


    // Store a snapshot of the current time and port state for later processing
    timestamps[timestamp_count].time = t.t;
    timestamps[timestamp_count].port_state = porta_temp;
    ++timestamp_count;
    
    
    // Check if we have seen the '1' periods of all channels, and the 
    // current state of all channels is 0 -- which means we have measurements
    // of the pulse width of all channels.
    //
    // We also terminate if we are running out of storage space, which can
    // only happens in error situations such as not all channels being 
    // transmitted in one sequence.
    channel_flags |= porta_temp;
    if ((channel_flags == CHANNEL_MASK  &&     // All channels have seen a '1'? AND ...
         porta_temp == 0)  ||                  // ... all channels currently at 0?
            timestamp_count > (NUM_CHANNELS * 2)) { 
        IOCIE = 0;    
        TMR1ON = 0;     
    }
}


/*****************************************************************************
 Calculate the servo value in range of +/-100% from the given pulse duration,
 center and endpoint values.
 *****************************************************************************/
static int8_t normalize(uint16_t pulse, uint16_t cen, uint16_t epl, uint16_t epr)
{
    if (pulse == cen) {
        return 0;
    }
    else if (pulse > cen) {
        if (pulse >= epr) {
            return 100;
        }
        return (pulse - cen) * 100 / (epr - cen);
    }
    else {
        if (pulse <= epl) {
            return -100;
        }
        return -((cen - pulse) * 100 / (cen - epl));
    }
}


/*****************************************************************************
 Calculate the CH3 switch position using the pulse duration plus a hysteresis.
 *****************************************************************************/
static uint8_t normalize_ch3(void)
{
    uint8_t ch3_normalized;

    ch3_normalized = ch3 >> 4;
    
    if (last_ch3) {
        if (ch3_normalized < (CH3_CENTER - CH3_HYSTERESIS)) {
            last_ch3 = 0;
        }
    }
    else {
        if (ch3_normalized > (CH3_CENTER + CH3_HYSTERESIS)) {
            last_ch3 = 1;
        }
    }
    
    return last_ch3;
}


/*****************************************************************************
 *****************************************************************************/
static uint8_t expansion_protocol_ch3(void) 
{
    // Remove the offset we added in the transmitter to ensure the minimum
    // pulse does not go down to zero.
    if (ch3 < SYNC_VALUE_LIMIT) {
        if (ch3 < TIMING_OFFSET) {
            return 0;
        }
        return ((ch3 - TIMING_OFFSET) >> 5);
    }
    
    // If we are dealing with a sync value we clamp it to the nominal value
    // so that our check whether the value has changed does not trigger 
    // wrongly when jitter moves between e.g. 0xa40 and 0xa3f
    return (SYNC_VALUE_NOMINAL >> 5);
}


/*****************************************************************************
 Check if a servo pulse is is within 600 and 2500us. 
 *****************************************************************************/
static uint8_t is_servo_measurement_valid(uint16_t pulse)
{
    if (pulse > 2500  ||  pulse < 600) {
        return 0;
    }

    return 1;    
}


/*****************************************************************************
 Clamp a servo pulse to 800 < pulse < 2300us.
 *****************************************************************************/
static uint16_t clamp_servo_pulse(uint16_t pulse)
{
    if (pulse < 800) {
        return 800;
    }

    if (pulse > 2300) {
        return 2300;
    }
    
    return pulse;
}


/*****************************************************************************
 Returns 1 if all servo pulses are within limits, otherwise 0.
 As a side effect it also clamps pulses to a narrower range.
 *****************************************************************************/
static uint8_t are_servo_measurements_valid(void)
{
    uint8_t retval = 1;
    
    retval = is_servo_measurement_valid(st);
    st = clamp_servo_pulse(st);

    retval &= is_servo_measurement_valid(th);
    th = clamp_servo_pulse(th);

    retval &= is_servo_measurement_valid(ch3);
    ch3 = clamp_servo_pulse(ch3);

    return retval;
}


/*****************************************************************************
 This function processes the edge times captured by the interrupt to calculate
 the pulse duration of a given servo port. The servo port is given as bit mask
 corresponding to the bit in PORTA that the servo connects to.
 
 If the function can't find both rising and falling edges then it returns 0.
 *****************************************************************************/
static uint16_t calculate_servo_pulse(uint8_t mask)
{
    uint8_t i;
    uint16_t pulse_start_time = 0;
    uint16_t pulse_end_time = 0;

    // Scan the edges until we find the first '1' in port_state for the given
    // servo port. Save the time of the edge as pulse_start_time
    for (i = 0; i < timestamp_count; i++) {
        if ((timestamps[i].port_state & mask) != 0) {
            pulse_start_time = timestamps[i].time;
            break;
        }
    }
    
    // Check if i >= timestamp_count, which means we didn't find the rising edge
    if (i >= timestamp_count) {
        return 0;
    }

    // Scan the edges until we find the next '0' in port_state for the given
    // servo port. Save the time of the edge as pulse_end_time
    for (; i < timestamp_count; i++) {
        if ((timestamps[i].port_state & mask) == 0) {
            pulse_end_time = timestamps[i].time;
            break;
        }
    }

    // Check if i >= timestamp_count, which means we didn't find the falling edge
    if (i >= timestamp_count) {
        return 0;
    }
    
    // Return (end - start)
    // Note that overflow is not taken into account as TIMER1 overflow is 
    // checked elsewhere and pulse repetition time should be << TIMER1MAX
    return pulse_end_time - pulse_start_time;
}



/*****************************************************************************
 Automatic endpoint adjustment: if the pulse we measure is larger/smaller then
 the respective endpoint, we move the endpoint to capture the pulse range that
 the transmitter is using.
 *****************************************************************************/
static void adjust_endpoints(void)
{
    if (st < st_epl) {
        st_epl = st;
    }
    
    if (st > st_epr) {
        st_epr = st;
    }
    
    if (th < th_epl) {
        th_epl = th;
    }
    
    if (th > th_epr) {
        th_epr = th;
    }
}


/*****************************************************************************
 ****************************************************************************/
static void tuneOscillator(uint16_t value) 
{
    char i;

    for (i = 0; i < NUM_AVERAGES - 1; i++) {
        avg[i] = avg[i + 1];
    }
    avg[i] = value;

    if (tune_wait) {
        --tune_wait;
        return;
    }

    // Calculate the average of the last four values, to remove jitter
    value = 0;
    for (i = 0; i < NUM_AVERAGES; i++) {
        value += avg[i];
    }
    value = value / NUM_AVERAGES;     

    tune_wait = NUM_AVERAGES;
    i = OSCTUNE & 0x3f;
    // Convert 6-bit signed into 6-bit unsigned
    i = i ^ 0x20;
    
    if (value < 0xa31) {
        flags.locked = 0;
        i += 10;
    }
    else if (value > 0xa57) {
        flags.locked = 0;
        i -= 10;
    }
    else if (value < 0xa3e) {
        ++i;
    }
    else if (value > 0xa42) {
        --i;
    }
    else {
        tune_wait = 0;
        flags.locked = 1;
    }    

    if (i < 0) {
        i = 0;
    }
    else if (i > 0x3f) {
        i = 0x3f;
    } 

    // Convert 6-bit unsigned back into 6-bit signed
    i = i ^ 0x20;
    OSCTUNE = i;
}


/*****************************************************************************
 This function must be called periodically, i.e. once every mainloop.
 
 It waits until the IOCIE bit is cleared, indicating that the interrupt has
 captured all edges. 
 It then calculates all the servo pulse durations, validates them, and sends
 them off if everything is ok.
 *****************************************************************************/
void Read_servos(void)
{
    static uint8_t init_count = 100;

    if (wait_for_all_channels_inactive == 0  &&  IOCIE == 0) {
        // Only process if there was no timer overflow. The timer overflows
        // every 65.536ms, which is way longer than the repeat duration of the
        // servo pulses, hence if the timer overflowed something was seriously
        // wrong.
        if (TMR1IF == 0) {
            // First we need to find the pulses and calculate the duration
            // for all channels.
            st = calculate_servo_pulse(1 << ST_BIT_NUMBER);
            th = calculate_servo_pulse(1 << TH_BIT_NUMBER);
            ch3 = calculate_servo_pulse(1 << CH3_BIT_NUMBER);

            /* If we receive a value > 0x880 then it can only be the special value
               0xa40, which is used for syncing as well as calibrating the oscillator.  
               
               0x880 has been chosen because we must ensure that it triggers regardless
               of how mis-tuned our local oscillator is.
             */
            if (ch3 > 0x880) {
                tuneOscillator(ch3);
            }
            
            // Only process the channels if all pulses are of reasonable duration
            if (are_servo_measurements_valid()) {
                adjust_endpoints();
            
                if (init_count) {
                    --init_count;
                    channel_data[0] = 0;
                    channel_data[1] = 0;
                    channel_data[2] = normalize_ch3() | (1 << STARTUP_MODE_NEUTRAL);
                    channel_data[3] = (SYNC_VALUE_NOMINAL >> 5);
                    
                    // At the end of the initialization phase capture the endpoints
                    if (init_count == 0) {
                        st_center = st;
                        th_center = th;
                    }                 
                }
                else {
                    channel_data[0] = normalize(st, st_center, st_epl, st_epr);
                    channel_data[1] = normalize(th, th_center, th_epl, th_epr);
                    channel_data[2] = normalize_ch3();
                    channel_data[3] = expansion_protocol_ch3();
                }
                flags.new_data = 1;
            }
        }        
        wait_for_all_channels_inactive = 1;
    }

    if (wait_for_all_channels_inactive) {
        if ((PORTA & CHANNEL_MASK) == 0) {
            wait_for_all_channels_inactive = 0;
            
            // Stop the timer and reset it. This way we don't have to deal with
            // overflow as the first edge will be at TIMER1=0. The interrupt
            // will switch on the timer at the first edge.
            TMR1ON = 0;
            TMR1L = 0;
            TMR1H = 0;
            TMR1IF = 0;
            
            // Initialized everything for the interrupt and enable it.
            channel_flags = 0;
            timestamp_count = 0;
            IOCIE = 1;
        }
    }
}

