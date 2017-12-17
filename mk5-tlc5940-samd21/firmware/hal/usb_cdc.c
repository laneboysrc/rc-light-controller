#include <stdbool.h>
#include <stdalign.h>
#include <string.h>

#include <hal.h>
#include <printf.h>

#include <usb_api.h>
#include <usb_cdc.h>
#include <usb_descriptors.h>


#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define USB_BUFFER_SIZE 64


#define CDC_SET_LINE_CODING 0x20
#define CDC_GET_LINE_CODING 0x21
#define CDC_SET_CONTROL_LINE_STATE 0x22
#define CDC_SEND_BREAK 0x23

#define CDC_NOTIFY_SERIAL_STATE 0x20

#define CDC_CTRL_SIGNAL_DTE_PRESENT 1         // DTR
#define CDC_CTRL_SIGNAL_ACTIVATE_CARRIER 2    // RTS

#define CDC_SERIAL_STATE_DCD 1
#define CDC_SERIAL_STATE_DSR 2
#define CDC_SERIAL_STATE_BREAK 4
#define CDC_SERIAL_STATE_RING 8
#define CDC_SERIAL_STATE_FRAMING 16
#define CDC_SERIAL_STATE_PARITY 32
#define CDC_SERIAL_STATE_OVERRUN 64

#define CDC_1_STOP_BIT 0
#define CDC_1_5_STOP_BITS 1
#define CDC_2_STOP_BITS 2

#define CDC_NO_PARITY 0
#define CDC_ODD_PARITY 1
#define CDC_EVEN_PARITY 2
#define CDC_MARK_PARITY 3
#define CDC_SPACE_PARITY 4

#define CDC_5_DATA_BITS 5
#define CDC_6_DATA_BITS 6
#define CDC_7_DATA_BITS 7
#define CDC_8_DATA_BITS 8
#define CDC_16_DATA_BITS 16


typedef struct __attribute__((packed)) {
    uint32_t dwDTERate;
    uint8_t bCharFormat;
    uint8_t bParityType;
    uint8_t bDataBits;
} cdc_line_coding_t;

typedef struct __attribute__((packed)) {
    usb_request_t request;
    uint16_t      value;
} cdc_notify_serial_state_t;


static alignas(4) uint8_t app_recv_buffer[USB_BUFFER_SIZE];
static alignas(4) uint8_t app_send_buffer[USB_BUFFER_SIZE];
static int app_recv_buffer_size = 0;
static int app_recv_buffer_ptr = 0;
// static int app_send_buffer_ptr = 0;
static bool app_send_buffer_free = true;
// static bool app_send_zlp = false;
// static int app_system_time = 0;
// static int app_uart_timeout = 0;
// static bool app_status = false;
// static int app_status_timeout = 0;


static alignas(4) cdc_line_coding_t line_coding = {
    .dwDTERate   = 115200,
    .bCharFormat = CDC_1_STOP_BIT,
    .bParityType = CDC_NO_PARITY,
    .bDataBits   = CDC_8_DATA_BITS,
};

static alignas(4) cdc_notify_serial_state_t usb_cdc_notify_message;
static int usb_cdc_serial_state;
static bool comm_busy;



//-----------------------------------------------------------------------------
// void usb_cdc_set_state(int mask)
// {
//     usb_cdc_serial_state |= mask;

//     usb_cdc_send_state_notify();
//     printf("usb_cdc_set_state\n");
// }

//-----------------------------------------------------------------------------
// void usb_cdc_clear_state(int mask)
// {
//     usb_cdc_serial_state &= ~mask;

//     usb_cdc_send_state_notify();
//     printf("usb_cdc_clear_state\n");
// }


// FIXME: do we need this? Check STM32 implementation
//-----------------------------------------------------------------------------
// static void usb_cdc_send_state_notify(void)
// {
//     if (comm_busy) {
//         return;
//     }

//     if (usb_cdc_serial_state != usb_cdc_notify_message.value) {
//         comm_busy = true;
//         usb_cdc_notify_message.value = usb_cdc_serial_state;

//         usb_send(USB_CDC_EP_COMM, (uint8_t *)&usb_cdc_notify_message, sizeof(usb_cdc_notify_serial_state_t));
//     }
// }


//-----------------------------------------------------------------------------
static void comm_callback(size_t size)
{
    // const int one_shot = USB_CDC_SERIAL_STATE_BREAK | USB_CDC_SERIAL_STATE_RING |
    //   USB_CDC_SERIAL_STATE_FRAMING | USB_CDC_SERIAL_STATE_PARITY |
    //   USB_CDC_SERIAL_STATE_OVERRUN;

    comm_busy = false;

    // usb_cdc_notify_message.value &= ~one_shot;
    // usb_cdc_serial_state &= ~one_shot;

    // usb_cdc_send_state_notify();

    (void)size;
    printf("comm_callback\n");
}


//-----------------------------------------------------------------------------
static void send_callback(size_t size)
{
    (void)size;

    app_send_buffer_free = true;
    // printf("send_callback\n");
}


//-----------------------------------------------------------------------------
static void receive_callback(size_t size)
{
    app_recv_buffer_ptr = 0;
    app_recv_buffer_size = size;

    printf("receive_callback %d \"", size);

    while (app_recv_buffer_size) {
        HAL_putc(STDOUT_DEBUG, app_recv_buffer[app_recv_buffer_ptr]);

        app_recv_buffer_ptr++;
        app_recv_buffer_size--;
    }
    printf("\"\n");

    // Data read, prepare endpoint to receive the next packet of data
    USB_receive(USB_CDC_EP_RECEIVE, app_recv_buffer, sizeof(app_recv_buffer));

    // Dummy transfer to see if sending works
    if (app_recv_buffer[0] == '*') {
        static alignas(4) const char *message = "Hello world!\n";

        sprintf((char *)app_send_buffer, message);
        USB_send(USB_CDC_EP_SEND, app_send_buffer, 13);
    }
}


//-----------------------------------------------------------------------------
static void line_coding_callback(uint8_t *data, size_t size)
{
    if (size != sizeof(cdc_line_coding_t)) {
       return;
    }

    line_coding = *((cdc_line_coding_t *)data);

    printf("line_coding_callback\n");

    printf("  baudrate: %d\n", line_coding.dwDTERate);
    printf("  parity: %d\n", line_coding.bParityType);
    printf("  bits: %d\n", line_coding.bDataBits);
    printf("  charFormat: 0x%x\n", line_coding.bCharFormat);
}


//-----------------------------------------------------------------------------
void USB_CDC_init(void)
{
    USB_set_endpoint_callback(USB_CDC_EP_COMM, comm_callback);
    USB_set_endpoint_callback(USB_CDC_EP_SEND, send_callback);
    USB_set_endpoint_callback(USB_CDC_EP_RECEIVE, receive_callback);

    usb_cdc_notify_message.request.bmRequestType =
        USB_IN_TRANSFER |
        USB_INTERFACE_RECIPIENT |
        USB_CLASS_REQUEST;
    usb_cdc_notify_message.request.bRequest = CDC_NOTIFY_SERIAL_STATE;
    usb_cdc_notify_message.request.wValue = 0;
    usb_cdc_notify_message.request.wIndex = 0;
    usb_cdc_notify_message.request.wLength = sizeof(uint16_t);
    usb_cdc_notify_message.value = 0;

    usb_cdc_serial_state = 0;
    comm_busy = false;
}


//-----------------------------------------------------------------------------
void USB_CDC_configuration_callback(uint8_t config)
{
    // Prepare the receive endpoint to receive a packet of data
    USB_receive(USB_CDC_EP_RECEIVE, app_recv_buffer, sizeof(app_recv_buffer));

    printf("usb_cdc_configuration_callback %d\n", config);
}


//-----------------------------------------------------------------------------
bool USB_CDC_handle_class_request(usb_request_t *request)
{
    unsigned int length = request->wLength;
    printf("usb_cdc_handle_class_request\n");

    switch (request->bRequest) {
        case CDC_GET_LINE_CODING:
            length = MIN(length, sizeof(cdc_line_coding_t));
            USB_control_send((uint8_t *)&line_coding, length);
            return true;

        case CDC_SET_LINE_CODING:
            length = MIN(length, sizeof(cdc_line_coding_t));
            USB_control_receive(line_coding_callback);
            return true;

        case CDC_SET_CONTROL_LINE_STATE:
            printf("control_line_state_updated DTR=%d\n",
                (request->wValue & CDC_CTRL_SIGNAL_DTE_PRESENT) ? 1 : 0);
            USB_control_send_zlp();
            return true;

        case CDC_SEND_BREAK:
            printf("CDC_SEND_BREAK %d\n", request->wValue);
            USB_control_send_zlp();
            return true;

        default:
            printf("Unhanled class request %d\n", request->bRequest);
            return false;
    }
}



