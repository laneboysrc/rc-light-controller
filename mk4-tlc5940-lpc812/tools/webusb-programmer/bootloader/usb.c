#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>

#include <usb.h>
#include <usb_samd.h>
#include <class/dfu/dfu.h>
#include <flash_layout.h>
#include <usb_descriptors.h>
#include <usb_bos.h>


// We can only erase the flash in units of 'row', where each row consists of
// four pages.
#define ROW_SIZE (FLASH_PAGE_SIZE * 4)

// Global flag that is set by the DFU class when a firmware upgrade has finished
// and the MCU should be restarted.
extern volatile bool bootloader_done;

static const uint8_t* data;
static uint16_t data_length;



// // The transfer size is set to the USB endpoint 0 size as we haven't been
// // able to figure output uploading the firmware if one transfer requires
// // multiple endpoint transactions.
// #define DFU_TRANSFER_SIZE USB_EP0_SIZE

// #define DFU_RUNTIME_PROTOCOL 1
// #define DFU_DFU_MODE_PROTOCOL 2

// #define USB_DTYPE_BOS 15
// #define WEBUSB_REQUEST_GET_URL 2
// #define WINUSB_REQUEST_DESCRIPTOR 7

// enum {
//     USB_INTERFACE_DFU,
//     USB_NUMBER_OF_INTERFACES
// };

// #define USB_STRING_LANGUAGE 0
// #define USB_STRING_MANUFACTURER 1
// #define USB_STRING_PRODUCT 2
// #define USB_STRING_SERIAL_NUMBER 3
// #define USB_STRING_DFU 4

// #define USB_EP0 (USB_IN + 0)


// // Declare the endpoints in use.
// USB_ENDPOINTS(1)


// typedef struct {
//     USB_ConfigurationDescriptor Config;

//     USB_InterfaceAssociationDescriptor DFU_interface_association;

//     USB_InterfaceDescriptor DFU_interface;
//     DFU_FunctionalDescriptor DFU_functional;

// }  __attribute__((packed)) configuration_descriptor_t;

// typedef struct {
//     uint8_t bLength;
//     uint8_t bDescriptorType;
//     __CHAR16_TYPE__ bString[1];
// } __attribute__ ((packed)) language_string_t;


// static const USB_DeviceDescriptor device_descriptor = {
//     .bLength = sizeof(USB_DeviceDescriptor),
//     .bDescriptorType = USB_DTYPE_Device,

//     .bcdUSB = 0x0210,
//     .bDeviceClass = USB_CSCP_NoDeviceClass,
//     .bDeviceSubClass = USB_CSCP_NoDeviceSubclass,
//     .bDeviceProtocol = USB_CSCP_NoDeviceProtocol,

//     .bMaxPacketSize0 = 64,
//     .idVendor = 0x6666,
//     .idProduct = 0xcab0,
//     .bcdDevice = 0x0104,

//     .iManufacturer = USB_STRING_MANUFACTURER,
//     .iProduct = USB_STRING_PRODUCT,
//     .iSerialNumber = USB_STRING_SERIAL_NUMBER,

//     .bNumConfigurations = 1
// };

// static const configuration_descriptor_t configuration_descriptor = {
//     .Config = {
//         .bLength = sizeof(USB_ConfigurationDescriptor),
//         .bDescriptorType = USB_DTYPE_Configuration,
//         .wTotalLength  = sizeof(configuration_descriptor_t),
//         .bNumInterfaces = USB_NUMBER_OF_INTERFACES,
//         .bConfigurationValue = 1,
//         .iConfiguration = 0,
//         .bmAttributes = USB_CONFIG_ATTR_BUSPOWERED,
//         .bMaxPower = USB_CONFIG_POWER_MA(100)
//     },

//     .DFU_interface_association = {
//         .bLength = sizeof(USB_InterfaceAssociationDescriptor),
//         .bDescriptorType = USB_DTYPE_InterfaceAssociation,
//         .bFirstInterface = USB_INTERFACE_DFU,
//         .bInterfaceCount = 1,
//         .bFunctionClass = DFU_INTERFACE_CLASS,
//         .bFunctionSubClass = DFU_INTERFACE_SUBCLASS,
//         .bFunctionProtocol = DFU_DFU_MODE_PROTOCOL,
//         .iFunction = USB_STRING_DFU,
//     },

//     .DFU_interface = {
//         .bLength = sizeof(USB_InterfaceDescriptor),
//         .bDescriptorType = USB_DTYPE_Interface,
//         .bInterfaceNumber = USB_INTERFACE_DFU,
//         .bAlternateSetting = 0,
//         .bNumEndpoints = 0,
//         .bInterfaceClass = DFU_INTERFACE_CLASS,
//         .bInterfaceSubClass = DFU_INTERFACE_SUBCLASS,
//         .bInterfaceProtocol = DFU_DFU_MODE_PROTOCOL,
//         .iInterface = USB_STRING_DFU
//     },
//     .DFU_functional = {
//         .bLength = sizeof(DFU_FunctionalDescriptor),
//         .bDescriptorType = DFU_DESCRIPTOR_TYPE,
//         .bmAttributes = DFU_ATTR_CAN_DOWNLOAD | DFU_ATTR_CAN_UPLOAD | DFU_ATTR_WILL_DETACH,
//         .wDetachTimeout = 0,
//         .wTransferSize = DFU_TRANSFER_SIZE,
//         .bcdDFUVersion = 0x0110,
//     }
// };

// static const language_string_t language_string = {
//     .bLength = USB_STRING_LEN(1),
//     .bDescriptorType = USB_DTYPE_String,
//     .bString = { USB_LANGUAGE_EN_US }
// };



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
    // if the block number matches the first address in a row.
    if (block_number % (ROW_SIZE / DFU_TRANSFER_SIZE)) {
        return;
    }

    address = FLASH_FIRMWARE_START + (block_number * DFU_TRANSFER_SIZE);
    NVMCTRL->ADDR.reg = address >> 1;
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD(NVMCTRL_CTRLA_CMD_ER);
    while (!NVMCTRL->INTFLAG.bit.READY);
}


// ****************************************************************************
void dfu_cb_dnload_packet_completed(uint16_t block_number, uint16_t offset, uint8_t* payload, uint16_t length) {
    uint32_t address;
    uint32_t nvm_address;

    address = FLASH_FIRMWARE_START + (block_number * DFU_TRANSFER_SIZE) + offset;
    nvm_address = address / 2;

    // The NVM must be accessed as a series of 16-bit words. The tricky bit
    // is that we must ensure that we can write an odd number of bytes, i.e.
    // the last word may be incomplete.
    for (uint16_t i = 0; i < length; i += 2) {
        uint16_t word;

        word = payload[i];
        if ((i + 1) < length) {
            word |= payload[i + 1] << 8;
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
static void send_descriptor_multi(void) {
    uint16_t transfer_length = data_length;

    if (transfer_length > USB_EP0_SIZE) {
        transfer_length = USB_EP0_SIZE;
    }

    memcpy(ep0_buf_in, data, transfer_length);
    usb_ep_start_in(USB_IN, ep0_buf_in, transfer_length, false);

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
static void dfu_upload_block(uint16_t block_number, uint16_t length) {
    static uint32_t last_used_address = 0;
    uint32_t address;
    uint32_t transfer_length;

    // Find the last address in the flash that is not empty
    if (last_used_address == 0) {
        last_used_address = FLASH_FIRMWARE_START + FLASH_FIRMWARE_SIZE - 4;

        while (last_used_address > FLASH_FIRMWARE_START && *(uint32_t *)last_used_address == 0xffffffff) {
            last_used_address -= 4;
        }
    }


    if (length > DFU_TRANSFER_SIZE) {
        dfu_error(DFU_STATUS_errUNKNOWN);
        usb_ep0_stall();
        return;
    }

    if ((block_number * DFU_TRANSFER_SIZE) >= FLASH_FIRMWARE_SIZE) {
        usb_ep0_in(0);
        usb_ep0_out();
        return;
    }

    // Send the content of the given block number
    address = FLASH_FIRMWARE_START + (block_number * DFU_TRANSFER_SIZE);

    transfer_length = last_used_address - address + 4;
    if (transfer_length > DFU_TRANSFER_SIZE) {
        transfer_length = DFU_TRANSFER_SIZE;
    }

    memcpy(ep0_buf_in, (void *)address, transfer_length);
    usb_ep_start_in(USB_IN, ep0_buf_in, transfer_length, false);
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
                    address = usb_string_to_descriptor((char *)"Light Controller Programmer");
                    break;

                case USB_STRING_SERIAL_NUMBER:
                    address = samd_serial_number_string_descriptor();
                    break;

                case USB_STRING_DFU:
                    address = usb_string_to_descriptor((char *)"Light Controller Programmer (DFU-boot)");
                    break;

                default:
                    *ptr = NULL;
                    return 0;
            }
            size = (((USB_StringDescriptor*)address))->bLength;
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
    uint8_t requestType = usb_setup.bmRequestType & USB_REQTYPE_TYPE_MASK;

    if (recipient == USB_RECIPIENT_INTERFACE) {
        if (usb_setup.wIndex == USB_INTERFACE_DFU) {
            // The USB library does not support firmware upload, so we patch
            // it in ourself.
            if (usb_setup.bRequest == DFU_UPLOAD) {
                dfu_upload_block(usb_setup.wValue, usb_setup.wLength);
                return;
            }

            // Forward all other DFU related requests to the USB library
            dfu_control_setup();
            return;
        }
    }

    else if (recipient == USB_RECIPIENT_DEVICE  &&  requestType == USB_REQTYPE_VENDOR) {
        switch(usb_setup.bRequest) {
            // case VENDOR_CODE_WEBUSB:
            //     if (usb_setup.wIndex == WEBUSB_REQUEST_GET_URL) {
            //         send_descriptor(&landing_page_descriptor, sizeof(landing_page_descriptor_t));
            //         return;
            //     }
            //     break;

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
    uint8_t recipient = usb_setup.bmRequestType & USB_REQTYPE_RECIPIENT_MASK;

    if (recipient == USB_RECIPIENT_INTERFACE) {
        if (usb_setup.wIndex ==USB_INTERFACE_DFU) {
            dfu_control_in_completion();
        }
    }
    else {
        send_descriptor_multi();
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
