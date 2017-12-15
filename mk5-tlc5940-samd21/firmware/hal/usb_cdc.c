#include <stdbool.h>
#include <stdalign.h>
#include <string.h>
#include <hal.h>

#include "hal_usb.h"
#include "usb_cdc.h"
#include "usb_descriptors.h"

/*- Prototypes --------------------------------------------------------------*/
static void usb_cdc_send_state_notify(void);
static void usb_cdc_ep_comm_callback(int size);
static void usb_cdc_ep_send_callback(int size);
static void usb_cdc_ep_recv_callback(int size);


bool usb_class_handle_request(usb_request_t *request);
void usb_set_callback(int ep, void (*callback)(int size));
void usb_send(int ep, uint8_t *data, int size);
void usb_recv(int ep, uint8_t *data, int size);
void usb_control_recv(void (*callback)(uint8_t *data, int size));
void usb_control_send_zlp(void);
void usb_control_send(uint8_t *data, int size);


#define MIN(a, b) (((a) < (b)) ? (a) : (b))


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
void usb_cdc_init(void)
{
  usb_set_callback(USB_CDC_EP_COMM, usb_cdc_ep_comm_callback);
  usb_set_callback(USB_CDC_EP_SEND, usb_cdc_ep_send_callback);
  usb_set_callback(USB_CDC_EP_RECV, usb_cdc_ep_recv_callback);

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
}

//-----------------------------------------------------------------------------
void usb_cdc_set_state(int mask)
{
  usb_cdc_serial_state |= mask;

  usb_cdc_send_state_notify();
}

//-----------------------------------------------------------------------------
void usb_cdc_clear_state(int mask)
{
  usb_cdc_serial_state &= ~mask;

  usb_cdc_send_state_notify();
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
}

//-----------------------------------------------------------------------------
static void usb_cdc_ep_send_callback(int size)
{
  usb_cdc_send_callback();
  (void)size;
}

//-----------------------------------------------------------------------------
static void usb_cdc_ep_recv_callback(int size)
{
  usb_cdc_recv_callback(size);
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
bool usb_class_handle_request(usb_request_t *request)
{
  unsigned int length = request->wLength;

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



