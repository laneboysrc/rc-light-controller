#include <stdint.h>
#include <stdalign.h>

#include <hal.h>
#include <usb.h>
#include <usb_samd.h>
#include <class/dfu/dfu.h>
#include <usb_descriptors.h>
#include <usb_bos.h>

#define VENDOR_CODE_COMMAND 72      // Light Controller programmer command interface

#define CMD_DUT_POWER_OFF (10)
#define CMD_DUT_POWER_ON (11)
#define CMD_OUT_ISP_LOW (20)
#define CMD_OUT_ISP_HIGH (21)
#define CMD_OUT_ISP_TRISTATE (22)
#define CMD_CH3_LOW (23)
#define CMD_CH3_HIGH (24)
#define CMD_CH3_TRISTATE (25)
#define CMD_BAUDRATE_38400 (30)
#define CMD_BAUDRATE_115200 (31)
#define CMD_LED_OK_OFF (40)
#define CMD_LED_OK_ON (41)
#define CMD_LED_BUSY_OFF (42)
#define CMD_LED_BUSY_ON (43)
#define CMD_LED_ERROR_OFF (44)
#define CMD_LED_ERROR_ON (45)

bool test_interface_is_write_busy(void);
void test_interface_write(uint8_t length);
extern void add_uint8_to_usb_receive_buffer(uint8_t byte);

alignas(4) uint8_t test_interface_buf_in[BUF_SIZE];
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
        add_uint8_to_usb_receive_buffer(test_interface_buf_out[i]);
    }

    usb_ep_start_out(USB_EP_TEST_OUT, test_interface_buf_out, BUF_SIZE);
}

// ****************************************************************************
bool test_interface_is_write_busy(void)
{
    return test_interface_buf_in_busy;
}

// ****************************************************************************
void test_interface_write(uint8_t length)
{
    test_interface_buf_in_busy = true;
    usb_ep_start_in(USB_EP_TEST_IN, test_interface_buf_in, length, false);
}

// ****************************************************************************
static void test_interface_in_completion(void)
{
    test_interface_buf_in_busy = false;
}

// ****************************************************************************
static void command_handler(void)
{
    switch(usb_setup.wValue) {
        case 0:
            break;

        case 1:
            ep0_buf_in[0] = 42;
            usb_ep0_in(1);
            usb_ep0_out();
            return;

        case CMD_DUT_POWER_ON:
            HAL_gpio_clear(HAL_GPIO_POWER_SHORT);
            HAL_gpio_clear(HAL_GPIO_POWER_ENABLE);
            // Switch the TX and RX back to UART
            HAL_gpio_pmuxen(HAL_GPIO_TX);
            HAL_gpio_in(HAL_GPIO_RX);
            HAL_gpio_pmuxen(HAL_GPIO_RX);
            break;

        case CMD_DUT_POWER_OFF:
            HAL_gpio_set(HAL_GPIO_POWER_ENABLE);
            // Switch the UART TX and RX to GPIO output  and set it to low, so that
            // we don't power the light controller via the ST/RX pin!
            HAL_gpio_clear(HAL_GPIO_TXIO);
            HAL_gpio_out(HAL_GPIO_TXIO);
            HAL_gpio_pmuxen(HAL_GPIO_TXIO);
            HAL_gpio_clear(HAL_GPIO_RXIO);
            HAL_gpio_out(HAL_GPIO_RXIO);
            HAL_gpio_pmuxen(HAL_GPIO_RXIO);

            HAL_gpio_set(HAL_GPIO_POWER_SHORT);
            break;

        case CMD_LED_OK_ON:
            HAL_gpio_set(HAL_GPIO_LED_OK);
            break;

        case CMD_LED_OK_OFF:
            HAL_gpio_clear(HAL_GPIO_LED_OK);
            break;

        case CMD_LED_BUSY_ON:
            HAL_gpio_set(HAL_GPIO_LED_BUSY);
            break;

        case CMD_LED_BUSY_OFF:
            HAL_gpio_clear(HAL_GPIO_LED_BUSY);
            break;

        case CMD_LED_ERROR_ON:
            HAL_gpio_set(HAL_GPIO_LED_ERROR);
            break;

        case CMD_LED_ERROR_OFF:
            HAL_gpio_clear(HAL_GPIO_LED_ERROR);
            break;

        case CMD_OUT_ISP_LOW:
            HAL_gpio_clear(HAL_GPIO_OUT_ISP);
            HAL_gpio_out(HAL_GPIO_OUT_ISP);
            break;

        case CMD_OUT_ISP_HIGH:
            HAL_gpio_set(HAL_GPIO_OUT_ISP);
            HAL_gpio_out(HAL_GPIO_OUT_ISP);
            break;

        case CMD_OUT_ISP_TRISTATE:
            HAL_gpio_in(HAL_GPIO_OUT_ISP);
            break;

        case CMD_CH3_LOW:
            HAL_gpio_clear(HAL_GPIO_CH3);
            HAL_gpio_out(HAL_GPIO_CH3);
            break;

        case CMD_CH3_HIGH:
            HAL_gpio_set(HAL_GPIO_CH3);
            HAL_gpio_out(HAL_GPIO_CH3);
            break;

        case CMD_CH3_TRISTATE:
            HAL_gpio_in(HAL_GPIO_CH3);
            break;

        case CMD_BAUDRATE_38400:
            HAL_uart_set_baudrate(38400);
            break;

        case CMD_BAUDRATE_115200:
            HAL_uart_set_baudrate(115200);
            break;

        default:
            usb_ep0_stall();
            return;
    }

    usb_ep0_in(0);
    usb_ep0_out();
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
                    address = usb_string_to_descriptor((char *)"Programmer");
                    break;

                case USB_STRING_SERIAL_NUMBER:
                    address = samd_serial_number_string_descriptor();
                    break;

                case USB_STRING_DFU:
                    address = usb_string_to_descriptor((char *)"Programmer DFU");
                    break;

                case USB_STRING_TEST:
                    address = usb_string_to_descriptor((char *)"Programmer UART");
                    break;

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

            case VENDOR_CODE_COMMAND:
                command_handler();
                return;

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
    // At this point we would have data from the host when the host sent a
    // control out request. The data would be in ep0_buf_out[]
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

