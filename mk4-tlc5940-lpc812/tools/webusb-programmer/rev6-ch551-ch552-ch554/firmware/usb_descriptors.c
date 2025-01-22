#include <stdint.h>
#include "usb.h"

#define USB_W(x) (x&0xff), ((uint16_t)x>>8)
#define USB_WC(x) (x), 0x00

//=============================================================================
__code uint8_t device_descriptor[] = {
    sizeof(device_descriptor), // bLength
    USB_DTYPE_DEVICE,   // bDescriptorType
    USB_W(0x0210),      // bcdUSB 2.10
    0xef,               // bDeviceClass Interface Association Descriptor Device Class
    0x02,               // bDeviceSubClass IADDeviceSubclass
    0x01,               // bDeviceProtocol IADDeviceProtocol
    EP0_SIZE,           // bMaxPacketSize0
    USB_W(0x6666),      // idVendor
    USB_W(0xcab7),      // idProduct
    USB_W(0x0401),      // bcdDevice 4.01 (device-specific version number)
    USB_STRING_MANUFACTURER, // iManufacturer
    USB_STRING_PRODUCT, // iProduct
    USB_STRING_SERIAL,  // iSerial
    1                   // bNumConfigurations
};
__code uint16_t device_descriptor_length = sizeof(device_descriptor);


//=============================================================================
__code uint8_t configuration_descriptor[] = {
    9,                  // bLength
    USB_DTYPE_CONFIGURATION, // bDescriptorType
    USB_W(sizeof(configuration_descriptor)), // wTotalLength
    2,                  // bNumInterfaces
    1,                  // bConfigurationValue
    0,                  // iConfiguration
    0x80,               // bmAttributes: Bus Powered
    0x32,               // MaxPower: 100mA

    // Test interface association
    8,                  // bLength
    USB_DTYPE_IAD,      // bDescriptorType
    USB_INTERFACE_TEST, // bFirstInterface
    1,                  // bInterfaceCount
    0xff,               // bFunctionClass Vendor specific
    0x00,               // bFunctionSubClass
    0x00,               // bFunctionProtocol
    USB_STRING_TEST,    // iFunction

    // Test interface
    9,                  // bLength
    USB_DTYPE_INTERFACE, // bDescriptorType
    USB_INTERFACE_TEST, // bInterfaceNumber
    0,                  // bAlternateSetting
    2,                  // bNumEndpoints
    0xff,               // bInterfaceClass Vendor specific
    0x00,               // bInterfaceSubClass
    0x00,               // bInterfaceProtocol
    USB_STRING_TEST,    // iInterface

    // Test interface in endpoint
    7,                  // bLength
    USB_DTYPE_ENDPOINT, // bDescriptorType
    0x82,               // bEndpointAddress
    0x02,               // bmAttributes USB_EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA
    USB_W(64),          // wMaxPacketSize
    5,                  // bInterval

    // Test interface out endpoint
    7,                  // bLength
    USB_DTYPE_ENDPOINT, // bDescriptorType
    0x01,               // bEndpointAddress
    0x02,               // bmAttributes USB_EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA
    USB_W(64),          // wMaxPacketSize
    5,                  // bInterval

    // DFU interface association
    8,                  // bLength
    USB_DTYPE_IAD,      // bDescriptorType
    USB_INTERFACE_DFU,  // bFirstInterface
    1,                  // bInterfaceCount
    0xfe,               // bFunctionClass DFU interface class
    0x01,               // bFunctionSubClass DFU interface subclass
    0x01,               // bFunctionProtocol DFU interface runtime protocol
    USB_STRING_DFU,     // iFunction

    // DFU interface
    9,                  // bLength
    USB_DTYPE_INTERFACE, // bDescriptorType
    USB_INTERFACE_DFU,  // bInterfaceNumber
    0,                  // bAlternateSetting
    0,                  // bNumEndpoints
    0xfe,               // bInterfaceClass DFU interface class
    0x01,               // bInterfaceSubClass DFU interface subclass
    0x01,               // bInterfaceProtocol DFU interface runtime protocol
    USB_STRING_DFU,     // iInterface

    // DFU functional
    9,                  // bLength
    USB_DTYPE_DFU,      // bDescriptorType
    0x0b,               // bmAttributes  DFU_ATTR_CAN_DOWNLOAD | DFU_ATTR_CAN_UPLOAD | DFU_ATTR_WILL_DETACH
    USB_W(0),           // wDetachTimeout
    USB_W(256),         // wTransferSize
    USB_W(0x0110),      // bcdDFUVersion
};
__code uint16_t configuration_descriptor_length = sizeof(configuration_descriptor);


//=============================================================================
__code uint8_t bos_descriptor[] = {
    5,                  // bLength = 5,
    USB_DTYPE_BOS,      // bDescriptorType Binary Object Store descriptor
    USB_W(sizeof(bos_descriptor)), // wTotalLength
    2,                  // bNumDeviceCaps

    // WebUSB descriptor
    24,                 // bLength
    0x10,               // bDescriptorType Device Capability descriptor
    0x05,               // bDevCapabilityType "Platform" capability descriptor
    0,                  // bReserved
                        // PlatformCapabilityUUID
    0x38, 0xb6, 0x08, 0x34, 0xa9, 0x09, 0xa0, 0x47, 0x8b, 0xfd, 0xa0, 0x76, 0x88, 0x15, 0xb6, 0x65,
    USB_W(0x0100),      // bcdVersion
    VENDOR_CODE_WEBUSB, // bVendorCode
    0,                  // iLandingPage  Set this to '1' to enable notification
                        // when the device is plugged-in.

    // MS_OS_20_Descriptor
    28,                 // bLength = sizeof(bos_descriptor.MS_OS_20_Descriptor),
    0x10,               // bDescriptorType Device Capability descriptor
    0x05,               // bDevCapabilityType "Platform" capability descriptor
    0,                  // bReserved
                        // PlatformCapabilityUUID
    0xdf, 0x60, 0xdd, 0xd8, 0x89, 0x45, 0xc7, 0x4c, 0x9c, 0xd2, 0x65, 0x9d, 0x9e, 0x64, 0x8a, 0x9f,
    0x00, 0x00, 0x03, 0x06, // dwVersion = 0x06030000  Windows Blue
    USB_W(sizeof(ms_os_20_descriptor)), // wLength
    VENDOR_CODE_MS,     // bVendorCode
    0,                  // bAlternateEnumerationCode
};
__code uint16_t bos_descriptor_length = sizeof(bos_descriptor);


//=============================================================================
__code uint8_t ms_os_20_descriptor[] = {
    // Descriptor Set Header
    USB_W(10),          // wLength
    USB_W(0),           // wDescriptorType MS OS 2.0 descriptor set header
    0x00, 0x00, 0x03, 0x06, // dwWindowsVersion 0x06030000  Windows Blue
    USB_W(sizeof(ms_os_20_descriptor)), // wTotalLength

    // Since we have two interfaces, we must declare the WINUSB compatible ID
    // for both of them.
    // To do so we have to use the configuration subset header, and then two
    // function subset header (one for each interface), which declares the
    // WINUSB compatible ID.

    // Configuration Subset Header
    USB_W(8),           // wLength
    USB_W(1),           // wDescriptorType MS OS 2.0 configuration subset header
    0,                  // bConfigurationValue - This is actualy bConfigurationINDEX
    0,                  // bReserved
    USB_W(328),         // wTotalLength

    // Function Subset Header 1
    USB_W(8),           // wLength
    USB_W(2),           // wDescriptorType MS OS 2.0 function subset header
    USB_INTERFACE_TEST, // bFirstInterface
    0,                  // bReserved
    USB_W(160),         // wSubsetLength

    // Compatible Id Descriptor 1
    USB_W(20),          // wLength
    USB_W(3),           // wDescriptorType MS OS 2.0 compatible ID descriptor
                        // bId
    'W',  'I',  'N',  'U',  'S',  'B',  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // Registry Property Descriptor 1
    USB_W(132),         // wLength
    USB_W(4),           // wDescriptorType Registry property descriptor
    USB_W(7),           // wPropertyDataType REG_MULTI_SZ
    USB_W(42),          // wPropertyNameLength
                        // bPropertyName
    'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, 'I', 0x00,
    'n', 0x00, 't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00, 'a', 0x00, 'c', 0x00,
    'e', 0x00, 'G', 0x00, 'U', 0x00, 'I', 0x00, 'D', 0x00, 's', 0x00, 0x00, 0x00,
    USB_W(80),          // wPropertyDataLength
                        // bPropertyData
    '{', 0x00, '9', 0x00, '7', 0x00, 'C', 0x00, 'F', 0x00, '4', 0x00, '1', 0x00,
    '1', 0x00, '3', 0x00, '-', 0x00, '4', 0x00, '9', 0x00, 'a', 0x00, 'A', 0x00,
    '-', 0x00, '9', 0x00, '2', 0x00, '1', 0x00, '9', 0x00, '-', 0x00, '1', 0x00,
    '9', 0x00, '8', 0x00, 'E', 0x00, '-', 0x00, '1', 0x00, '9', 0x00, '8', 0x00,
    'C', 0x00, '5', 0x00, 'D', 0x00, '4', 0x00, '6', 0x00, '6', 0x00, '7', 0x00,
    '6', 0x00, 'A', 0x00, '}', 0x00, 0x00, 0x00, 0x00, 0x00,

    // Function_Subset_Header 2
    USB_W(8),           // wLength
    USB_W(2),           // wDescriptorType MS OS 2.0 function subset header
    USB_INTERFACE_DFU,  // bFirstInterface
    0,                  // bReserved
    USB_W(160),         // wSubsetLength

    // Compatible_Id_Descriptor 2
    USB_W(20),          // wLength
    USB_W(3),           // wDescriptorType MS OS 2.0 compatible ID descriptor
                        // bId
    'W',  'I',  'N',  'U',  'S',  'B',  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // Registry Property Descriptor 2
    USB_W(132),         // wLength
    USB_W(4),           // wDescriptorType Registry property descriptor
    USB_W(7),           // wPropertyDataType REG_MULTI_SZ
    USB_W(42),          // wPropertyNameLength
                        // bPropertyName
    'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, 'I', 0x00,
    'n', 0x00, 't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00, 'a', 0x00, 'c', 0x00,
    'e', 0x00, 'G', 0x00, 'U', 0x00, 'I', 0x00, 'D', 0x00, 's', 0x00, 0x00, 0x00,
    USB_W(80),          // wPropertyDataLength
                        // bPropertyData
    '{', 0x00, '8', 0x00, 'A', 0x00, '5', 0x00, '0', 0x00, 'E', 0x00, '1', 0x00,
    '2', 0x00, '8', 0x00, '-', 0x00, '4', 0x00, '1', 0x00, '1', 0x00, '3', 0x00,
    '-', 0x00, '4', 0x00, '9', 0x00, '1', 0x00, 'A', 0x00, '-', 0x00, '9', 0x00,
    '8', 0x00, 'E', 0x00, 'E', 0x00, '-', 0x00, '7', 0x00, '0', 0x00, 'E', 0x00,
    '5', 0x00, 'F', 0x00, '2', 0x00, 'E', 0x00, '7', 0x00, '3', 0x00, '8', 0x00,
    '0', 0x00, 'D', 0x00, '}', 0x00, 0x00, 0x00, 0x00, 0x00,
};
__code uint16_t ms_os_20_descriptor_length = sizeof(ms_os_20_descriptor);


//=============================================================================
__code uint8_t language_descriptor[] = {
    sizeof(language_descriptor), // bLength
    USB_DTYPE_STRING,   // bDescriptorType
    USB_W(0x0409)       // wLangID[0] English ( United States)
};
__code uint16_t language_descriptor_length = sizeof(language_descriptor);


//=============================================================================
__code uint8_t manufacturer_string_descriptor[] = {
    sizeof(manufacturer_string_descriptor), // bLength
    USB_DTYPE_STRING,   // bDescriptorType
    USB_WC('L'),
    USB_WC('A'),
    USB_WC('N'),
    USB_WC('E'),
    USB_WC(' '),
    USB_WC('B'),
    USB_WC('o'),
    USB_WC('y'),
    USB_WC('s'),
    USB_WC(' '),
    USB_WC('R'),
    USB_WC('C'),
};
__code uint16_t manufacturer_string_descriptor_length = sizeof(manufacturer_string_descriptor);


//=============================================================================
__code uint8_t product_string_descriptor[] = {
    sizeof(product_string_descriptor), // bLength
    USB_DTYPE_STRING,   // bDescriptorType
    USB_WC('P'),
    USB_WC('r'),
    USB_WC('o'),
    USB_WC('g'),
    USB_WC('r'),
    USB_WC('a'),
    USB_WC('m'),
    USB_WC('m'),
    USB_WC('e'),
    USB_WC('r'),
};
__code uint16_t product_string_descriptor_length = sizeof(product_string_descriptor);


//=============================================================================
__code uint8_t dfu_string_descriptor[] = {
    sizeof(dfu_string_descriptor), // bLength
    USB_DTYPE_STRING,   // bDescriptorType
    USB_WC('P'),
    USB_WC('r'),
    USB_WC('o'),
    USB_WC('g'),
    USB_WC('r'),
    USB_WC('a'),
    USB_WC('m'),
    USB_WC('m'),
    USB_WC('e'),
    USB_WC('r'),
    USB_WC(' '),
    USB_WC('D'),
    USB_WC('F'),
    USB_WC('U'),
};
__code uint16_t dfu_string_descriptor_length = sizeof(dfu_string_descriptor);


//=============================================================================
__code uint8_t test_string_descriptor[] = {
    sizeof(test_string_descriptor), // bLength
    USB_DTYPE_STRING,   // bDescriptorType
    USB_WC('P'),
    USB_WC('r'),
    USB_WC('o'),
    USB_WC('g'),
    USB_WC('r'),
    USB_WC('a'),
    USB_WC('m'),
    USB_WC('m'),
    USB_WC('e'),
    USB_WC('r'),
    USB_WC(' '),
    USB_WC('U'),
    USB_WC('A'),
    USB_WC('R'),
    USB_WC('T'),
};
__code uint16_t test_string_descriptor_length = sizeof(test_string_descriptor);


//=============================================================================
__code uint8_t landing_page_descriptor[] = {
    sizeof(landing_page_descriptor), // bLength
    0x03,               // bDescriptorType WebUSB URL
    0x01,               // bScheme https://
    USB_WC('l'),
    USB_WC('a'),
    USB_WC('n'),
    USB_WC('e'),
    USB_WC('b'),
    USB_WC('o'),
    USB_WC('y'),
    USB_WC('s'),
    USB_WC('r'),
    USB_WC('c'),
    USB_WC('.'),
    USB_WC('g'),
    USB_WC('i'),
    USB_WC('t'),
    USB_WC('h'),
    USB_WC('u'),
    USB_WC('b'),
    USB_WC('.'),
    USB_WC('i'),
    USB_WC('o'),
    USB_WC('/'),
    USB_WC('r'),
    USB_WC('c'),
    USB_WC('-'),
    USB_WC('l'),
    USB_WC('i'),
    USB_WC('g'),
    USB_WC('h'),
    USB_WC('t'),
    USB_WC('-'),
    USB_WC('c'),
    USB_WC('o'),
    USB_WC('n'),
    USB_WC('t'),
    USB_WC('r'),
    USB_WC('o'),
    USB_WC('l'),
    USB_WC('l'),
    USB_WC('e'),
    USB_WC('r'),
    USB_WC('/'),
};
__code uint16_t landing_page_descriptor_length = sizeof(landing_page_descriptor);
