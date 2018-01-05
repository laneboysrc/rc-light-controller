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
#define USB_STRING_UART 5

#define USB_DTYPE_BOS 15
#define WEBUSB_REQUEST_GET_URL 2

// Our arbitrary vendor codes
#define VENDOR_CODE_MS 42       // Retrieve the Microsoft OS 2.0 descriptor
#define VENDOR_CODE_WEBUSB 69   // Retrieve WebUSB landing page URL

#define USB_NUMBER_OF_INTERFACES 3
#define USB_INTERFACE_DFU 0
#define USB_INTERFACE_CDC_CONTROL 1
#define USB_INTERFACE_CDC_DATA 2




#define USB_EP0 (USB_IN + 0)
#define USB_EP_CDC_NOTIFICATION (USB_IN + 1)
#define USB_EP_CDC_IN (USB_IN + 2)
#define USB_EP_CDC_OUT (USB_OUT + 3)

#define BUF_SIZE 64


static alignas(4) uint8_t usbserial_buf_in[BUF_SIZE];
static alignas(4) uint8_t usbserial_buf_out[BUF_SIZE];


static const uint8_t* data;
static uint16_t data_length;


static alignas(4) const USB_DeviceDescriptor device_descriptor = {
    .bLength = sizeof(USB_DeviceDescriptor),
    .bDescriptorType = USB_DTYPE_Device,

    .bcdUSB = 0x0210,
    .bDeviceClass = USB_CSCP_NoDeviceClass,
    .bDeviceSubClass = USB_CSCP_NoDeviceSubclass,
    .bDeviceProtocol = USB_CSCP_NoDeviceProtocol,

    .bMaxPacketSize0 = 64,
    .idVendor = 0x6666,
    .idProduct = 0xcab1,
    .bcdDevice = 0x0103,

    .iManufacturer = USB_STRING_MANUFACTURER,
    .iProduct = USB_STRING_PRODUCT,
    .iSerialNumber = USB_STRING_SERIAL_NUMBER,

    .bNumConfigurations = 1
};


typedef struct {
    USB_ConfigurationDescriptor Config;

    USB_InterfaceAssociationDescriptor DFU_interface_association;

    USB_InterfaceDescriptor DFU_interface;
    DFU_FunctionalDescriptor DFU_functional;

    USB_InterfaceAssociationDescriptor CDC_interface_association;

    USB_InterfaceDescriptor CDC_control_interface;
    CDC_FunctionalHeaderDescriptor CDC_functional_header;
    CDC_FunctionalACMDescriptor CDC_functional_ACM;
    CDC_FunctionalUnionDescriptor CDC_functional_union;
    USB_EndpointDescriptor CDC_notification_endpoint;

    USB_InterfaceDescriptor CDC_data_interface;
    USB_EndpointDescriptor CDC_out_endpoint;
    USB_EndpointDescriptor CDC_in_endpoint;
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
        .iInterface = USB_STRING_UART,
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
        .iInterface = USB_STRING_UART,
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
    }
};


static const struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    __CHAR16_TYPE__ bString[1];
}  __attribute__ ((packed)) alignas(4) language_string = {
    .bLength = USB_STRING_LEN(1),
    .bDescriptorType = USB_DTYPE_String,
    .bString = { USB_LANGUAGE_EN_US }
};


// Binary Object Store descriptor
static const struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wTotalLength;
    uint8_t bNumDeviceCaps;

    struct {
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint8_t bDevCapabilityType;

        uint8_t bReserved;
        uint8_t PlatformCapabilityUUID[16];
        uint16_t bcdVersion;
        uint8_t bVendorCode;
        uint8_t iLandingPage;
    } __attribute__((packed)) WebUSB_Descriptor;

    struct {
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint8_t bDevCapabilityType;

        uint8_t bReserved;
        uint8_t PlatformCapabilityUUID[16];
        uint32_t dwVersion;
        uint16_t wLength;
        uint8_t bVendorCode;
        uint8_t bAlternateEnumerationCode;
    } __attribute__((packed)) MS_OS_20_Descriptor;

} __attribute__((packed)) alignas(4) BOS_Descriptor = {
    .bLength = 5,
    .bDescriptorType = 15,          // Binary Object Store descriptor
    .wTotalLength = 5 + 24,
    .bNumDeviceCaps = 2,

    .WebUSB_Descriptor = {
        .bLength = 24,
        .bDescriptorType = 16,      // "Device Capability" descriptor
        .bDevCapabilityType = 5,    // "Platform" capability descriptor

        .bReserved = 0,
        .PlatformCapabilityUUID = {0x38, 0xb6, 0x08, 0x34, 0xa9, 0x09, 0xa0, 0x47, 0x8b, 0xfd, 0xa0, 0x76, 0x88, 0x15, 0xb6, 0x65},
        .bcdVersion = 0x0100,
        .bVendorCode = VENDOR_CODE_WEBUSB,
        .iLandingPage = 1
    },
    .MS_OS_20_Descriptor = {
        .bLength = 24,
        .bDescriptorType = 16,      // "Device Capability" descriptor
        .bDevCapabilityType = 5,    // "Platform" capability descriptor

        .bReserved = 0,
        .PlatformCapabilityUUID = {0xdf, 0x60, 0xdd, 0xd8, 0x89, 0x45, 0xc7, 0x4c, 0x9c, 0xd2, 0x65, 0x9d, 0x9e, 0x64, 0x8a, 0x9f},
        .dwVersion = 0x06030000,    // Windows version (8.1)
        .wLength = 46,
        .bVendorCode = VENDOR_CODE_MS,
        .bAlternateEnumerationCode = 0
    }
};

#define URL1 "laneboysrc.github.io/rc-light-controller"
static const struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bScheme;
    uint8_t URL[sizeof(URL1)];
} __attribute__((packed)) alignas(4) LandingPageDescriptor = {
    .bLength = 3 + sizeof(URL1),
    .bDescriptorType = 3,       // WebUSB URL
    .bScheme = 1,               // https://
    .URL = URL1
};


static const struct {
    // Microsoft OS 2.0 descriptor set header (table 10)
    struct {
        uint16_t wLength;
        uint16_t wHeaderType;
        uint32_t dwVersion;
        uint16_t wSize;
    } __attribute__((packed)) Descriptor_Set_Header;

    struct {
        uint16_t wLength;
        uint16_t wHeaderType;
        uint8_t bConfigurationValue;
        uint8_t bReserved;
        uint16_t wSize;
    } __attribute__((packed)) Configuration_Subset_Header;

    struct {
        uint16_t wLength;
        uint16_t wHeaderType;
        uint8_t iFirstInterfaceNumber;
        uint8_t bReserved;
        uint16_t wSize;
    } __attribute__((packed)) Function_Subset_Header;

    struct {
        uint16_t wLength;
        uint16_t wHeaderType;
        uint8_t bId[16];
    } __attribute__((packed)) Compatible_Id_Descriptor;

} __attribute__((packed)) alignas(4) MS_OS_20_Descriptor = {
    .Descriptor_Set_Header = {
        .wLength = sizeof(MS_OS_20_Descriptor.Descriptor_Set_Header),
        .wHeaderType = 0,           // MS OS 2.0 descriptor set header
        .dwVersion = 0x06030000,    // Windows version (8.1)
        .wSize = 46
    },

    .Configuration_Subset_Header = {
        .wLength = sizeof(MS_OS_20_Descriptor.Configuration_Subset_Header),
        .wHeaderType = 1,       // MS OS 2.0 configuration subset header
        .bConfigurationValue = 0,
        .bReserved = 0,
        .wSize = 36
    },

    .Function_Subset_Header = {
        .wLength = sizeof(MS_OS_20_Descriptor.Function_Subset_Header),
        .wHeaderType = 2,       // MS OS 2.0 function subset header
        .iFirstInterfaceNumber = 0,
        .bReserved = 0,
        .wSize = 28
    },

    .Compatible_Id_Descriptor = {
        .wLength = sizeof(MS_OS_20_Descriptor.Compatible_Id_Descriptor),
        .wHeaderType = 3,       // MS OS 2.0 compatible ID descriptor
        .bId = "WINUSB"
    }
};


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
            size = sizeof(configuration_descriptor);
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

                case USB_STRING_UART:
                    address = usb_string_to_descriptor((char *)"RC Light Controller (UART)");
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
    }

    else if (recipient == USB_RECIPIENT_DEVICE  &&  requestType == USB_REQTYPE_VENDOR) {
        switch(usb_setup.bRequest) {
            case VENDOR_CODE_WEBUSB:
                if (usb_setup.wIndex == WEBUSB_REQUEST_GET_URL) {
                    send_descriptor(&LandingPageDescriptor, sizeof(LandingPageDescriptor));
                    return;
                }
                break;

            case VENDOR_CODE_MS:
                send_descriptor(&MS_OS_20_Descriptor, sizeof(MS_OS_20_Descriptor));
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
    printf("usb_cb_control_in_completion\n");
    send_descriptor_multi();
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

