#include <stdint.h>
#include <stdalign.h>

#include <hal.h>
#include <usb.h>
#include <usb_samd.h>
#include <class/cdc/cdc_standard.h>
#include <printf.h>

USB_ENDPOINTS(3)

#define USB_EP_CDC_NOTIFICATION 0x83
#define USB_EP_CDC_IN           0x84
#define USB_EP_CDC_OUT          0x04



#define BUF_SIZE 64

alignas(4) uint8_t usbserial_buf_in[BUF_SIZE];
alignas(4) uint8_t usbserial_buf_out[BUF_SIZE];

static void usbserial_init(void)
{
    printf("usbserial_init\n");
    usb_enable_ep(USB_EP_CDC_NOTIFICATION, USB_EP_TYPE_INTERRUPT, 8);
    usb_enable_ep(USB_EP_CDC_OUT, USB_EP_TYPE_BULK, 64);
    usb_enable_ep(USB_EP_CDC_IN, USB_EP_TYPE_BULK, 64);

    usb_ep_start_out(USB_EP_CDC_OUT, usbserial_buf_out, BUF_SIZE);
}


static void usbserial_out_completion(void)
{
    uint32_t len = usb_ep_out_length(USB_EP_CDC_OUT);
    // dma_sercom_start_tx(DMA_TERMINAL_TX, SERCOM_TERMINAL, usbserial_buf_out, len);

    printf("usbserial_out_completion %d \"", len);
    for (uint32_t i = 0; i < len ; i++) {
        HAL_putc(STDOUT_DEBUG, usbserial_buf_out[i]);
    }
    printf("\"\n");

    if (usbserial_buf_out[0] == '*') {

        sprintf((char *)usbserial_buf_in, "Hello world\n");
        usb_ep_start_in(USB_EP_CDC_IN, usbserial_buf_in, 12, false);
    }


    usb_ep_start_out(USB_EP_CDC_OUT, usbserial_buf_out, BUF_SIZE);
}

static void usbserial_in_completion(void)
{
    printf("usbserial_in_completion\n");
}





__attribute__((__aligned__(4))) const USB_DeviceDescriptor device_descriptor = {
    .bLength = sizeof(USB_DeviceDescriptor),
    .bDescriptorType = USB_DTYPE_Device,

    .bcdUSB                 = 0x0200,
    .bDeviceClass           = 0,
    .bDeviceSubClass        = USB_CSCP_NoDeviceSubclass,
    .bDeviceProtocol        = USB_CSCP_NoDeviceProtocol,

    .bMaxPacketSize0        = 64,
    .idVendor               = 0x1209,
    .idProduct              = 0x7551,
    .bcdDevice              = 0x0111,

    .iManufacturer          = 0x01,
    .iProduct               = 0x02,
    .iSerialNumber          = 0x03,

    .bNumConfigurations     = 1
};

uint16_t altsetting = 0;

#define INTERFACE_VENDOR 0
    #define ALTSETTING_FLASH 1
    #define ALTSETTING_PIPE 2
#define INTERFACE_CDC_CONTROL 1
#define INTERFACE_CDC_DATA 2

typedef struct ConfigDesc {
    USB_ConfigurationDescriptor Config;
    // USB_InterfaceDescriptor OffInterface;

    // USB_InterfaceDescriptor FlashInterface;
    // USB_EndpointDescriptor FlashInEndpoint;
    // USB_EndpointDescriptor FlashOutEndpoint;

    // USB_InterfaceDescriptor PipeInterface;
    // USB_EndpointDescriptor PipeInEndpoint;
    // USB_EndpointDescriptor PipeOutEndpoint;

    USB_InterfaceDescriptor CDC_control_interface;

    CDC_FunctionalHeaderDescriptor CDC_functional_header;
    CDC_FunctionalACMDescriptor CDC_functional_ACM;
    CDC_FunctionalUnionDescriptor CDC_functional_union;
    USB_EndpointDescriptor CDC_notification_endpoint;

    USB_InterfaceDescriptor CDC_data_interface;
    USB_EndpointDescriptor CDC_out_endpoint;
    USB_EndpointDescriptor CDC_in_endpoint;
}  __attribute__((packed)) ConfigDesc;

__attribute__((__aligned__(4))) const ConfigDesc configuration_descriptor = {
    .Config = {
        .bLength = sizeof(USB_ConfigurationDescriptor),
        .bDescriptorType = USB_DTYPE_Configuration,
        .wTotalLength  = sizeof(ConfigDesc),
        .bNumInterfaces = 3,
        .bConfigurationValue = 1,
        .iConfiguration = 0,
        .bmAttributes = USB_CONFIG_ATTR_BUSPOWERED,
        .bMaxPower = USB_CONFIG_POWER_MA(500)
    },
    .CDC_control_interface = {
        .bLength = sizeof(USB_InterfaceDescriptor),
        .bDescriptorType = USB_DTYPE_Interface,
        .bInterfaceNumber = INTERFACE_CDC_CONTROL,
        .bAlternateSetting = 0,
        .bNumEndpoints = 1,
        .bInterfaceClass = CDC_INTERFACE_CLASS,
        .bInterfaceSubClass = CDC_INTERFACE_SUBCLASS_ACM,
        .bInterfaceProtocol = 0,
        .iInterface = 0,
    },
    .CDC_functional_header = {
        .bLength = sizeof(CDC_FunctionalHeaderDescriptor),
        .bDescriptorType = USB_DTYPE_CSInterface,
        .bDescriptorSubtype = CDC_SUBTYPE_HEADER,
        .bcdCDC = 0x0110,
    },
    .CDC_functional_ACM = {
        .bLength = sizeof(CDC_FunctionalACMDescriptor),
        .bDescriptorType = USB_DTYPE_CSInterface,
        .bDescriptorSubtype = CDC_SUBTYPE_ACM,
        .bmCapabilities = 0x00,
    },
    .CDC_functional_union = {
        .bLength = sizeof(CDC_FunctionalUnionDescriptor),
        .bDescriptorType = USB_DTYPE_CSInterface,
        .bDescriptorSubtype = CDC_SUBTYPE_UNION,
        .bMasterInterface = INTERFACE_CDC_CONTROL,
        .bSlaveInterface = INTERFACE_CDC_DATA,
    },
    .CDC_notification_endpoint = {
        .bLength = sizeof(USB_EndpointDescriptor),
        .bDescriptorType = USB_DTYPE_Endpoint,
        .bEndpointAddress = USB_EP_CDC_NOTIFICATION,
        .bmAttributes = (USB_EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .wMaxPacketSize = 8,
        .bInterval = 0xFF
    },
    .CDC_data_interface = {
        .bLength = sizeof(USB_InterfaceDescriptor),
        .bDescriptorType = USB_DTYPE_Interface,
        .bInterfaceNumber = INTERFACE_CDC_DATA,
        .bAlternateSetting = 0,
        .bNumEndpoints = 2,
        .bInterfaceClass = CDC_INTERFACE_CLASS_DATA,
        .bInterfaceSubClass = 0,
        .bInterfaceProtocol = 0,
        .iInterface = 0,
    },
    .CDC_out_endpoint = {
        .bLength = sizeof(USB_EndpointDescriptor),
        .bDescriptorType = USB_DTYPE_Endpoint,
        .bEndpointAddress = USB_EP_CDC_OUT,
        .bmAttributes = (USB_EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .wMaxPacketSize = 64,
        .bInterval = 0x05
    },
    .CDC_in_endpoint = {
        .bLength = sizeof(USB_EndpointDescriptor),
        .bDescriptorType = USB_DTYPE_Endpoint,
        .bEndpointAddress = USB_EP_CDC_IN,
        .bmAttributes = (USB_EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .wMaxPacketSize = 64,
        .bInterval = 0x05
    },
};

__attribute__((__aligned__(4))) const USB_StringDescriptor language_string = {
    .bLength = USB_STRING_LEN(1),
    .bDescriptorType = USB_DTYPE_String,
    .bString = {USB_LANGUAGE_EN_US},
};

#define MSFT_ID 0xEE
#define MSFT_ID_STR u"\xEE"

__attribute__((__aligned__(4))) const USB_StringDescriptor msft_os = {
    .bLength = 18,
    .bDescriptorType = USB_DTYPE_String,
    .bString = u"MSFT100" MSFT_ID_STR
};

__attribute__((__aligned__(4))) uint8_t ep0_buffer[146];

// TODO: this doesn't need to be in RAM if it is copied into usb_ep0_out one packet at a time
const USB_MicrosoftCompatibleDescriptor msft_compatible = {
    .dwLength = sizeof(USB_MicrosoftCompatibleDescriptor) + (3 * sizeof(USB_MicrosoftCompatibleDescriptor_Interface)),
    .bcdVersion = 0x0100,
    .wIndex = 0x0004,
    .bCount = 3,
    .reserved = {0, 0, 0, 0, 0, 0, 0},
    .interfaces = {
        {
            .bFirstInterfaceNumber = 0,
            .reserved1 = 0x01,
            .compatibleID = "WINUSB\0\0",
            .subCompatibleID = {0, 0, 0, 0, 0, 0, 0, 0},
            .reserved2 = {0, 0, 0, 0, 0, 0},
        },
        {
            .bFirstInterfaceNumber = 1,
            .reserved1 = 0x01,
            .compatibleID = "WINUSB\0\0",
            .subCompatibleID = {0, 0, 0, 0, 0, 0, 0, 0},
            .reserved2 = {0, 0, 0, 0, 0, 0},
        },
        {
            .bFirstInterfaceNumber = 2,
            .reserved1 = 0x01,
            .compatibleID = "WINUSB\0\0",
            .subCompatibleID = {0, 0, 0, 0, 0, 0, 0, 0},
            .reserved2 = {0, 0, 0, 0, 0, 0},
        },
    }
};

typedef struct {
    uint32_t dwLength;
    uint16_t bcdVersion;
    uint16_t wIndex;
    uint16_t wCount;
    uint32_t dwPropLength;
    uint32_t dwType;
    uint16_t wNameLength;
    uint16_t name[21];
    uint32_t dwDataLength;
    uint16_t data[40];
    uint8_t _padding[2];
} __attribute__((packed)) USB_MicrosoftExtendedPropertiesDescriptor;

const USB_MicrosoftExtendedPropertiesDescriptor msft_extended = {
    .dwLength = 146,
    .bcdVersion = 0x0100,
    .wIndex = 0x05,
    .wCount = 0x01,
    .dwPropLength = 136,
    .dwType = 7,
    .wNameLength = 42,
    .name = u"DeviceInterfaceGUIDs\0",
    .dwDataLength = 80,
    .data = u"{3c33bbfd-71f9-4815-8b8f-7cd1ef928b3d}\0\0",
};

uint16_t usb_cb_get_descriptor(uint8_t type, uint8_t index, const uint8_t** ptr) {
    const void* address = NULL;
    uint16_t size = 0;

    printf("usb_cb_get_descriptor\n");

    switch (type) {
        case USB_DTYPE_Device:
            address = &device_descriptor;
            size    = sizeof(USB_DeviceDescriptor);
            break;
        case USB_DTYPE_Configuration:
            address = &configuration_descriptor;
            size    = sizeof(ConfigDesc);
            break;
        case USB_DTYPE_String:
            switch (index) {
                case 0x00:
                    address = &language_string;
                    break;
                case 0x01:
                    address = usb_string_to_descriptor((char *)"LANE Boys RC");
                    break;
                case 0x02:
                    address = usb_string_to_descriptor((char *)"RC Light Controller");
                    break;
                case 0x03:
                    address = samd_serial_number_string_descriptor();
                    break;
                case 0xee:
                    address = &msft_os;
                    break;

                default:
                    break;
            }
            size = (((USB_StringDescriptor*)address))->bLength;
            break;

        default:
            break;
    }

    *ptr = address;
    return size;
}

void usb_cb_reset(void) {
    printf("usb_cb_reset\n");
}

bool usb_cb_set_configuration(uint8_t config) {
    printf("usb_cb_set_configuration\n");
    if (config <= 1) {
        usbserial_init();
        return true;
    }
    return false;
}


// TODO: Use the version in the USB library after making it handle descriptors larger than 64 bytes
static inline void handle_msft_compatible(
    const USB_MicrosoftCompatibleDescriptor* msft_descriptor,
    const USB_MicrosoftExtendedPropertiesDescriptor* msft_extended_descriptor)
{
    uint16_t len;
    if (usb_setup.wIndex == 0x0005) {
        len = msft_extended_descriptor->dwLength;
        memcpy(ep0_buffer, msft_extended_descriptor, len);
    }
    else if (usb_setup.wIndex == 0x0004) {
        len = msft_descriptor->dwLength;
        memcpy(ep0_buffer, msft_descriptor, len);
    }
    else {
        usb_ep0_stall();
        return;
    }
    if (len > usb_setup.wLength) {
        len = usb_setup.wLength;
    }
    usb_ep_start_in(0x80, ep0_buffer, len, true);
    usb_ep0_out();
}

void usb_cb_control_setup(void) {
    uint8_t recipient = usb_setup.bmRequestType & USB_REQTYPE_RECIPIENT_MASK;

    printf("usb_cb_control_setup\n");

    if (recipient == USB_RECIPIENT_DEVICE) {
        switch(usb_setup.bRequest) {
            case MSFT_ID:
                handle_msft_compatible(&msft_compatible, &msft_extended);
                return;
            // case REQ_PWR: return req_gpio(usb_setup.wIndex, usb_setup.wValue);
            // case REQ_INFO: return req_info(usb_setup.wIndex);
            // case REQ_BOOT: return req_boot();
            // case REQ_RESET: return req_reset();
            // case REQ_OPENWRT_BOOT_STATUS: return req_boot_status();
            default:
                break;
        }
    }
    else if (recipient == USB_RECIPIENT_INTERFACE) {
        switch(usb_setup.bRequest) {
            case MSFT_ID:
                handle_msft_compatible(&msft_compatible, &msft_extended);
                return;

            default:
                break;
        }
    }

    usb_ep0_stall();
    return;
}

void usb_cb_control_in_completion(void) {
    printf("usb_cb_control_in_completion\n");
}

void usb_cb_control_out_completion(void) {
    printf("usb_cb_control_out_completion\n");
}

void usb_cb_completion(void) {
    // printf("usb_cb_completion\n");

    if (usb_ep_pending(USB_EP_CDC_OUT)) {
        usbserial_out_completion();
        usb_ep_handled(USB_EP_CDC_OUT);
    }

    if (usb_ep_pending(USB_EP_CDC_IN)) {
        usbserial_in_completion();
        usb_ep_handled(USB_EP_CDC_IN);
    }
}

bool usb_cb_set_interface(uint16_t interface, uint16_t new_altsetting) {
    printf("usb_cb_set_interface\n");
    if (interface == 0) {
        // if (new_altsetting > 2) {
        //     return false;
        // }

        // if (altsetting == ALTSETTING_FLASH) {
        //     flash_disable();
        //     init_breathing_animation();
        // } else if (altsetting == ALTSETTING_PIPE) {
        //     usbpipe_disable();
        // }

        // if (altsetting != ALTSETTING_FLASH && new_altsetting == ALTSETTING_FLASH) {
        //     bridge_disable();
        // } else if (altsetting == ALTSETTING_FLASH && new_altsetting != ALTSETTING_FLASH) {
        //     bridge_init();
        // }

        // if (new_altsetting == ALTSETTING_FLASH){
        //     cancel_breathing_animation();
        //     flash_init();
        // } else if (booted && new_altsetting == ALTSETTING_PIPE) {
        //     usbpipe_init();
        // }

        altsetting = new_altsetting;
        return true;
    }
    return false;
}
