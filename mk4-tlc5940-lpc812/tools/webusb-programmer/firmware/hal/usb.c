#include <stdint.h>
#include <stdalign.h>

#include <hal.h>
#include <usb.h>
#include <usb_samd.h>
#include <class/dfu/dfu.h>
#include <usb_descriptors.h>
#include <usb_bos.h>
#include <printf.h>

bool test_interface_is_write_busy(void);
void test_interface_write(uint8_t *data, uint8_t length);
extern void add_uint8_to_receive_buffer(uint8_t byte);

static alignas(4) uint8_t test_interface_buf_in[BUF_SIZE];
static alignas(4) uint8_t test_interface_buf_out[BUF_SIZE];
static bool test_interface_buf_in_busy = false;

static const uint8_t* data;
static uint16_t data_length;



// ****************************************************************************
static void send_descriptor_multi(void) {
    uint16_t transfer_length = data_length;

    if (transfer_length > USB_EP0_SIZE) {
        transfer_length = USB_EP0_SIZE;
    }

    memcpy(ep0_buf_in, data, transfer_length);
    usb_ep_start_in(0x80, ep0_buf_in, transfer_length, false);

    if (transfer_length == 0) {
        usb_ep0_out();
    }

    data_length -= transfer_length;
    data += transfer_length;
}


// ****************************************************************************
static void send_descriptor(const void * descriptor, uint16_t length)
{
    if (length > usb_setup.wLength) {
        length = usb_setup.wLength;
    }

    data_length = length;
    data = descriptor;
    send_descriptor_multi();
}


// ****************************************************************************
static void test_interface_init(void)
{
    test_interface_buf_in_busy = false;

    usb_enable_ep(USB_EP_TEST_OUT, USB_EP_TYPE_BULK, BUF_SIZE);
    usb_enable_ep(USB_EP_TEST_IN, USB_EP_TYPE_BULK, BUF_SIZE);

    usb_ep_start_out(USB_EP_TEST_OUT, test_interface_buf_out, BUF_SIZE);
}


// ****************************************************************************
static void test_interface_out_completion(void)
{
    uint32_t length = usb_ep_out_length(USB_EP_TEST_OUT);

    for (uint32_t i = 0; i < length ; i++) {
        add_uint8_to_receive_buffer(test_interface_buf_out[i]);
    }

    usb_ep_start_out(USB_EP_TEST_OUT, test_interface_buf_out, BUF_SIZE);
}

// ****************************************************************************
bool test_interface_is_write_busy(void)
{
    return test_interface_buf_in_busy;
}

// ****************************************************************************
void test_interface_write(uint8_t *data_to_send, uint8_t length)
{
    (void) data_to_send;
    (void) length;

    if (length) {
        test_interface_buf_in_busy = true;

        for (uint32_t i = 0; i < length ; i++) {
            test_interface_buf_in[i] = data_to_send[i];
        }
        usb_ep_start_in(USB_EP_TEST_IN, test_interface_buf_in, length, false);
    }
}

// ****************************************************************************
static void test_interface_in_completion(void)
{
    test_interface_buf_in_busy = false;
}


// ****************************************************************************
static void dfu_send_app_idle(void)
{
    DFU_StatusResponse* status = (DFU_StatusResponse*) ep0_buf_in;
    status->bStatus = 0;                // OK
    status->bwPollTimeout[0] = 0;
    status->bwPollTimeout[1] = 0;
    status->bwPollTimeout[2] = 0;
    status->bState = 0;                 // APP IDLE
    status->iString = 0;
    usb_ep0_in(sizeof(DFU_StatusResponse));
    usb_ep0_out();
}


// ****************************************************************************
uint16_t usb_cb_get_descriptor(uint8_t type, uint8_t index, const uint8_t** ptr) {
    const void* address = NULL;
    uint16_t size = 0;

    switch (type) {
        case USB_DTYPE_Device:
            address = &device_descriptor;
            size = sizeof(USB_DeviceDescriptor);
            break;

        case USB_DTYPE_Configuration:
            address = &configuration_descriptor;
            size = sizeof(configuration_descriptor_t);
            break;

        case USB_DTYPE_String:
            switch (index) {
                case USB_STRING_LANGUAGE:
                    address = &language_string;
                    break;

                case USB_STRING_MANUFACTURER:
                    address = usb_string_to_descriptor((char *)"LANE Boys RC");
                    break;

                case USB_STRING_PRODUCT:
                    address = usb_string_to_descriptor((char *)"RC Light Controller");
                    break;

                case USB_STRING_SERIAL_NUMBER:
                    address = samd_serial_number_string_descriptor();
                    break;

                case USB_STRING_DFU:
                    address = usb_string_to_descriptor((char *)"RC Light Controller (DFU)");
                    break;

                // case USB_STRING_TEST:
                //     address = usb_string_to_descriptor((char *)"RC Light Controller (Test)");
                //     break;

                default:
                    *ptr = NULL;
                    return 0;
            }
            size = (((USB_StringDescriptor *)address))->bLength;
            break;

        case USB_DTYPE_BOS:
            address = &bos_descriptor;
            size = sizeof(bos_descriptor_t);
            break;

        default:
            break;
    }

    *ptr = address;
    return size;
}


// ****************************************************************************
void usb_cb_reset(void) {
    test_interface_buf_in_busy = false;
}


// ****************************************************************************
bool usb_cb_set_configuration(uint8_t config) {
    if (config <= 1) {
        test_interface_init();
        return true;
    }
    return false;
}


// ****************************************************************************
bool usb_cb_set_interface(uint16_t interface, uint16_t new_altsetting) {
    (void) new_altsetting;

    switch (interface) {
        case USB_INTERFACE_TEST:
        case USB_INTERFACE_DFU:
            return true;

        default:
            return false;
    }
}


// ****************************************************************************
void usb_cb_control_setup(void) {
    uint8_t recipient = usb_setup.bmRequestType & USB_REQTYPE_RECIPIENT_MASK;
    uint8_t requestType = usb_setup.bmRequestType & USB_REQTYPE_TYPE_MASK;

    if (recipient == USB_RECIPIENT_INTERFACE) {
        // Forward all DFU related requests
        if (usb_setup.wIndex == USB_INTERFACE_DFU) {
            switch (usb_setup.bRequest) {
                case DFU_DETACH:
                    start_bootloader = 1;
                    usb_ep0_in(0);
                    usb_ep0_out();
                    return;

                case DFU_GETSTATUS:
                    dfu_send_app_idle();
                    return;

                default:
                    break;
            }
        }
    }

    else if (recipient == USB_RECIPIENT_DEVICE  &&  requestType == USB_REQTYPE_VENDOR) {
        switch(usb_setup.bRequest) {
            case VENDOR_CODE_WEBUSB:
                if (usb_setup.wIndex == WEBUSB_REQUEST_GET_URL) {
                    send_descriptor(&landing_page_descriptor, sizeof(landing_page_descriptor_t));
                    return;
                }
                break;

            case VENDOR_CODE_MS:
                if (usb_setup.wIndex == WINUSB_REQUEST_DESCRIPTOR) {
                    send_descriptor(&ms_os_20_descriptor, sizeof(ms_os_20_descriptor_t));
                    return;
                }
                break;

            default:
                break;
        }
    }

    usb_ep0_stall();
    return;
}


// ****************************************************************************
void usb_cb_control_in_completion(void) {
    send_descriptor_multi();
}


// ****************************************************************************
void usb_cb_control_out_completion(void) {
    // Nothing to do
}


// ****************************************************************************
void usb_cb_completion(void) {
    if (usb_ep_pending(USB_EP_TEST_OUT)) {
        test_interface_out_completion();
        usb_ep_handled(USB_EP_TEST_OUT);
    }

    if (usb_ep_pending(USB_EP_TEST_IN)) {
        test_interface_in_completion();
        usb_ep_handled(USB_EP_TEST_IN);
    }
}

