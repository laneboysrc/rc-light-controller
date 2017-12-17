#include <stdbool.h>
#include <string.h>
#include <stdalign.h>
#include <hal.h>

#include "hal_usb.h"
#include "usb_cdc.h"
#include "usb_descriptors.h"
#include <printf.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))


typedef void (* usb_ep_callback_t)(size_t size);
typedef void (* usb_recv_callback_t)(uint8_t *data, size_t size);


void usb_init(void);
void usb_send(uint8_t ep, uint8_t *data, size_t size);
void usb_recv(uint8_t ep, uint8_t *data, size_t size);
void usb_control_send_zlp(void);
void usb_control_stall(void);
void usb_control_send(uint8_t *data, size_t size);
void usb_control_recv(usb_recv_callback_t callback);
void usb_set_endpoint_callback(uint8_t ep, usb_ep_callback_t callback);
void usb_task(void);

bool usb_cdc_handle_class_request(usb_request_t *request);
void usb_cdc_configuration_callback(int config);


#define SERIAL_NUMBER_WORD_0 (volatile uint32_t *)(0x0080a00c)
#define SERIAL_NUMBER_WORD_1 (volatile uint32_t *)(0x0080a040)
#define SERIAL_NUMBER_WORD_2 (volatile uint32_t *)(0x0080a044)
#define SERIAL_NUMBER_WORD_3 (volatile uint32_t *)(0x0080a048)


//  Standard requests
#define GET_STATUS                  0
#define CLEAR_FEATURE               1
#define SET_FEATURE                 3
#define SET_ADDRESS                 5
#define GET_DESCRIPTOR              6
#define SET_DESCRIPTOR              7
#define GET_CONFIGURATION           8
#define SET_CONFIGURATION           9
#define GET_INTERFACE               10
#define SET_INTERFACE               11

// bmRequestType
#define REQUEST_HOSTTODEVICE        0x00
#define REQUEST_DEVICETOHOST        0x80
#define REQUEST_DIRECTION           0x80

#define REQUEST_STANDARD            0x00
#define REQUEST_CLASS               0x20
#define REQUEST_VENDOR              0x40
#define REQUEST_TYPE                0x60

#define REQUEST_DEVICE              0x00
#define REQUEST_INTERFACE           0x01
#define REQUEST_ENDPOINT            0x02
#define REQUEST_OTHER               0x03
#define REQUEST_RECIPIENT           0x1F

// End point direction
#define OUT 0
#define IN 1

#define EPTYPE_DISABLED 0
#define EPTYPE_CONTROL 1
#define EPTYPE_ISOCHRONOUS 2
#define EPTYPE_BULK 3
#define EPTYPE_INTERRUPT 4
#define EPTYPE_DUAL_BANK 5

#define PCKSIZE_SIZE_8 0
#define PCKSIZE_SIZE_16 1
#define PCKSIZE_SIZE_32 2
#define PCKSIZE_SIZE_64 3
#define PCKSIZE_SIZE_128 4
#define PCKSIZE_SIZE_256 5
#define PCKSIZE_SIZE_512 6
#define PCKSIZE_SIZE_1023 7


static alignas(4) UsbDeviceDescriptor EP[USB_EPT_NUM];
static alignas(4) uint8_t usb_ctrl_in_buf[64];
static alignas(4) uint8_t usb_ctrl_out_buf[64];

static uint8_t selected_configuration = 0;

static usb_recv_callback_t control_recv_callback;
static usb_ep_callback_t ep_callbacks[USB_EPT_NUM];


//-----------------------------------------------------------------------------
static uint8_t get_PCKSIZE(uint16_t packet_size)
{
    if (packet_size <= 8) {
        return PCKSIZE_SIZE_8;
    }

    if (packet_size <= 16) {
        return PCKSIZE_SIZE_16;
    }

    if (packet_size <= 32) {
        return PCKSIZE_SIZE_32;
    }

    if (packet_size <= 64) {
        return PCKSIZE_SIZE_64;
    }

    if (packet_size <= 128) {
        return PCKSIZE_SIZE_128;
    }

    if (packet_size <= 256) {
        return PCKSIZE_SIZE_256;
    }

    if (packet_size <= 512) {
        return PCKSIZE_SIZE_512;
    }

    if (packet_size <= 1023) {
        return PCKSIZE_SIZE_1023;
    }

    return PCKSIZE_SIZE_8;
}


//-----------------------------------------------------------------------------
static uint8_t get_EPTYPE(uint8_t type)
{
    switch (type) {
        case USB_CONTROL_ENDPOINT:
            return EPTYPE_CONTROL;

        case USB_ISOCHRONOUS_ENDPOINT:
            return EPTYPE_ISOCHRONOUS;

        case USB_BULK_ENDPOINT:
            return EPTYPE_BULK;

        case USB_INTERRUPT_ENDPOINT:
            return EPTYPE_INTERRUPT;

        default:
            return EPTYPE_INTERRUPT;
    }
}


//-----------------------------------------------------------------------------
static void uint32_t_to_hex(uint32_t val, char* s) {
    static const char hex_chars[] = "0123456789abcdef";

    for (int8_t i = 7;  i >= 0;  i--, val >>= 4) {
        s[i] = hex_chars[val & 0x0f];
    }
}


//-----------------------------------------------------------------------------
static void usb_reset_endpoint(int ep, int dir)
{
  if (USB_IN_ENDPOINT == dir) {
        USB->DEVICE.DeviceEndpoint[ep].EPCFG.bit.EPTYPE1 = EPTYPE_DISABLED;
  }
  else {
        USB->DEVICE.DeviceEndpoint[ep].EPCFG.bit.EPTYPE0 = EPTYPE_DISABLED;
  }
}


//-----------------------------------------------------------------------------
static void usb_configure_endpoint(usb_endpoint_descriptor_t *desc)
{
    uint8_t ep;
    uint8_t dir;
    uint8_t type;
    uint8_t size;

    ep = desc->bEndpointAddress & USB_INDEX_MASK;
    dir = desc->bEndpointAddress & USB_DIRECTION_MASK;
    type = get_EPTYPE(desc->bmAttributes & 0x03);
    size = get_PCKSIZE(desc->wMaxPacketSize);

    usb_reset_endpoint(ep, dir);

    if (USB_IN_ENDPOINT == dir) {
        USB->DEVICE.DeviceEndpoint[ep].EPCFG.bit.EPTYPE1 = type;
        USB->DEVICE.DeviceEndpoint[ep].EPINTENSET.bit.TRCPT1 = 1;
        USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.DTGLIN = 1;
        USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.BK1RDY = 1;
        EP[ep].DeviceDescBank[IN].PCKSIZE.bit.SIZE = size;
    }
    else {
        USB->DEVICE.DeviceEndpoint[ep].EPCFG.bit.EPTYPE0 = type;
        USB->DEVICE.DeviceEndpoint[ep].EPINTENSET.bit.TRCPT0 = 1;
        USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.DTGLOUT = 1;
        USB->DEVICE.DeviceEndpoint[ep].EPSTATUSSET.bit.BK0RDY = 1;
        EP[ep].DeviceDescBank[OUT].PCKSIZE.bit.SIZE = size;
    }
}


//-----------------------------------------------------------------------------
static bool usb_endpoint_configured(int ep, int dir)
{
    if (USB_IN_ENDPOINT == dir) {
        return (USB->DEVICE.DeviceEndpoint[ep].EPCFG.bit.EPTYPE1 != EPTYPE_DISABLED);
    }
    else {
        return (USB->DEVICE.DeviceEndpoint[ep].EPCFG.bit.EPTYPE0 != EPTYPE_DISABLED);
    }
}


//-----------------------------------------------------------------------------
static int usb_endpoint_get_status(int ep, int dir)
{
    if (USB_IN_ENDPOINT == dir) {
        return USB->DEVICE.DeviceEndpoint[ep].EPSTATUS.bit.STALLRQ1;
    }
    else {
        return USB->DEVICE.DeviceEndpoint[ep].EPSTATUS.bit.STALLRQ0;
    }
}


//-----------------------------------------------------------------------------
static void usb_endpoint_set_feature(int ep, int dir)
{
    if (USB_IN_ENDPOINT == dir) {
        USB->DEVICE.DeviceEndpoint[ep].EPSTATUSSET.bit.STALLRQ1 = 1;
    }
    else {
        USB->DEVICE.DeviceEndpoint[ep].EPSTATUSSET.bit.STALLRQ0 = 1;
    }
}


//-----------------------------------------------------------------------------
static void usb_endpoint_clear_feature(int ep, int dir)
{
    if (USB_IN_ENDPOINT == dir) {
        if (USB->DEVICE.DeviceEndpoint[ep].EPSTATUS.bit.STALLRQ1) {
            USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.STALLRQ1 = 1;

            if (USB->DEVICE.DeviceEndpoint[ep].EPINTFLAG.bit.STALL1) {
                USB->DEVICE.DeviceEndpoint[ep].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_STALL1;
                USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.DTGLIN = 1;
            }
        }
    }
    else {
        if (USB->DEVICE.DeviceEndpoint[ep].EPSTATUS.bit.STALLRQ0) {
            USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.STALLRQ0 = 1;

            if (USB->DEVICE.DeviceEndpoint[ep].EPINTFLAG.bit.STALL0) {
                USB->DEVICE.DeviceEndpoint[ep].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_STALL0;
                USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.DTGLOUT = 1;
            }
        }
    }
}


//-----------------------------------------------------------------------------
static bool send_descriptor(usb_request_t *request)
{
    int type = request->wValue >> 8;
    int index = request->wValue & 0xff;
    unsigned length = request->wLength;


    switch (type) {
        case USB_DEVICE_DESCRIPTOR:
            length = MIN(length, usb_device_descriptor.bLength);
            fprintf(STDOUT_DEBUG, "Get dev desc %d\n", length);
            usb_control_send((uint8_t *)&usb_device_descriptor, length);
            return true;

        case USB_CONFIGURATION_DESCRIPTOR:
            length = MIN(length, usb_configuration_hierarchy.configuration.wTotalLength);
            fprintf(STDOUT_DEBUG, "Get config desc %d\n", length);
            usb_control_send((uint8_t *)&usb_configuration_hierarchy, length);
            return true;

        case USB_STRING_DESCRIPTOR:
            if (index == 0) {
                length = MIN(length, sizeof(usb_language_descriptor));
                fprintf(STDOUT_DEBUG, "Get language descriptor %d\n", length);
                usb_control_send((uint8_t *)&usb_language_descriptor, length);
                return true;
            }

            if (index < USB_STR_COUNT) {
                const char *str = usb_strings[index];
                size_t len;
                unsigned char buf[66];

                // FIXME: check that string does not cause buffer overflow
                // FIXME: check that string does not cause buffer overflow

                for (len = 0; *str; len++, str++) {
                    buf[2 + len*2] = *str;
                    buf[3 + len*2] = 0;
                }

                buf[0] = len*2 + 2;
                buf[1] = USB_STRING_DESCRIPTOR;

                length = MIN(length, buf[0]);

                fprintf(STDOUT_DEBUG, "Get string desc index %d %d 0x%x\n", index, length, str);
                usb_control_send(buf, length);
                return true;
            }
            return false;

        default:
            return false;
    }
}


//-----------------------------------------------------------------------------
static bool usb_handle_standard_request(usb_request_t *request)
{
    switch (request->bRequest) {
        case GET_CONFIGURATION:
            usb_control_send(&selected_configuration, sizeof(selected_configuration));
            return true;

        case GET_DESCRIPTOR:
            return send_descriptor(request);

        case SET_ADDRESS:
            usb_control_send_zlp();
            USB->DEVICE.DADD.reg = USB_DEVICE_DADD_ADDEN | USB_DEVICE_DADD_DADD(request->wValue);
            return true;

        case SET_CONFIGURATION:
            if ((request->bmRequestType & REQUEST_RECIPIENT) == REQUEST_DEVICE) {
                size_t size;
                usb_descriptor_header_t *desc;

                selected_configuration = request->wValue;

                size = usb_configuration_hierarchy.configuration.wTotalLength;
                desc = (usb_descriptor_header_t *)&usb_configuration_hierarchy;

                while (size) {
                    if (USB_ENDPOINT_DESCRIPTOR == desc->bDescriptorType) {
                        usb_configure_endpoint((usb_endpoint_descriptor_t *)desc);
                    }

                    size -= desc->bLength;
                    desc = (usb_descriptor_header_t *)((uint8_t *)desc + desc->bLength);
                }

                usb_cdc_configuration_callback(selected_configuration);
                usb_control_send_zlp();
                return true;
            }
            return false;

        case GET_STATUS:
            if (request->bmRequestType == REQUEST_DEVICE) {
                // FIXME: use control buffer instead of
                static uint16_t status = 0;

                usb_control_send((uint8_t *)&status, sizeof(status));
                return true;
            }
            else if (request->bmRequestType == REQUEST_ENDPOINT) {
                int ep = request->wIndex & USB_INDEX_MASK;
                int dir = request->wIndex & USB_DIRECTION_MASK;
                static uint16_t status = 0;

                if (usb_endpoint_configured(ep, dir)) {
                    status = usb_endpoint_get_status(ep, dir);
                    usb_control_send((uint8_t *)&status, sizeof(status));
                    return true;
                }
                else {
                    return false;
                }
            }
            return false;

        case SET_FEATURE:
            if (request->bmRequestType == REQUEST_DEVICE) {
                return false;
            }
            else if (request->bmRequestType == REQUEST_INTERFACE) {
                usb_control_send_zlp();
                return true;
            }
            else if (request->bmRequestType == REQUEST_ENDPOINT) {
                int ep = request->wIndex & USB_INDEX_MASK;
                int dir = request->wIndex & USB_DIRECTION_MASK;

                if (0 == request->wValue && ep && usb_endpoint_configured(ep, dir)) {
                    usb_endpoint_set_feature(ep, dir);
                    usb_control_send_zlp();
                    return true;
                }
                return false;
            }

        case CLEAR_FEATURE:
            if (request->bmRequestType == REQUEST_DEVICE) {
                return false;
            }
            else if (request->bmRequestType == REQUEST_INTERFACE) {
                usb_control_send_zlp();
                return true;
            }
            else if (request->bmRequestType == REQUEST_ENDPOINT) {
                int ep = request->wIndex & USB_INDEX_MASK;
                int dir = request->wIndex & USB_DIRECTION_MASK;

                if (0 == request->wValue && ep && usb_endpoint_configured(ep, dir)) {
                    usb_endpoint_clear_feature(ep, dir);
                    usb_control_send_zlp();
                    return true;
                }
                return false;
            }

        default:
            return false;
    }
}


//-----------------------------------------------------------------------------
static void service_end_of_reset(void)
{
    if (!USB->DEVICE.INTFLAG.bit.EORST) {
        return;
    }
    USB->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_EORST;

    USB->DEVICE.DADD.reg = USB_DEVICE_DADD_ADDEN;

    for (uint8_t i = 0; i < USB_EPT_NUM; i++) {
        usb_reset_endpoint(i, USB_IN_ENDPOINT);
        usb_reset_endpoint(i, USB_OUT_ENDPOINT);
    }

    USB->DEVICE.DeviceEndpoint[0].EPCFG.reg =
        USB_DEVICE_EPCFG_EPTYPE0(EPTYPE_CONTROL) |
        USB_DEVICE_EPCFG_EPTYPE1(EPTYPE_CONTROL);
    USB->DEVICE.DeviceEndpoint[0].EPSTATUSSET.bit.BK0RDY = 1;
    USB->DEVICE.DeviceEndpoint[0].EPSTATUSCLR.bit.BK1RDY = 1;

    EP[0].DeviceDescBank[IN].PCKSIZE.bit.SIZE = PCKSIZE_SIZE_64;

    EP[0].DeviceDescBank[OUT].ADDR.reg = (uint32_t)usb_ctrl_out_buf;
    EP[0].DeviceDescBank[OUT].PCKSIZE.bit.SIZE = PCKSIZE_SIZE_64;
    EP[0].DeviceDescBank[OUT].PCKSIZE.bit.MULTI_PACKET_SIZE = 64;
    EP[0].DeviceDescBank[OUT].PCKSIZE.bit.BYTE_COUNT = 0;

    USB->DEVICE.DeviceEndpoint[0].EPINTENSET.bit.RXSTP = 1;
    USB->DEVICE.DeviceEndpoint[0].EPINTENSET.bit.TRCPT0 = 1;
}


//-----------------------------------------------------------------------------
static void service_ep0_receive_setup(void)
{
    bool request_handled;
    usb_request_t *request;

    if (!USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.bit.RXSTP) {
        return;
    }

    request = (usb_request_t *)usb_ctrl_out_buf;

    if ((request->bmRequestType & REQUEST_TYPE) == REQUEST_STANDARD) {
        request_handled = usb_handle_standard_request(request);
    }
    else {
        request_handled = usb_cdc_handle_class_request(request);
    }

    if (request_handled) {
        EP[0].DeviceDescBank[OUT].PCKSIZE.bit.BYTE_COUNT = 0;
        USB->DEVICE.DeviceEndpoint[0].EPSTATUSCLR.bit.BK0RDY = 1;
        USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;
    }
    else {
        usb_control_stall();
    }

    USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_RXSTP;
}


//-----------------------------------------------------------------------------
static void service_ep0_transmit_complete(void)
{
    if (!USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.bit.TRCPT0) {
        return;
    }

    if (control_recv_callback) {
        control_recv_callback(usb_ctrl_out_buf, EP[0].DeviceDescBank[OUT].PCKSIZE.bit.BYTE_COUNT);
        control_recv_callback = NULL;
        usb_control_send_zlp();
    }

    USB->DEVICE.DeviceEndpoint[0].EPSTATUSSET.bit.BK0RDY = 1;
    USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;
}


//-----------------------------------------------------------------------------
static void service_endpoints(void)
{
    uint8_t endpoint_interrupts;

    // Load a list of pending endpoint interrupts, and mask out EP0 which we
    // handled seperately.
    endpoint_interrupts = USB->DEVICE.EPINTSMRY.reg & 0xfe;

    for (int i = 1; i < USB_EPT_NUM && endpoint_interrupts > 0; i++) {
        uint8_t flags;

        flags = USB->DEVICE.DeviceEndpoint[i].EPINTFLAG.reg;
        endpoint_interrupts &= ~(1 << i);

        // Transmit Complete 0 (OUT / data sent from host to device)
        if (flags & USB_DEVICE_EPINTFLAG_TRCPT0) {
            USB->DEVICE.DeviceEndpoint[i].EPSTATUSSET.bit.BK0RDY = 1;
            USB->DEVICE.DeviceEndpoint[i].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;

            if (ep_callbacks[i]) {
                ep_callbacks[i](EP[i].DeviceDescBank[OUT].PCKSIZE.bit.BYTE_COUNT);
            }
        }

        // Transmit Complete 1 (IN / data device to host)
        if (flags & USB_DEVICE_EPINTFLAG_TRCPT1) {
            USB->DEVICE.DeviceEndpoint[i].EPSTATUSCLR.bit.BK1RDY = 1;
            USB->DEVICE.DeviceEndpoint[i].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;

            if (ep_callbacks[i]) {
                ep_callbacks[i](0);
            }
        }
    }
}


//-----------------------------------------------------------------------------
void usb_init(void)
{
    GCLK->CLKCTRL.reg =
      GCLK_CLKCTRL_ID(USB_GCLK_ID) |
      GCLK_CLKCTRL_GEN(0) |
      GCLK_CLKCTRL_CLKEN;

    USB->DEVICE.CTRLA.bit.SWRST = 1;
    while (USB->DEVICE.SYNCBUSY.bit.SWRST);

    for (uint32_t i = 0; i < (sizeof(EP) / sizeof(uint32_t)); i++) {
        ((uint32_t *)EP)[i] = 0;
    }

    USB->DEVICE.DESCADD.reg = (uint32_t)EP;

    USB->DEVICE.CTRLA.bit.MODE = USB_CTRLA_MODE_DEVICE_Val;
    USB->DEVICE.CTRLA.bit.RUNSTDBY = 1;
    USB->DEVICE.CTRLB.bit.SPDCONF = USB_DEVICE_CTRLB_SPDCONF_FS_Val;
    USB->DEVICE.CTRLB.bit.DETACH = 0;

    USB->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_EORST;
    USB->DEVICE.DeviceEndpoint[0].EPINTENSET.bit.RXSTP = 1;

    USB->DEVICE.CTRLA.reg |= USB_CTRLA_ENABLE;

    for (int i = 0; i < USB_EPT_NUM; i++) {
        ep_callbacks[i] = NULL;
        usb_reset_endpoint(i, USB_IN_ENDPOINT);
        usb_reset_endpoint(i, USB_OUT_ENDPOINT);
    }

    // Build the USB serial number from the 128 bit SAM D21 serial number
    // by simply representing it as hex digits. This makes the serial number
    // 128 / 4 = 32 bytes/characters long (plus \0).
    uint32_t_to_hex(*SERIAL_NUMBER_WORD_0, usb_serial_number);
    uint32_t_to_hex(*SERIAL_NUMBER_WORD_1, usb_serial_number + 8);
    uint32_t_to_hex(*SERIAL_NUMBER_WORD_2, usb_serial_number + 16);
    uint32_t_to_hex(*SERIAL_NUMBER_WORD_3, usb_serial_number + 24);
}


//-----------------------------------------------------------------------------
void usb_task(void)
{
    service_end_of_reset();
    service_ep0_receive_setup();
    service_ep0_transmit_complete();
    service_endpoints();
}


//-----------------------------------------------------------------------------
void usb_send(uint8_t ep, uint8_t *data, size_t size)
{
    EP[ep].DeviceDescBank[IN].ADDR.reg = (uint32_t)data;
    EP[ep].DeviceDescBank[IN].PCKSIZE.bit.BYTE_COUNT = size;
    EP[ep].DeviceDescBank[IN].PCKSIZE.bit.MULTI_PACKET_SIZE = 0;

    USB->DEVICE.DeviceEndpoint[ep].EPSTATUSSET.bit.BK1RDY = 1;
}


//-----------------------------------------------------------------------------
void usb_recv(uint8_t ep, uint8_t *data, size_t size)
{
    EP[ep].DeviceDescBank[OUT].ADDR.reg = (uint32_t)data;
    EP[ep].DeviceDescBank[OUT].PCKSIZE.bit.MULTI_PACKET_SIZE = size;
    EP[ep].DeviceDescBank[OUT].PCKSIZE.bit.BYTE_COUNT = 0;

    USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.BK0RDY = 1;
}


//-----------------------------------------------------------------------------
void usb_control_send_zlp(void)
{
    EP[0].DeviceDescBank[IN].PCKSIZE.bit.BYTE_COUNT = 0;
    USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;
    USB->DEVICE.DeviceEndpoint[0].EPSTATUSSET.bit.BK1RDY = 1;

    while (0 == USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.bit.TRCPT1);
}


//-----------------------------------------------------------------------------
void usb_control_stall(void)
{
    USB->DEVICE.DeviceEndpoint[0].EPSTATUSSET.bit.STALLRQ1 = 1;
}


//-----------------------------------------------------------------------------
void usb_control_send(uint8_t *data, size_t size)
{
    // USB controller does not have access to the flash memory, so here we do
    // a manual multi-packet transfer. This way data can be located in in
    // the flash memory (big constant descriptors).

    while (size) {
        size_t transfer_size = MIN(size, usb_device_descriptor.bMaxPacketSize0);

        for (uint16_t i = 0; i < transfer_size; i++) {
            usb_ctrl_in_buf[i] = data[i];
        }

        EP[0].DeviceDescBank[IN].ADDR.reg = (uint32_t)usb_ctrl_in_buf;
        EP[0].DeviceDescBank[IN].PCKSIZE.bit.BYTE_COUNT = transfer_size;
        EP[0].DeviceDescBank[IN].PCKSIZE.bit.MULTI_PACKET_SIZE = 0;

        USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;
        USB->DEVICE.DeviceEndpoint[0].EPSTATUSSET.bit.BK1RDY = 1;

        while (!USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.bit.TRCPT1);

        size -= transfer_size;
        data += transfer_size;
    }
}


//-----------------------------------------------------------------------------
void usb_control_recv(usb_recv_callback_t callback)
{
    control_recv_callback = callback;
}


//-----------------------------------------------------------------------------
void usb_set_endpoint_callback(uint8_t ep, usb_ep_callback_t callback)
{
    ep_callbacks[ep] = callback;
}






