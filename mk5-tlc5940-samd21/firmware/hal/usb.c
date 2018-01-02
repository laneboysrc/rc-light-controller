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

#define USB_DTYPE_BOS 15
#define WEBUSB_REQUEST_GET_URL 2

// Our arbitrary vendor code, used to retrieve the Microsoft Compatible descritpros
#define VENDOR_CODE 42

#define USB_NUMBER_OF_INTERFACES 3
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
// IMPORTANT: the Endpoint 0 buffer must be able to hold a full copy of the
// USB_MicrosoftExtendedPropertiesDescriptor descriptor!
alignas(4) uint8_t ep0_buffer[256];


static alignas(4) const USB_DeviceDescriptor device_descriptor = {
    .bLength = sizeof(USB_DeviceDescriptor),
    .bDescriptorType = USB_DTYPE_Device,

    .bcdUSB = 0x0200,
    .bDeviceClass = USB_CSCP_NoDeviceClass,
    .bDeviceSubClass = USB_CSCP_NoDeviceSubclass,
    .bDeviceProtocol = USB_CSCP_NoDeviceProtocol,

    .bMaxPacketSize0 = 64,
    .idVendor = 0x6666,
    .idProduct = 0xcab1,
    .bcdDevice = 0x0102,

    .iManufacturer = USB_STRING_MANUFACTURER,
    .iProduct = USB_STRING_PRODUCT,
    .iSerialNumber = USB_STRING_SERIAL_NUMBER,

    .bNumConfigurations = 1
};


typedef struct {
    USB_ConfigurationDescriptor Config;

    USB_InterfaceAssociationDescriptor CDC_interface_association;

    USB_InterfaceDescriptor CDC_control_interface;
    CDC_FunctionalHeaderDescriptor CDC_functional_header;
    CDC_FunctionalACMDescriptor CDC_functional_ACM;
    CDC_FunctionalUnionDescriptor CDC_functional_union;
    USB_EndpointDescriptor CDC_notification_endpoint;

    USB_InterfaceDescriptor CDC_data_interface;
    USB_EndpointDescriptor CDC_out_endpoint;
    USB_EndpointDescriptor CDC_in_endpoint;

    USB_InterfaceAssociationDescriptor DFU_interface_association;

    USB_InterfaceDescriptor DFU_interface;
    DFU_FunctionalDescriptor DFU_functional;

}  __attribute__((packed)) configuration_descriptor_t;

static alignas(4) const configuration_descriptor_t configuration_descriptor = {
    .Config = {
        .bLength = sizeof(USB_ConfigurationDescriptor),
        .bDescriptorType = USB_DTYPE_Configuration,
        .wTotalLength  = sizeof(configuration_descriptor_t),
        .bNumInterfaces = USB_NUMBER_OF_INTERFACES,
        .bConfigurationValue = 1,
        .iConfiguration = 0,
        .bmAttributes = USB_CONFIG_ATTR_BUSPOWERED,
        .bMaxPower = USB_CONFIG_POWER_MA(100)
    },

    .CDC_interface_association = {
        .bLength = sizeof(USB_InterfaceAssociationDescriptor),
        .bDescriptorType = USB_DTYPE_InterfaceAssociation,
        .bFirstInterface = USB_INTERFACE_CDC_CONTROL,
        .bInterfaceCount = 2,
        .bFunctionClass = CDC_INTERFACE_CLASS,
        .bFunctionSubClass = CDC_INTERFACE_SUBCLASS_ACM,
        .bFunctionProtocol = 0,         // USB_CDC_PROTOCOL_NONE
        .iFunction = 0,
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

    .DFU_interface_association = {
        .bLength = sizeof(USB_InterfaceAssociationDescriptor),
        .bDescriptorType = USB_DTYPE_InterfaceAssociation,
        .bFirstInterface = USB_INTERFACE_DFU,
        .bInterfaceCount = 1,
        .bFunctionClass = DFU_INTERFACE_CLASS,
        .bFunctionSubClass = DFU_INTERFACE_SUBCLASS,
        .bFunctionProtocol = DFU_RUNTIME_PROTOCOL,
        .iFunction = 0,
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

static alignas(4) const language_string_t language_string = {
    .bLength = USB_STRING_LEN(1),
    .bDescriptorType = USB_DTYPE_String,
    .bString = { USB_LANGUAGE_EN_US }
};


// WCID descriptors
// A WCID device, where WCID stands for "Windows Compatible ID", is an USB
// device that provides extra information to a Windows system, in order to
// facilitate automated driver installation and, in most circumstances, allow
// immediate access.
//
// Info: https://github.com/pbatard/libwdi/wiki/WCID-Devices
typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    __CHAR16_TYPE__ bString[7];
    uint8_t bVendorCode;
    uint8_t bPadding;
} __attribute__((packed)) msft_os_t;

static alignas(4) const msft_os_t msft_os = {
    .bLength = 18,
    .bDescriptorType = USB_DTYPE_String,
    .bString = { u"MSFT100" },
    .bVendorCode = VENDOR_CODE,
    .bPadding = 0
};

typedef struct {
    uint32_t dwLength;
    uint16_t bcdVersion;
    uint16_t wIndex;
    uint8_t bCount;
    uint8_t reserved[7];
    USB_MicrosoftCompatibleDescriptor_Interface interfaces[USB_NUMBER_OF_INTERFACES];
} __attribute__((packed)) USB_MicrosoftCompatibleDescriptor_t;

static const USB_MicrosoftCompatibleDescriptor_t msft_compatible = {
    .dwLength = sizeof(USB_MicrosoftCompatibleDescriptor_t),
    .bcdVersion = 0x0100,
    .wIndex = 4,
    .bCount = USB_NUMBER_OF_INTERFACES,
    .reserved = {0, 0, 0, 0, 0, 0, 0},
    .interfaces = {
        {
            .bFirstInterfaceNumber = 0,
            .reserved1 = 1,
            .compatibleID = "WINUSB\0\0",
            .subCompatibleID = {0, 0, 0, 0, 0, 0, 0, 0},
            .reserved2 = {0, 0, 0, 0, 0, 0},
        },
        {
            .bFirstInterfaceNumber = 1,
            .reserved1 = 1,
            .compatibleID = "WINUSB\0\0",
            .subCompatibleID = {0, 0, 0, 0, 0, 0, 0, 0},
            .reserved2 = {0, 0, 0, 0, 0, 0},
        },
        {
            .bFirstInterfaceNumber = 2,
            .reserved1 = 1,
            .compatibleID = "WINUSB\0\0",
            .subCompatibleID = {0, 0, 0, 0, 0, 0, 0, 0},
            .reserved2 = {0, 0, 0, 0, 0, 0},
        }
    }
};

// Microsoft Extended Properties Feature Descriptor
#define M1_NAME u"DeviceInterfaceGUID"
// Ports | Windows 10 only
#define M1_GUID u"{4d36e978-e325-11ce-bfc1-08002be10318}"
// Modem | Windows 7 and up, but requires custom INF that references mdmcpq.inf
// See https://msdn.microsoft.com/en-us/library/windows/hardware/ff538820(v=vs.85).aspx
// #define M1_GUID u"{4d36e96d-e325-11ce-bfc1-08002be10318}"
typedef struct {
    uint32_t dwLength;
    uint16_t bcdVersion;
    uint16_t wIndex;
    uint16_t wCount;

    uint32_t dwPropLength;
    uint32_t dwType;
    uint16_t wNameLength;
    __CHAR16_TYPE__ name[sizeof(M1_NAME)/2];
    uint32_t dwDataLength;
    __CHAR16_TYPE__ data[sizeof(M1_GUID)/2];
    uint8_t _padding[2];
} __attribute__((packed)) USB_MicrosoftExtendedPropertiesDescriptor_t;

const USB_MicrosoftExtendedPropertiesDescriptor_t msft_extended_properties = {
    .dwLength = sizeof(USB_MicrosoftExtendedPropertiesDescriptor_t),
    .bcdVersion = 0x0100,
    .wIndex = 5,
    .wCount = 1,

    .dwPropLength = sizeof(M1_NAME) + sizeof(M1_GUID) + 14,
    .dwType = 1,                    // Unicode string
    .wNameLength = sizeof(M1_NAME),
    .name = M1_NAME,
    .dwDataLength = sizeof(M1_GUID),
    .data = M1_GUID,
};

// WebUSB descriptor (Binary Object Store descriptor)
const uint8_t BOS_Descriptor[] = {
    0x05,           // Length
    0x0F,           // Binary Object Store descriptor
    0x39, 0x00,     // Total length
    0x01,           // Number of device capabilities

    // WebUSB Platform Capability descriptor (bVendorCode == 0x01).
    24,             // Length
    16,             // Value for "Device Capability" descriptor
    5,              // Value to signify a "Platform" capability descriptor
    0x00,           // Reserved
    0x38, 0xB6, 0x08, 0x34, 0xA9, 0x09, 0xA0, 0x47,
    0x8B, 0xFD, 0xA0, 0x76, 0x88, 0x15, 0xB6, 0x65,  // WebUSB GUID
    0x00, 0x01,     // Version 1.0
    0x01,           // Vendor request code

    0x01,           // Landing page URL is available
};


// ****************************************************************************
static void send_microsoft_descriptors(const USB_MicrosoftCompatibleDescriptor_t* msft_descriptor)
{
    uint16_t len;

    len = msft_descriptor->dwLength;
    if (len > usb_setup.wLength) {
        len = usb_setup.wLength;
    }

    memcpy(ep0_buffer, msft_descriptor, len);

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

    printf("usb_cb_get_descriptor type=%d index=%d len=%d\n", type, index, usb_setup.wLength);

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

        case USB_DTYPE_BOS:
            address = &BOS_Descriptor;
            size = sizeof(BOS_Descriptor);
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
    uint8_t requestType = usb_setup.bmRequestType & USB_REQTYPE_TYPE_MASK;

    printf("usb_cb_control_setup bmRequestType=%x bRequest=%d wIndex=%d len=%d\n",
        usb_setup.bmRequestType, usb_setup.bRequest, usb_setup.wIndex, usb_setup.wLength);

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
                    return;
            }
        }

        switch(usb_setup.bRequest) {
            case VENDOR_CODE:
                if (usb_setup.wIndex == 0x0005) {
                    send_microsoft_descriptors((const USB_MicrosoftCompatibleDescriptor_t*) &msft_extended_properties);
                    return;
                }
                break;

            default:
                break;
        }
    }

    else if (recipient == USB_RECIPIENT_DEVICE) {
        if (requestType == USB_REQTYPE_VENDOR &&
                usb_setup.bRequest == 0x01 &&       // VENDOR_CODE! See BOS descriptor
                usb_setup.wIndex == WEBUSB_REQUEST_GET_URL) {
            // if (setup.wValueL != 1)
            //     return false;
            // uint8_t urlLength = strlen(landingPageUrl);
            // uint8_t descriptorLength = urlLength + 3;
            // if (USB_SendControl(0, &descriptorLength, 1) < 0)
            //     return false;
            // uint8_t descriptorType = 3;
            // if (USB_SendControl(0, &descriptorType, 1) < 0)
            //     return false;
            // if (USB_SendControl(0, &landingPageScheme, 1) < 0)
            //     return false;
            // return USB_SendControl(0, landingPageUrl, urlLength) >= 0;
        }

        switch(usb_setup.bRequest) {
            case VENDOR_CODE:
                if (usb_setup.wIndex == 0x0004) {
                    send_microsoft_descriptors(&msft_compatible);
                    return;
                }
                // Work around WINUSB bug by responding with the extended properties
                // descriptor on device level too.
                // See https://github.com/pbatard/libwdi/wiki/WCID-Devices#defining-a-device-interface-guid-or-other-device-specific-properties
                else if (usb_setup.wIndex == 0x0005) {
                    send_microsoft_descriptors((const USB_MicrosoftCompatibleDescriptor_t*) &msft_extended_properties);
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

