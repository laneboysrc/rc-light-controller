#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>

#include <usb.h>
#include <usb_samd.h>
#include <class/dfu/dfu.h>
#include <flash_layout.h>


// We can only erase the flash in units of 'row', where each row consists of
// four pages. We therefore have the DFU transfer in blocks of 4 pages = 1 ROW
#define DFU_TRANSFER_SIZE (FLASH_PAGE_SIZE * 4)

#define DFU_RUNTIME_PROTOCOL 1
#define DFU_DFU_MODE_PROTOCOL 2

#define USB_INTERFACE_DFU 0

#define USB_STRING_LANGUAGE 0
#define USB_STRING_MANUFACTURER 1
#define USB_STRING_PRODUCT 2
#define USB_STRING_SERIAL_NUMBER 3
#define USB_STRING_DFU 4
#define USB_STRING_MSFT 0xee

#define USB_EP0 (USB_IN + 0)

#define MSFT_ID 0xee

// Declare the endpoints in use.
USB_ENDPOINTS(1)

alignas(4) uint8_t ep0_buffer[146];

// Global flag that is set by the DFU class when a firmware upgrade has finished
// and the MCU should be restarted.
extern volatile bool bootloader_done;



alignas(4) const USB_DeviceDescriptor device_descriptor = {
    .bLength = sizeof(USB_DeviceDescriptor),
    .bDescriptorType = USB_DTYPE_Device,

    .bcdUSB = 0x0210,
    .bDeviceClass = USB_CSCP_NoDeviceClass,
    .bDeviceSubClass = USB_CSCP_NoDeviceSubclass,
    .bDeviceProtocol = USB_CSCP_NoDeviceProtocol,

    .bMaxPacketSize0 = 64,
    .idVendor = 0x6666,
    .idProduct = 0xcab0,
    .bcdDevice = 0x0103,

    .iManufacturer = USB_STRING_MANUFACTURER,
    .iProduct = USB_STRING_PRODUCT,
    .iSerialNumber = USB_STRING_SERIAL_NUMBER,

    .bNumConfigurations = 1
};


typedef struct ConfigDesc {
    USB_ConfigurationDescriptor Config;

    USB_InterfaceAssociationDescriptor DFU_interface_association;

    USB_InterfaceDescriptor DFU_interface;
    DFU_FunctionalDescriptor DFU_functional;

}  __attribute__((packed)) ConfigDesc;

alignas(4) const ConfigDesc configuration_descriptor = {
    .Config = {
        .bLength = sizeof(USB_ConfigurationDescriptor),
        .bDescriptorType = USB_DTYPE_Configuration,
        .wTotalLength  = sizeof(ConfigDesc),
        .bNumInterfaces = 1,
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
        .bFunctionProtocol = DFU_DFU_MODE_PROTOCOL,
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
        .bInterfaceProtocol = DFU_DFU_MODE_PROTOCOL,
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
    .bCount = 1,
    .reserved = {0, 0, 0, 0, 0, 0, 0},
    .interfaces = {
        {
            .bFirstInterfaceNumber = 0,
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
void dfu_cb_dnload_block(uint16_t block_number, uint16_t length) {
    uint32_t address;

    (void) length;

    if (usb_setup.wLength > DFU_TRANSFER_SIZE) {
        dfu_error(DFU_STATUS_errUNKNOWN);
        return;
    }

    if ((block_number * DFU_TRANSFER_SIZE) > FLASH_FIRMWARE_SIZE) {
        dfu_error(DFU_STATUS_errADDRESS);
        return;
    }

    // Erase the of flash memory row that corresponds to the given block_number
    address = FLASH_FIRMWARE_START + (block_number * DFU_TRANSFER_SIZE);
    NVMCTRL->ADDR.reg = address >> 1;
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD(NVMCTRL_CTRLA_CMD_ER);
    while (!NVMCTRL->INTFLAG.bit.READY);
}


// ****************************************************************************
void dfu_cb_dnload_packet_completed(uint16_t block_number, uint16_t offset, uint8_t* data, uint16_t length) {
    uint32_t address;
    uint32_t nvm_address;

    address = FLASH_FIRMWARE_START + (block_number * DFU_TRANSFER_SIZE) + offset;
    nvm_address = address / 2;

    // The NVM must be accessed as a series of 16-bit words. The tricky bit
    // is that we must ensure that we can write an odd number of bytes, i.e.
    // the last word may be incomplete.
    for (uint16_t i = 0; i < length; i += 2) {
        uint16_t word;

        word = data[i];
        if ((i + 1) < length) {
            word |= data[i + 1] << 8;
        }

        ((volatile uint16_t *)FLASH_ADDR)[nvm_address++] = word;
    }

    // Perform the write
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD(NVMCTRL_CTRLA_CMD_WP);
    while (!NVMCTRL->INTFLAG.bit.READY);
}


// ****************************************************************************
unsigned dfu_cb_dnload_block_completed(uint16_t block_number, uint16_t length) {
    (void) block_number;
    (void) length;
    return 0;
}


// ****************************************************************************
void dfu_cb_manifest(void) {
    // When we receive "manifest" from the host we detach from USB and reboot
    // the MCU
    bootloader_done = true;

    // Reset the DFU code to report IDLE state, which is checked by dfu-util
    dfu_reset();
}


// ****************************************************************************
uint16_t usb_cb_get_descriptor(uint8_t type, uint8_t index, const uint8_t** ptr) {
    const void* address = NULL;
    uint16_t size = 0;

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
                    address = usb_string_to_descriptor((char *)"RC Light Controller (DFU-bootloader)");
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
        return true;
    }
    return false;
}


// ****************************************************************************
bool usb_cb_set_interface(uint16_t interface, uint16_t new_altsetting) {
    (void) new_altsetting;

    if (interface == USB_INTERFACE_DFU) {
        // Reset the DFU interface
        NVMCTRL->CTRLB.bit.MANW = 1;
        dfu_reset();
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
            dfu_control_setup();
            return;
        }

        switch(usb_setup.bRequest) {
            case MSFT_ID:
                // FIXME: only one should be here, read docs!
                handle_msft_compatible((USB_MicrosoftCompatibleDescriptor *)&msft_compatible, &msft_extended);
                return;

            default:
                break;
        }
    }

    else if (recipient == USB_RECIPIENT_DEVICE) {
        switch(usb_setup.bRequest) {
            case MSFT_ID:
                // FIXME: only one should be here, read docs!
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
    uint8_t recipient = usb_setup.bmRequestType & USB_REQTYPE_RECIPIENT_MASK;

    if (recipient == USB_RECIPIENT_INTERFACE) {
        if (usb_setup.wIndex ==USB_INTERFACE_DFU) {
            dfu_control_in_completion();
        }
    }
}


// ****************************************************************************
void usb_cb_control_out_completion(void) {
    uint8_t recipient = usb_setup.bmRequestType & USB_REQTYPE_RECIPIENT_MASK;

    if (recipient == USB_RECIPIENT_INTERFACE) {
        if (usb_setup.wIndex == USB_INTERFACE_DFU) {
            dfu_control_out_completion();
        }
    }
}


// ****************************************************************************
void usb_cb_completion(void) {
    // Nothing to do
}
