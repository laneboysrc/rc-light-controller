#pragma once

#include <stdint.h>
#include <stdalign.h>

// Our arbitrary vendor codes
#define VENDOR_CODE_MS 42           // Retrieve the Microsoft OS 2.0 descriptor
#define VENDOR_CODE_WEBUSB 69       // Retrieve WebUSB landing page URL
#define VENDOR_CODE_SIMULATOR 72    // Preprocessor simulator input

// #define URL1 "laneboysrc.github.io/rc-light-controller"
#define URL1 "laneboysrc.github.io/rc-light-controller/preprocessor-simulator.html"


// Binary Object Store descriptor
typedef struct {
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
        uint32_t dwVersion;
        uint16_t wLength;
        uint8_t bVendorCode;
        uint8_t bAlternateEnumerationCode;
    } __attribute__((packed)) MS_OS_20_Descriptor;

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

} __attribute__((packed)) bos_descriptor_t;


typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bScheme;
    uint8_t URL[sizeof(URL1)];
} __attribute__((packed)) landing_page_descriptor_t;


// Microsoft OS 2.0 descriptor
typedef struct {
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

} __attribute__((packed)) ms_os_20_descriptor_t;


static const ms_os_20_descriptor_t ms_os_20_descriptor = {
    .Descriptor_Set_Header = {
        .wLength = sizeof(ms_os_20_descriptor.Descriptor_Set_Header),
        .wHeaderType = 0,           // MS OS 2.0 descriptor set header
        .dwVersion = 0x06030000,    // Windows version (8.1)
        .wSize = sizeof(ms_os_20_descriptor_t)
    },

    .Configuration_Subset_Header = {
        .wLength = sizeof(ms_os_20_descriptor.Configuration_Subset_Header),
        .wHeaderType = 1,       // MS OS 2.0 configuration subset header
        .bConfigurationValue = 0,
        .bReserved = 0,
        .wSize = 36
    },

    .Function_Subset_Header = {
        .wLength = sizeof(ms_os_20_descriptor.Function_Subset_Header),
        .wHeaderType = 2,       // MS OS 2.0 function subset header
        .iFirstInterfaceNumber = 0,
        .bReserved = 0,
        .wSize = 28
    },

    .Compatible_Id_Descriptor = {
        .wLength = sizeof(ms_os_20_descriptor.Compatible_Id_Descriptor),
        .wHeaderType = 3,       // MS OS 2.0 compatible ID descriptor
        .bId = "WINUSB"
    }
};


static const landing_page_descriptor_t landing_page_descriptor = {
    .bLength = 3 + sizeof(URL1),
    .bDescriptorType = 3,       // WebUSB URL
    .bScheme = 1,               // https://
    .URL = URL1
};


static const bos_descriptor_t bos_descriptor = {
    .bLength = 5,
    .bDescriptorType = 15,          // Binary Object Store descriptor
    .wTotalLength = sizeof(bos_descriptor_t),
    .bNumDeviceCaps = 2,

    .MS_OS_20_Descriptor = {
        .bLength = sizeof(bos_descriptor.MS_OS_20_Descriptor),
        .bDescriptorType = 16,      // "Device Capability" descriptor
        .bDevCapabilityType = 5,    // "Platform" capability descriptor

        .bReserved = 0,
        .PlatformCapabilityUUID = {0xdf, 0x60, 0xdd, 0xd8, 0x89, 0x45, 0xc7, 0x4c, 0x9c, 0xd2, 0x65, 0x9d, 0x9e, 0x64, 0x8a, 0x9f},
        .dwVersion = 0x06030000,    // Windows version (8.1)
        .wLength = sizeof(ms_os_20_descriptor_t),
        .bVendorCode = VENDOR_CODE_MS,
        .bAlternateEnumerationCode = 0
    },

    .WebUSB_Descriptor = {
        .bLength = sizeof(bos_descriptor.WebUSB_Descriptor),
        .bDescriptorType = 16,      // "Device Capability" descriptor
        .bDevCapabilityType = 5,    // "Platform" capability descriptor

        .bReserved = 0,
        .PlatformCapabilityUUID = {0x38, 0xb6, 0x08, 0x34, 0xa9, 0x09, 0xa0, 0x47, 0x8b, 0xfd, 0xa0, 0x76, 0x88, 0x15, 0xb6, 0x65},
        .bcdVersion = 0x0100,
        .bVendorCode = VENDOR_CODE_WEBUSB,
        .iLandingPage = 1
    }
};
