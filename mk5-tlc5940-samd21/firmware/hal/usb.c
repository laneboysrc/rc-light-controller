#include <stdbool.h>
#include <string.h>
#include <stdalign.h>

#include <hal.h>
#include <printf.h>

#include <usb_api.h>
#include <usb_cdc.h>
#include <usb_descriptors.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))


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
static alignas(4) uint8_t usb_ctrl_in_buf[MAX_PACKET_SIZE_0];
static alignas(4) uint8_t usb_ctrl_out_buf[MAX_PACKET_SIZE_0];

static uint8_t selected_configuration = 0;

static usb_receive_callback_t control_receive_callback;
static usb_ep_callback_t ep_callbacks[USB_EPT_NUM];


//-----------------------------------------------------------------------------
static uint8_t get_PCKSIZE(uint16_t packet_size)
{
    switch (packet_size) {
        case 8:
            return PCKSIZE_SIZE_8;

        case 16:
            return PCKSIZE_SIZE_16;

        case 32:
            return PCKSIZE_SIZE_32;

        case 64:
            return PCKSIZE_SIZE_64;

        case 128:
            return PCKSIZE_SIZE_128;

        case 256:
            return PCKSIZE_SIZE_256;

        case 512:
            return PCKSIZE_SIZE_512;

        case 1023:
            return PCKSIZE_SIZE_1023;

        default:
            return PCKSIZE_SIZE_8;
    }
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
static void usb_reset_endpoint(uint8_t ep, uint8_t dir)
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
        EP[ep].DeviceDescBank[USB_IN_TRANSFER].PCKSIZE.bit.SIZE = size;
    }
    else {
        USB->DEVICE.DeviceEndpoint[ep].EPCFG.bit.EPTYPE0 = type;
        USB->DEVICE.DeviceEndpoint[ep].EPINTENSET.bit.TRCPT0 = 1;
        USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.DTGLOUT = 1;
        USB->DEVICE.DeviceEndpoint[ep].EPSTATUSSET.bit.BK0RDY = 1;
        EP[ep].DeviceDescBank[USB_OUT_TRANSFER].PCKSIZE.bit.SIZE = size;
    }
}


//-----------------------------------------------------------------------------
static bool usb_endpoint_configured(uint8_t ep, uint8_t dir)
{
    if (USB_IN_ENDPOINT == dir) {
        return (USB->DEVICE.DeviceEndpoint[ep].EPCFG.bit.EPTYPE1 != EPTYPE_DISABLED);
    }
    else {
        return (USB->DEVICE.DeviceEndpoint[ep].EPCFG.bit.EPTYPE0 != EPTYPE_DISABLED);
    }
}


//-----------------------------------------------------------------------------
static uint8_t usb_endpoint_get_status(uint8_t ep, uint8_t dir)
{
    if (USB_IN_ENDPOINT == dir) {
        return USB->DEVICE.DeviceEndpoint[ep].EPSTATUS.bit.STALLRQ1;
    }
    else {
        return USB->DEVICE.DeviceEndpoint[ep].EPSTATUS.bit.STALLRQ0;
    }
}


//-----------------------------------------------------------------------------
static void usb_endpoint_set_feature(uint8_t ep, uint8_t dir)
{
    if (USB_IN_ENDPOINT == dir) {
        USB->DEVICE.DeviceEndpoint[ep].EPSTATUSSET.bit.STALLRQ1 = 1;
    }
    else {
        USB->DEVICE.DeviceEndpoint[ep].EPSTATUSSET.bit.STALLRQ0 = 1;
    }
}


//-----------------------------------------------------------------------------
static void usb_endpoint_clear_feature(uint8_t ep, uint8_t dir)
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


static void send_string(const char *ascii_string, size_t max_length)
{
    size_t length;

    // Maximum string length according to USB specification
    uint8_t string_descriptor[255];

    // Pointer to the 3rd byte of string_descriptor, which is the position of
    // the first unicode character
    uint16_t* unicode_string = (uint16_t*)(string_descriptor + 2);

    for (length = 0; *ascii_string; length++) {
        if (length >= (max_length / 2)) {
            break;
        }

        *unicode_string++ = (uint16_t)(*ascii_string++);
    }
    string_descriptor[0] = (2 * length) + 2;
    string_descriptor[1] = USB_STRING_DESCRIPTOR;

    USB_control_send(string_descriptor, string_descriptor[0]);
}


//-----------------------------------------------------------------------------
static bool send_descriptor(usb_request_t *request)
{
    uint8_t type = request->wValue >> 8;
    uint8_t index = request->wValue & 0xff;
    size_t length = request->wLength;


    switch (type) {
        case USB_DEVICE_DESCRIPTOR:
            length = MIN(length, usb_device_descriptor.bLength);
            fprintf(STDOUT_DEBUG, "Get dev desc %d\n", length);
            USB_control_send((uint8_t *)&usb_device_descriptor, length);
            return true;

        case USB_CONFIGURATION_DESCRIPTOR:
            length = MIN(length, usb_configuration_hierarchy.configuration.wTotalLength);
            fprintf(STDOUT_DEBUG, "Get config desc %d\n", length);
            USB_control_send((uint8_t *)&usb_configuration_hierarchy, length);
            return true;

        case USB_STRING_DESCRIPTOR:
            if (index == 0) {
                length = MIN(length, sizeof(usb_language_descriptor));
                fprintf(STDOUT_DEBUG, "Get language descriptor %d\n", length);
                USB_control_send((uint8_t *)&usb_language_descriptor, length);
                return true;
            }

            if (index < USB_STRING_COUNT) {
                send_string(usb_strings[index], length);
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
        case USB_GET_CONFIGURATION:
            USB_control_send(&selected_configuration, sizeof(selected_configuration));
            return true;

        case USB_GET_DESCRIPTOR:
            return send_descriptor(request);

        case USB_SET_ADDRESS:
            USB_control_send_zlp();
            USB->DEVICE.DADD.reg = USB_DEVICE_DADD_ADDEN | USB_DEVICE_DADD_DADD(request->wValue);
            return true;

        case USB_SET_CONFIGURATION:
            if ((request->bmRequestType & USB_REQUEST_RECIPIENT) == USB_REQUEST_DEVICE) {
                size_t size;
                usb_descriptor_header_t *desc;

                selected_configuration = request->wValue;

                // Initialize all endpoints according to the configuration
                // descriptor. Note that this code assumes that we only have
                // one configuration, i.e. selected_configuration is always 1
                size = usb_configuration_hierarchy.configuration.wTotalLength;
                desc = (usb_descriptor_header_t *)&usb_configuration_hierarchy;

                while (size) {
                    if (USB_ENDPOINT_DESCRIPTOR == desc->bDescriptorType) {
                        usb_configure_endpoint((usb_endpoint_descriptor_t *)desc);
                    }

                    size -= desc->bLength;
                    desc = (usb_descriptor_header_t *)((uint8_t *)desc + desc->bLength);
                }

                USB_CDC_configuration_callback(selected_configuration);
                USB_control_send_zlp();

                fprintf(STDOUT_DEBUG, "CONNECTED\n");

                return true;
            }
            return false;

        case USB_GET_STATUS:
            if (request->bmRequestType == USB_REQUEST_DEVICE) {
                // FIXME: use control buffer instead of
                static uint16_t status = 0;

                USB_control_send((uint8_t *)&status, sizeof(status));
                return true;
            }
            else if (request->bmRequestType == USB_REQUEST_ENDPOINT) {
                uint8_t ep = request->wIndex & USB_INDEX_MASK;
                uint8_t dir = request->wIndex & USB_DIRECTION_MASK;
                static uint16_t status = 0;

                if (usb_endpoint_configured(ep, dir)) {
                    status = usb_endpoint_get_status(ep, dir);
                    USB_control_send((uint8_t *)&status, sizeof(status));
                    return true;
                }
                else {
                    return false;
                }
            }
            return false;

        case USB_SET_FEATURE:
            if (request->bmRequestType == USB_REQUEST_DEVICE) {
                return false;
            }
            else if (request->bmRequestType == USB_REQUEST_INTERFACE) {
                USB_control_send_zlp();
                return true;
            }
            else if (request->bmRequestType == USB_REQUEST_ENDPOINT) {
                uint8_t ep = request->wIndex & USB_INDEX_MASK;
                uint8_t dir = request->wIndex & USB_DIRECTION_MASK;

                if (0 == request->wValue && ep && usb_endpoint_configured(ep, dir)) {
                    usb_endpoint_set_feature(ep, dir);
                    USB_control_send_zlp();
                    return true;
                }
                return false;
            }

        case USB_CLEAR_FEATURE:
            if (request->bmRequestType == USB_REQUEST_DEVICE) {
                return false;
            }
            else if (request->bmRequestType == USB_REQUEST_INTERFACE) {
                USB_control_send_zlp();
                return true;
            }
            else if (request->bmRequestType == USB_REQUEST_ENDPOINT) {
                uint8_t ep = request->wIndex & USB_INDEX_MASK;
                uint8_t dir = request->wIndex & USB_DIRECTION_MASK;

                if (0 == request->wValue && ep && usb_endpoint_configured(ep, dir)) {
                    usb_endpoint_clear_feature(ep, dir);
                    USB_control_send_zlp();
                    return true;
                }
                return false;
            }

        default:
            fprintf(STDOUT_DEBUG, "Unhandled std req %d", request->bRequest);
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

    selected_configuration = 0;

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

    EP[0].DeviceDescBank[USB_IN_TRANSFER].PCKSIZE.bit.SIZE = PCKSIZE_SIZE_64;

    EP[0].DeviceDescBank[USB_OUT_TRANSFER].ADDR.reg = (uint32_t)usb_ctrl_out_buf;
    EP[0].DeviceDescBank[USB_OUT_TRANSFER].PCKSIZE.bit.SIZE = PCKSIZE_SIZE_64;
    EP[0].DeviceDescBank[USB_OUT_TRANSFER].PCKSIZE.bit.MULTI_PACKET_SIZE = 64;
    EP[0].DeviceDescBank[USB_OUT_TRANSFER].PCKSIZE.bit.BYTE_COUNT = 0;

    USB->DEVICE.DeviceEndpoint[0].EPINTENSET.bit.RXSTP = 1;
    USB->DEVICE.DeviceEndpoint[0].EPINTENSET.bit.TRCPT0 = 1;

    fprintf(STDOUT_DEBUG, "DISCONNECTED\n");
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

    if ((request->bmRequestType & USB_REQUEST_TYPE) == USB_REQUEST_STANDARD) {
        request_handled = usb_handle_standard_request(request);
    }
    else {
        request_handled = USB_CDC_handle_class_request(request);
    }

    if (request_handled) {
        EP[0].DeviceDescBank[USB_OUT_TRANSFER].PCKSIZE.bit.BYTE_COUNT = 0;
        USB->DEVICE.DeviceEndpoint[0].EPSTATUSCLR.bit.BK0RDY = 1;
        USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;
    }
    else {
        USB_control_stall();
    }

    USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_RXSTP;
}


//-----------------------------------------------------------------------------
static void service_ep0_transmit_complete(void)
{
    if (!USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.bit.TRCPT0) {
        return;
    }

    if (control_receive_callback) {
        control_receive_callback(usb_ctrl_out_buf, EP[0].DeviceDescBank[USB_OUT_TRANSFER].PCKSIZE.bit.BYTE_COUNT);
        control_receive_callback = NULL;
        USB_control_send_zlp();
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

    for (uint8_t i = 1; (i < USB_EPT_NUM) && endpoint_interrupts; i++) {
        uint8_t flags;

        flags = USB->DEVICE.DeviceEndpoint[i].EPINTFLAG.reg;
        endpoint_interrupts &= ~(1 << i);

        // Transmit Complete 0 (OUT / data sent from host to device)
        if (flags & USB_DEVICE_EPINTFLAG_TRCPT0) {
            USB->DEVICE.DeviceEndpoint[i].EPSTATUSSET.bit.BK0RDY = 1;
            USB->DEVICE.DeviceEndpoint[i].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;

            if (ep_callbacks[i]) {
                ep_callbacks[i](EP[i].DeviceDescBank[USB_OUT_TRANSFER].PCKSIZE.bit.BYTE_COUNT);
            }
            else {
                fprintf(STDOUT_DEBUG, "No out ep handler %d\n", i);
            }

        }

        // Transmit Complete 1 (IN / data device to host)
        if (flags & USB_DEVICE_EPINTFLAG_TRCPT1) {
            USB->DEVICE.DeviceEndpoint[i].EPSTATUSCLR.bit.BK1RDY = 1;
            USB->DEVICE.DeviceEndpoint[i].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;

            if (ep_callbacks[i]) {
                ep_callbacks[i](0);
            }
            else {
                fprintf(STDOUT_DEBUG, "No in ep handler %d\n", i);
            }
        }
    }
}


//-----------------------------------------------------------------------------
void USB_init(void)
{
    // Local unnamed enumeration that holds the memory addresses of the
    // locations where the 128 bit serial number is located.
    enum {
        SERIAL_NUMBER_WORD_0 = 0x0080a00c,
        SERIAL_NUMBER_WORD_1 = 0x0080a040,
        SERIAL_NUMBER_WORD_2 = 0x0080a044,
        SERIAL_NUMBER_WORD_3 = 0x0080a048,
    };

    // Use GLKGEN0 (48 MHz) as clock source for USB
    GCLK->CLKCTRL.reg =
        GCLK_CLKCTRL_ID(USB_GCLK_ID) |
        GCLK_CLKCTRL_GEN(0) |
        GCLK_CLKCTRL_CLKEN;

    // Reset the USB peripheral
    USB->DEVICE.CTRLA.bit.SWRST = 1;
    while (USB->DEVICE.SYNCBUSY.bit.SWRST);

    // Build the USB serial number from the 128 bit SAM D21 serial number
    // by simply representing it as hex digits. This makes the serial number
    // 128 / 4 = 32 bytes/characters long (plus \0).
    uint32_t_to_hex(*(volatile uint32_t *)SERIAL_NUMBER_WORD_0, usb_serial_number);
    uint32_t_to_hex(*(volatile uint32_t *)SERIAL_NUMBER_WORD_1, usb_serial_number + 8);
    uint32_t_to_hex(*(volatile uint32_t *)SERIAL_NUMBER_WORD_2, usb_serial_number + 16);
    uint32_t_to_hex(*(volatile uint32_t *)SERIAL_NUMBER_WORD_3, usb_serial_number + 24);

    // Initialize the endpoint buffer
    for (size_t i = 0; i < sizeof(EP); i++) {
        ((uint8_t *)EP)[i] = 0;
    }

    USB->DEVICE.DESCADD.reg = (uint32_t)EP;

    // Set USB to device mode
    USB->DEVICE.CTRLA.reg =
        USB_CTRLA_MODE_DEVICE |
        USB_CTRLA_RUNSTDBY;

    // We operate as full speed device
    USB->DEVICE.CTRLB.reg = USB_DEVICE_CTRLB_SPDCONF_FS;

    // Enable the USB peripheral
    USB->DEVICE.CTRLA.reg |= USB_CTRLA_ENABLE;

    for (uint8_t i = 0; i < USB_EPT_NUM; i++) {
        ep_callbacks[i] = NULL;
        usb_reset_endpoint(i, USB_IN_ENDPOINT);
        usb_reset_endpoint(i, USB_OUT_ENDPOINT);
    }
}


//-----------------------------------------------------------------------------
void USB_set_endpoint_callback(uint8_t ep, usb_ep_callback_t callback)
{
    ep_callbacks[ep] = callback;
}


//-----------------------------------------------------------------------------
void USB_service(void)
{
    service_end_of_reset();
    service_ep0_receive_setup();
    service_ep0_transmit_complete();
    service_endpoints();
}


//-----------------------------------------------------------------------------
void USB_send(uint8_t ep, uint8_t *data, size_t size)
{
    EP[ep].DeviceDescBank[USB_IN_TRANSFER].ADDR.reg = (uint32_t)data;
    EP[ep].DeviceDescBank[USB_IN_TRANSFER].PCKSIZE.bit.BYTE_COUNT = size;
    EP[ep].DeviceDescBank[USB_IN_TRANSFER].PCKSIZE.bit.MULTI_PACKET_SIZE = 0;

    USB->DEVICE.DeviceEndpoint[ep].EPSTATUSSET.bit.BK1RDY = 1;
}


//-----------------------------------------------------------------------------
void USB_receive(uint8_t ep, uint8_t *data, size_t size)
{
    EP[ep].DeviceDescBank[USB_OUT_TRANSFER].ADDR.reg = (uint32_t)data;
    EP[ep].DeviceDescBank[USB_OUT_TRANSFER].PCKSIZE.bit.MULTI_PACKET_SIZE = size;
    EP[ep].DeviceDescBank[USB_OUT_TRANSFER].PCKSIZE.bit.BYTE_COUNT = 0;

    USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.BK0RDY = 1;
}


//-----------------------------------------------------------------------------
void USB_control_send(uint8_t *data, size_t size)
{
    // We send large amounts of data in chunks of MAX_PACKET_SIZE_0.
    // Note that this means that this function may take a long time, i.e.
    // multiple milliseconds.

    uint32_t start = milliseconds;
    uint32_t diff;

    while (size) {
        size_t transfer_size = MIN(size, MAX_PACKET_SIZE_0);

        for (uint16_t i = 0; i < transfer_size; i++) {
            usb_ctrl_in_buf[i] = data[i];
        }

        EP[0].DeviceDescBank[USB_IN_TRANSFER].ADDR.reg = (uint32_t)usb_ctrl_in_buf;
        EP[0].DeviceDescBank[USB_IN_TRANSFER].PCKSIZE.bit.BYTE_COUNT = transfer_size;
        EP[0].DeviceDescBank[USB_IN_TRANSFER].PCKSIZE.bit.MULTI_PACKET_SIZE = 0;

        USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;
        USB->DEVICE.DeviceEndpoint[0].EPSTATUSSET.bit.BK1RDY = 1;

        // Wait for the transfer to complete
        while (!USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.bit.TRCPT1);

        size -= transfer_size;
        data += transfer_size;
    }

    diff = milliseconds - start;
    if (diff) {
        printf("USB_control_send took %d ms\n", milliseconds - start);
    }
}


//-----------------------------------------------------------------------------
void USB_control_send_zlp(void)
{
    EP[0].DeviceDescBank[USB_IN_TRANSFER].PCKSIZE.bit.BYTE_COUNT = 0;
    USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;
    USB->DEVICE.DeviceEndpoint[0].EPSTATUSSET.bit.BK1RDY = 1;

    while (0 == USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.bit.TRCPT1);
}


//-----------------------------------------------------------------------------
void USB_control_receive(usb_receive_callback_t callback)
{
    control_receive_callback = callback;
}


//-----------------------------------------------------------------------------
void USB_control_stall(void)
{
    USB->DEVICE.DeviceEndpoint[0].EPSTATUSSET.bit.STALLRQ1 = 1;
}

