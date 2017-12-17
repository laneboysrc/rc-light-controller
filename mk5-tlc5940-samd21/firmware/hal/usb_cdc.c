#include <stdbool.h>
#include <stdalign.h>
#include <string.h>

#include <hal.h>
#include <printf.h>

#include "hal_usb.h"
#include "usb_cdc.h"
#include "usb_descriptors.h"

/*- Prototypes --------------------------------------------------------------*/
static void usb_cdc_send_state_notify(void);
static void usb_cdc_ep_comm_callback(int size);
static void usb_cdc_ep_send_callback(int size);
static void usb_cdc_ep_recv_callback(int size);


bool usb_handle_class_request(usb_request_t *request);
void usb_set_endpoint_callback(int ep, void (*callback)(int size));
void usb_send(int ep, uint8_t *data, int size);
void usb_recv(int ep, uint8_t *data, int size);
void usb_control_recv(void (*callback)(uint8_t *data, int size));
void usb_control_send_zlp(void);
void usb_control_send(uint8_t *data, int size);
void usb_configuration_callback(int config);


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


/*- Variables ---------------------------------------------------------------*/
static usb_cdc_line_coding_t usb_cdc_line_coding =
{
  .dwDTERate   = 115200,
  .bCharFormat = USB_CDC_1_STOP_BIT,
  .bParityType = USB_CDC_NO_PARITY,
  .bDataBits   = USB_CDC_8_DATA_BITS,
};

static alignas(4) usb_cdc_notify_serial_state_t usb_cdc_notify_message;
static int usb_cdc_serial_state;
static bool usb_cdc_comm_busy;

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
void usb_cdc_send_callback(void)
{
  app_send_buffer_free = true;
}

// //-----------------------------------------------------------------------------
// static void send_buffer(void)
// {
//   app_send_buffer_free = false;
//   app_send_zlp = (USB_BUFFER_SIZE == app_send_buffer_ptr);

//   usb_cdc_send(app_send_buffer, app_send_buffer_ptr);

//   app_send_buffer_ptr = 0;
// }

//-----------------------------------------------------------------------------
void usb_cdc_recv_callback(int size)
{
    app_recv_buffer_ptr = 0;
    app_recv_buffer_size = size;

    // printf("usb_cdc_recv_callback %d\n", size);

    while (app_recv_buffer_size) {
        HAL_putc(STDOUT_DEBUG, app_recv_buffer[app_recv_buffer_ptr]);

        app_recv_buffer_ptr++;
        app_recv_buffer_size--;
    }

    usb_cdc_recv(app_recv_buffer, sizeof(app_recv_buffer));

    if (app_recv_buffer[0] == '*') {
        app_send_buffer[0] = 'H';
        app_send_buffer[1] = 'e';
        app_send_buffer[2] = 'l';
        app_send_buffer[3] = 'l';
        app_send_buffer[4] = 'o';
        app_send_buffer[5] = '\n';
        usb_cdc_send(app_send_buffer, 6);
    }

}

//-----------------------------------------------------------------------------
void usb_configuration_callback(int config)
{
    usb_cdc_recv(app_recv_buffer, sizeof(app_recv_buffer));
    // (void)config;

    printf("usb_configuration_callback %d\n", config);
}

void usb_cdc_line_coding_updated(usb_cdc_line_coding_t *line_coding)
{
  // uart_init(line_coding);
  (void) line_coding;
}

//-----------------------------------------------------------------------------
void usb_cdc_control_line_state_update(int line_state)
{
  // update_status(line_state & USB_CDC_CTRL_SIGNAL_DTE_PRESENT);
  (void) line_state;
}

//-----------------------------------------------------------------------------
void usb_cdc_init(void)
{
  usb_set_endpoint_callback(USB_CDC_EP_COMM, usb_cdc_ep_comm_callback);
  usb_set_endpoint_callback(USB_CDC_EP_SEND, usb_cdc_ep_send_callback);
  usb_set_endpoint_callback(USB_CDC_EP_RECV, usb_cdc_ep_recv_callback);

  usb_cdc_notify_message.request.bmRequestType = USB_IN_TRANSFER |
      USB_INTERFACE_RECIPIENT | USB_CLASS_REQUEST;
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
void usb_cdc_send(uint8_t *data, int size)
{
  usb_send(USB_CDC_EP_SEND, data, size);
}

//-----------------------------------------------------------------------------
void usb_cdc_recv(uint8_t *data, int size)
{
    usb_recv(USB_CDC_EP_RECV, data, size);
    printf("usb_cdc_recv\n");
}

//-----------------------------------------------------------------------------
void usb_cdc_set_state(int mask)
{
  usb_cdc_serial_state |= mask;

  usb_cdc_send_state_notify();
    printf("usb_cdc_set_state\n");
}

//-----------------------------------------------------------------------------
void usb_cdc_clear_state(int mask)
{
  usb_cdc_serial_state &= ~mask;

  usb_cdc_send_state_notify();
    printf("usb_cdc_clear_state\n");
}

//-----------------------------------------------------------------------------
static void usb_cdc_send_state_notify(void)
{
  if (usb_cdc_comm_busy)
    return;

  if (usb_cdc_serial_state != usb_cdc_notify_message.value)
  {
    usb_cdc_comm_busy = true;
    usb_cdc_notify_message.value = usb_cdc_serial_state;

    usb_send(USB_CDC_EP_COMM, (uint8_t *)&usb_cdc_notify_message, sizeof(usb_cdc_notify_serial_state_t));
  }
}

//-----------------------------------------------------------------------------
static void usb_cdc_ep_comm_callback(int size)
{
  const int one_shot = USB_CDC_SERIAL_STATE_BREAK | USB_CDC_SERIAL_STATE_RING |
      USB_CDC_SERIAL_STATE_FRAMING | USB_CDC_SERIAL_STATE_PARITY |
      USB_CDC_SERIAL_STATE_OVERRUN;

  usb_cdc_comm_busy = false;

  usb_cdc_notify_message.value &= ~one_shot;
  usb_cdc_serial_state &= ~one_shot;

  usb_cdc_send_state_notify();

  (void)size;
    printf("usb_cdc_ep_comm_callback\n");
}

//-----------------------------------------------------------------------------
static void usb_cdc_ep_send_callback(int size)
{
  usb_cdc_send_callback();
  (void)size;
    printf("usb_cdc_ep_send_callback\n");
}

//-----------------------------------------------------------------------------
static void usb_cdc_ep_recv_callback(int size)
{
  usb_cdc_recv_callback(size);
    printf("usb_cdc_ep_recv_callback\n");
}

//-----------------------------------------------------------------------------
static void usb_cdc_set_line_coding_handler(uint8_t *data, int size)
{
  usb_cdc_line_coding_t *line_coding = (usb_cdc_line_coding_t *)data;

  if (sizeof(usb_cdc_line_coding_t) != size)
    return;

  usb_cdc_line_coding = *line_coding;

  usb_cdc_line_coding_updated(&usb_cdc_line_coding);
}

//-----------------------------------------------------------------------------
bool usb_handle_class_request(usb_request_t *request)
{
  unsigned int length = request->wLength;
    printf("usb_handle_class_request\n");

  switch ((request->bRequest << 8) | request->bmRequestType)
  {
    case USB_CMD(OUT, INTERFACE, CLASS, CDC_SET_LINE_CODING):
    {
      length = MIN(length, sizeof(usb_cdc_line_coding_t));

      usb_control_recv(usb_cdc_set_line_coding_handler);
    } break;

    case USB_CMD(IN, INTERFACE, CLASS, CDC_GET_LINE_CODING):
    {
      length = MIN(length, sizeof(usb_cdc_line_coding_t));

      usb_control_send((uint8_t *)&usb_cdc_line_coding, length);
    } break;

    case USB_CMD(OUT, INTERFACE, CLASS, CDC_SET_CONTROL_LINE_STATE):
    {
      usb_cdc_control_line_state_update(request->wValue);

      usb_control_send_zlp();
    } break;

    default:
      return false;
  }

  return true;
}



