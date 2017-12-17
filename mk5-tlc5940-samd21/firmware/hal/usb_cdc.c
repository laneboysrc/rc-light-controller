#include <stdbool.h>
#include <stdalign.h>
#include <string.h>

#include <hal.h>
#include <printf.h>

#include "hal_usb.h"
#include "usb_cdc.h"
#include "usb_descriptors.h"

// bRequest for CDC
#define CDC_SET_LINE_CODING 0x20
#define CDC_GET_LINE_CODING 0x21
#define CDC_SET_CONTROL_LINE_STATE 0x22
#define CDC_SEND_BREAK 0x23



typedef void (* usb_recv_callback_t)(uint8_t *data, size_t size);

extern void usb_set_endpoint_callback(uint8_t ep, void (*callback)(size_t size));
extern void usb_send(uint8_t ep, uint8_t *data, size_t size);
extern void usb_recv(uint8_t ep, uint8_t *data, size_t size);
extern void usb_control_recv(usb_recv_callback_t callback);
extern void usb_control_send(uint8_t *data, size_t size);
extern void usb_control_send_zlp(void);

bool usb_cdc_handle_class_request(usb_request_t *request);
void usb_cdc_configuration_callback(uint8_t config);


// static void usb_cdc_send_state_notify(void);
static void comm_callback(size_t size);
static void send_callback(size_t size);
static void recv_callback(size_t size);


#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define USB_BUFFER_SIZE 64

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


static usb_cdc_line_coding_t usb_cdc_line_coding = {
    .dwDTERate   = 115200,
    .bCharFormat = USB_CDC_1_STOP_BIT,
    .bParityType = USB_CDC_NO_PARITY,
    .bDataBits   = USB_CDC_8_DATA_BITS,
};

static alignas(4) usb_cdc_notify_serial_state_t usb_cdc_notify_message;
static int usb_cdc_serial_state;
static bool usb_cdc_comm_busy;


//-----------------------------------------------------------------------------
void usb_cdc_configuration_callback(uint8_t config)
{
    usb_recv(USB_CDC_EP_RECV, app_recv_buffer, sizeof(app_recv_buffer));
    // (void)config;

    printf("usb_cdc_configuration_callback %d\n", config);
}

//-----------------------------------------------------------------------------
void usb_cdc_line_coding_updated(usb_cdc_line_coding_t *line_coding)
{
    // uart_init(line_coding);
    (void) line_coding;
    printf("usb_cdc_line_coding_updated\n");

    printf("  baudrate: %d\n", line_coding->dwDTERate);
    printf("  parity: %d\n", line_coding->bParityType);
    printf("  bits: %d\n", line_coding->bDataBits);
    printf("  charFormat: 0x%x\n", line_coding->bCharFormat);
}

//-----------------------------------------------------------------------------
void usb_cdc_control_line_state_update(int line_state)
{
    printf("usb_cdc_control_line_state_update DTR=%d\n",
        (line_state & USB_CDC_CTRL_SIGNAL_DTE_PRESENT) ? 1 : 0);

}

//-----------------------------------------------------------------------------
void usb_cdc_init(void)
{
    usb_set_endpoint_callback(USB_CDC_EP_COMM, comm_callback);
    usb_set_endpoint_callback(USB_CDC_EP_SEND, send_callback);
    usb_set_endpoint_callback(USB_CDC_EP_RECV, recv_callback);

    usb_cdc_notify_message.request.bmRequestType =
        USB_IN_TRANSFER |
        USB_INTERFACE_RECIPIENT |
        USB_CLASS_REQUEST;
    usb_cdc_notify_message.request.bRequest = USB_CDC_NOTIFY_SERIAL_STATE;
    usb_cdc_notify_message.request.wValue = 0;
    usb_cdc_notify_message.request.wIndex = 0;
    usb_cdc_notify_message.request.wLength = sizeof(uint16_t);
    usb_cdc_notify_message.value = 0;

    usb_cdc_serial_state = 0;
    usb_cdc_comm_busy = false;

    usb_cdc_line_coding_updated(&usb_cdc_line_coding);
}

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
//     if (usb_cdc_comm_busy) {
//         return;
//     }

//     if (usb_cdc_serial_state != usb_cdc_notify_message.value) {
//         usb_cdc_comm_busy = true;
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

    usb_cdc_comm_busy = false;

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
    printf("send_callback\n");
}


//-----------------------------------------------------------------------------
static void recv_callback(size_t size)
{
    app_recv_buffer_ptr = 0;
    app_recv_buffer_size = size;

    printf("recv_callback %d \"", size);

    while (app_recv_buffer_size) {
        HAL_putc(STDOUT_DEBUG, app_recv_buffer[app_recv_buffer_ptr]);

        app_recv_buffer_ptr++;
        app_recv_buffer_size--;
    }
    printf("\"\n");

    // Data read, prepare endpoint to receive the next packet of data
    usb_recv(USB_CDC_EP_RECV, app_recv_buffer, sizeof(app_recv_buffer));

    // Dummy transfer to see if sending works
    if (app_recv_buffer[0] == '*') {
        static alignas(4) const char *message = "Hello world!\n";

        sprintf((char *)app_send_buffer, message);
        usb_send(USB_CDC_EP_SEND, app_send_buffer, 13);
    }
}


//-----------------------------------------------------------------------------
static void line_coding_handler(uint8_t *data, size_t size)
{
    if (size != sizeof(usb_cdc_line_coding_t)) {
       return;
    }

    usb_cdc_line_coding = *((usb_cdc_line_coding_t *)data);
    usb_cdc_line_coding_updated(&usb_cdc_line_coding);
}


//-----------------------------------------------------------------------------
bool usb_cdc_handle_class_request(usb_request_t *request)
{
    unsigned int length = request->wLength;
    printf("usb_cdc_handle_class_request\n");

    switch (request->bRequest) {
        case CDC_GET_LINE_CODING:
            length = MIN(length, sizeof(usb_cdc_line_coding_t));
            usb_control_send((uint8_t *)&usb_cdc_line_coding, length);
            return true;

        case CDC_SET_LINE_CODING:
            length = MIN(length, sizeof(usb_cdc_line_coding_t));
            usb_control_recv(line_coding_handler);
            return true;

        case CDC_SET_CONTROL_LINE_STATE:
            usb_cdc_control_line_state_update(request->wValue);
            usb_control_send_zlp();
            return true;

        case CDC_SEND_BREAK:
            printf("CDC_SEND_BREAK %d\n", request->wValue);
            usb_control_send_zlp();
            return true;

        default:
            printf("Unhanled class request %d\n", request->bRequest);
            return false;
    }
}



