#include <stdint.h>
#include <stdalign.h>

#include <hal.h>
#include <usb.h>
#include <usb_samd.h>
#include <class/cdc/cdc_standard.h>
#include <class/dfu/dfu.h>
#include <printf.h>


// Transfer size conforms to one flash 'row', which consists of four 'pages'
#define DFU_TRANSFER_SIZE (FLASH_PAGE_SIZE * 4)
#define DFU_RUNTIME_PROTOCOL 1
#define DFU_DFU_PROTOCOL 2

USB_ENDPOINTS(3)

#define USB_STRING_LANGUAGE 0
#define USB_STRING_MANUFACTURER 1
#define USB_STRING_PRODUCT 2
#define USB_STRING_SERIAL_NUMBER 3
#define USB_STRING_DFU 4
#define USB_STRING_MSFT 0xee

// FIXME: is this the 'vendor id'?
#define MSFT_ID 0xee

#define USB_INTERFACE_CDC_CONTROL 0
#define USB_INTERFACE_CDC_DATA 1
#define USB_INTERFACE_DFU 2

#define USB_EP0 (USB_IN + 0)
#define USB_EP_CDC_NOTIFICATION (USB_IN + 1)
#define USB_EP_CDC_IN (USB_IN + 2)
#define USB_EP_CDC_OUT (USB_OUT + 3)

#define BUF_SIZE 64

alignas(4) uint8_t usbserial_buf_in[BUF_SIZE];
alignas(4) uint8_t usbserial_buf_out[BUF_SIZE];
alignas(4) uint8_t ep0_buffer[146];


__attribute__((__aligned__(4))) const USB_DeviceDescriptor device_descriptor = {
    .bLength = sizeof(USB_DeviceDescriptor),
    .bDescriptorType = USB_DTYPE_Device,

    .bcdUSB = 0x0200,
    .bDeviceClass = USB_CSCP_NoDeviceClass,
    .bDeviceSubClass = USB_CSCP_NoDeviceSubclass,
    .bDeviceProtocol = USB_CSCP_NoDeviceProtocol,

    .bMaxPacketSize0 = 64,
    .idVendor = 0x6666,
    .idProduct = 0xcab1,
    .bcdDevice = 0x0111,

    .iManufacturer = USB_STRING_MANUFACTURER,
    .iProduct = USB_STRING_PRODUCT,
    .iSerialNumber = USB_STRING_SERIAL_NUMBER,

    .bNumConfigurations = 1
};


typedef struct ConfigDesc {
    USB_ConfigurationDescriptor Config;

    USB_InterfaceDescriptor CDC_control_interface;
    CDC_FunctionalHeaderDescriptor CDC_functional_header;
    CDC_FunctionalACMDescriptor CDC_functional_ACM;
    CDC_FunctionalUnionDescriptor CDC_functional_union;
    USB_EndpointDescriptor CDC_notification_endpoint;

    USB_InterfaceDescriptor CDC_data_interface;
    USB_EndpointDescriptor CDC_out_endpoint;
    USB_EndpointDescriptor CDC_in_endpoint;

    USB_InterfaceDescriptor DFU_interface;
    DFU_FunctionalDescriptor DFU_functional;

}  __attribute__((packed)) ConfigDesc;

alignas(4) const ConfigDesc configuration_descriptor = {
    .Config = {
        .bLength = sizeof(USB_ConfigurationDescriptor),
        .bDescriptorType = USB_DTYPE_Configuration,
        .wTotalLength  = sizeof(ConfigDesc),
        .bNumInterfaces = 3,
        .bConfigurationValue = 1,
        .iConfiguration = 0,
        .bmAttributes = USB_CONFIG_ATTR_BUSPOWERED,
        .bMaxPower = USB_CONFIG_POWER_MA(100)
    },

    .CDC_control_interface = {
        .bLength = sizeof(USB_InterfaceDescriptor),
        .bDescriptorType = USB_DTYPE_Interface,
        .bInterfaceNumber = USB_INTERFACE_CDC_CONTROL,
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
        .bMasterInterface = USB_INTERFACE_CDC_CONTROL,
        .bSlaveInterface = USB_INTERFACE_CDC_DATA,
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
        .bInterfaceNumber = USB_INTERFACE_CDC_DATA,
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

    .DFU_interface = {
        .bLength = sizeof(USB_InterfaceDescriptor),
        .bDescriptorType = USB_DTYPE_Interface,
        .bInterfaceNumber = USB_INTERFACE_DFU,
        .bAlternateSetting = 0,
        .bNumEndpoints = 0,
        .bInterfaceClass = DFU_INTERFACE_CLASS,
        .bInterfaceSubClass = DFU_INTERFACE_SUBCLASS,
        .bInterfaceProtocol = DFU_RUNTIME_PROTOCOL,
        .iInterface = USB_STRING_DFU
    },
    .DFU_functional = {
        .bLength = sizeof(DFU_FunctionalDescriptor),
        .bDescriptorType = DFU_DESCRIPTOR_TYPE,
        .bmAttributes = DFU_ATTR_CAN_DOWNLOAD | DFU_ATTR_WILL_DETACH,
        .wDetachTimeout = 0,
        .wTransferSize = DFU_TRANSFER_SIZE,
        .bcdDFUVersion = 0x0110,
    }
};


typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    __CHAR16_TYPE__ bString[1];
} __attribute__ ((packed)) language_string_t;

alignas(4) const language_string_t language_string = {
    .bLength = USB_STRING_LEN(1),
    .bDescriptorType = USB_DTYPE_String,
    .bString = { USB_LANGUAGE_EN_US }
};


typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    __CHAR16_TYPE__ bString[7];
    uint8_t bVendorCode;
    uint8_t bPadding;
} __attribute__((packed)) msft_os_t;

alignas(4) const msft_os_t msft_os = {
    .bLength = 18,
    .bDescriptorType = USB_DTYPE_String,
    .bString = { u"MSFT100" },
    .bVendorCode = 0x42,
    .bPadding = 0
};


typedef struct {
    uint32_t dwLength;
    uint16_t bcdVersion;
    uint16_t wIndex;
    uint8_t bCount;
    uint8_t reserved[7];
    USB_MicrosoftCompatibleDescriptor_Interface interfaces[2];
} __attribute__((packed)) msft_compatible_t;


const msft_compatible_t msft_compatible = {
    .dwLength = sizeof(USB_MicrosoftCompatibleDescriptor) + (2 * sizeof(USB_MicrosoftCompatibleDescriptor_Interface)),
    .bcdVersion = 0x0100,
    .wIndex = 0x0004,
    .bCount = 2,
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
        }
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

// FIXME: check this...
const USB_MicrosoftExtendedPropertiesDescriptor msft_extended = {
    .dwLength = 146,
    .bcdVersion = 0x0100,
    .wIndex = 0x05,
    .wCount = 0x01,
    .dwPropLength = 136,
    .dwType = 7,
    .wNameLength = 42,
    .name = u"DeviceInterfaceGUIDs\0",
    .dwDataLength = 78,
    .data = u"{3c33bbfd-71f9-4815-8b8f-7cd1ef928b3d}",
};


// ****************************************************************************
static void handle_msft_compatible(
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

    usb_ep_start_in(USB_EP0, ep0_buffer, len, true);
    usb_ep0_out();
}


// ****************************************************************************
static void usbserial_init(void)
{
    usb_enable_ep(USB_EP_CDC_NOTIFICATION, USB_EP_TYPE_INTERRUPT, 8);
    usb_enable_ep(USB_EP_CDC_OUT, USB_EP_TYPE_BULK, 64);
    usb_enable_ep(USB_EP_CDC_IN, USB_EP_TYPE_BULK, 64);

    usb_ep_start_out(USB_EP_CDC_OUT, usbserial_buf_out, BUF_SIZE);
}


// ****************************************************************************
static void usbserial_out_completion(void)
{
    uint32_t len = usb_ep_out_length(USB_EP_CDC_OUT);

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


// ****************************************************************************
static void usbserial_in_completion(void)
{
    // Nothing to do
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

    printf("usb_cb_get_descriptor\n");

    switch (type) {
        case USB_DTYPE_Device:
            address = &device_descriptor;
            size    = sizeof(USB_DeviceDescriptor);
            break;

        case USB_DTYPE_Configuration:
            address = &configuration_descriptor;
            size = sizeof(ConfigDesc);
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
                    address = usb_string_to_descriptor((char *)"Firmware update");
                    break;

                case USB_STRING_MSFT:
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


// ****************************************************************************
void usb_cb_reset(void) {
    // Nothing to do
}


// ****************************************************************************
bool usb_cb_set_configuration(uint8_t config) {
    if (config <= 1) {
        usbserial_init();
        return true;
    }
    return false;
}


// ****************************************************************************
void usb_cb_control_setup(void) {
    uint8_t recipient = usb_setup.bmRequestType & USB_REQTYPE_RECIPIENT_MASK;

    if (recipient == USB_RECIPIENT_INTERFACE) {
        // Forward all DFU related requests
        if (usb_setup.wIndex == USB_INTERFACE_DFU) {
            switch (usb_setup.bRequest) {
                case DFU_DETACH:
                    start_bootloader = true;
                    return;

                case DFU_GETSTATUS:
                    dfu_send_app_idle();
                    return;

                default:
                    return;
            }
        }

        switch(usb_setup.bRequest) {
            case MSFT_ID:
                handle_msft_compatible((USB_MicrosoftCompatibleDescriptor *)&msft_compatible, &msft_extended);
                return;

            default:
                break;
        }
    }

    else if (recipient == USB_RECIPIENT_DEVICE) {
        switch(usb_setup.bRequest) {
            case MSFT_ID:
                handle_msft_compatible((USB_MicrosoftCompatibleDescriptor *)&msft_compatible, &msft_extended);
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
    // Nothing to do
}


// ****************************************************************************
void usb_cb_control_out_completion(void) {
    // Nothing to do
}


// ****************************************************************************
void usb_cb_completion(void) {
    if (usb_ep_pending(USB_EP_CDC_OUT)) {
        usbserial_out_completion();
        usb_ep_handled(USB_EP_CDC_OUT);
    }

    if (usb_ep_pending(USB_EP_CDC_IN)) {
        usbserial_in_completion();
        usb_ep_handled(USB_EP_CDC_IN);
    }
}


// ****************************************************************************
bool usb_cb_set_interface(uint16_t interface, uint16_t new_altsetting) {
    (void) new_altsetting;

    switch (interface) {
        case USB_INTERFACE_CDC_CONTROL:
        case USB_INTERFACE_DFU:
            return true;

        default:
            return false;
    }
}

