#pragma once

#include <stdint.h>
#include <stdalign.h>

// Declare the endpoints in use.
USB_ENDPOINTS(3)

#define USB_EP0 (USB_IN + 0)
#define USB_EP_TEST_IN (USB_IN + 1)
#define USB_EP_TEST_OUT (USB_OUT + 2)

#define BUF_SIZE 64


// Transfer size conforms to one flash 'row', which consists of four 'pages'
#define DFU_TRANSFER_SIZE (FLASH_PAGE_SIZE * 4)

#define DFU_RUNTIME_PROTOCOL 1
#define DFU_DFU_PROTOCOL 2

#define VENDOR_INTERFACE_CLASS 0xff

#define USB_DTYPE_BOS 15
#define WEBUSB_REQUEST_GET_URL 2
#define WINUSB_REQUEST_DESCRIPTOR 7

enum {
    USB_INTERFACE_DFU,
    USB_INTERFACE_TEST,
    USB_NUMBER_OF_INTERFACES
};

#define USB_STRING_LANGUAGE 0
#define USB_STRING_MANUFACTURER 1
#define USB_STRING_PRODUCT 2
#define USB_STRING_SERIAL_NUMBER 3
#define USB_STRING_DFU 4
#define USB_STRING_TEST 5

typedef struct {
    USB_ConfigurationDescriptor Config;

    USB_InterfaceAssociationDescriptor DFU_interface_association;
    USB_InterfaceDescriptor DFU_interface;
    DFU_FunctionalDescriptor DFU_functional;

    USB_InterfaceAssociationDescriptor Test_interface_association;
    USB_InterfaceDescriptor Test_interface;
    USB_EndpointDescriptor Test_interface_in_endpoint;
    USB_EndpointDescriptor Test_interface_out_endpoint;
}  __attribute__((packed)) configuration_descriptor_t;

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    __CHAR16_TYPE__ bString[1];
}  __attribute__ ((packed)) language_string_t;


static const USB_DeviceDescriptor device_descriptor = {
    .bLength = sizeof(USB_DeviceDescriptor),
    .bDescriptorType = USB_DTYPE_Device,

    .bcdUSB = 0x0210,
    .bDeviceClass = USB_CSCP_NoDeviceClass,
    .bDeviceSubClass = USB_CSCP_NoDeviceSubclass,
    .bDeviceProtocol = USB_CSCP_NoDeviceProtocol,

    .bMaxPacketSize0 = 64,
    .idVendor = 0x6666,
    .idProduct = 0xcab1,
    .bcdDevice = 0x0109,

    .iManufacturer = USB_STRING_MANUFACTURER,
    .iProduct = USB_STRING_PRODUCT,
    .iSerialNumber = USB_STRING_SERIAL_NUMBER,

    .bNumConfigurations = 1
};

static const configuration_descriptor_t configuration_descriptor = {
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
        .iFunction = USB_STRING_DFU,
    },

    .DFU_interface = {
        .bLength = sizeof(USB_InterfaceDescriptor),
        .bDescriptorType = USB_DTYPE_Interface,
        .bInterfaceNumber = USB_INTERFACE_DFU,
        .bAlternateSetting = 0,
        .bNumEndpoints = 0,
        .bInterfaceClass = DFU_INTERFACE_CLASS,
        .bInterfaceSubClass = DFU_INTERFACE_SUBCLASS,
#ifdef BOOTLOADER
        .bInterfaceProtocol = DFU_DFU_PROTOCOL,
#else
        .bInterfaceProtocol = DFU_RUNTIME_PROTOCOL,
#endif
        .iInterface = USB_STRING_DFU
    },
    .DFU_functional = {
        .bLength = sizeof(DFU_FunctionalDescriptor),
        .bDescriptorType = DFU_DESCRIPTOR_TYPE,
        .bmAttributes = DFU_ATTR_CAN_DOWNLOAD | DFU_ATTR_CAN_UPLOAD | DFU_ATTR_WILL_DETACH,
        .wDetachTimeout = 0,
        .wTransferSize = DFU_TRANSFER_SIZE,
        .bcdDFUVersion = 0x0110,
    },

    .Test_interface_association = {
        .bLength = sizeof(USB_InterfaceAssociationDescriptor),
        .bDescriptorType = USB_DTYPE_InterfaceAssociation,
        .bFirstInterface = USB_INTERFACE_TEST,
        .bInterfaceCount = 1,
        .bFunctionClass = VENDOR_INTERFACE_CLASS,
        .bFunctionSubClass = 0,
        .bFunctionProtocol = 0,
        .iFunction = USB_STRING_TEST,
    },

    .Test_interface = {
        .bLength = sizeof(USB_InterfaceDescriptor),
        .bDescriptorType = USB_DTYPE_Interface,
        .bInterfaceNumber = USB_INTERFACE_TEST,
        .bAlternateSetting = 0,
        .bNumEndpoints = 2,
        .bInterfaceClass = VENDOR_INTERFACE_CLASS,
        .bInterfaceSubClass = 0,
        .bInterfaceProtocol = 0,
        .iInterface = USB_STRING_TEST,
    },
    .Test_interface_in_endpoint = {
        .bLength = sizeof(USB_EndpointDescriptor),
        .bDescriptorType = USB_DTYPE_Endpoint,
        .bEndpointAddress = USB_EP_TEST_IN,
        .bmAttributes = (USB_EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .wMaxPacketSize = BUF_SIZE,
        .bInterval = 0x05
    },
    .Test_interface_out_endpoint = {
        .bLength = sizeof(USB_EndpointDescriptor),
        .bDescriptorType = USB_DTYPE_Endpoint,
        .bEndpointAddress = USB_EP_TEST_OUT,
        .bmAttributes = (USB_EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .wMaxPacketSize = BUF_SIZE,
        .bInterval = 0x05
    }
};

static const language_string_t language_string = {
    .bLength = USB_STRING_LEN(1),
    .bDescriptorType = USB_DTYPE_String,
    .bString = { USB_LANGUAGE_EN_US }
};
