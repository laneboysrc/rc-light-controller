#include <stdint.h>
#include "ch552.h"
#include "usb.h"

// USB setup request type and values
typedef struct {
    uint8_t bRequestType;
    uint8_t bRequest;
    uint8_t wValueL;
    uint8_t wValueH;
    uint8_t wIndexL;
    uint8_t wIndexH;
    uint8_t wLengthL;
    uint8_t wLengthH;
} USB_SETUP_REQ, *pUSB_SETUP_REQ;

// Convenience define pointing to the EP0 buffer for handling USB setup requests
#define usb_setup ((pUSB_SETUP_REQ) ep0_buffer)


__xdata __at (EP0_ADDR) uint8_t ep0_buffer[EP0_SIZE];
__xdata __at (EP1_ADDR) uint8_t ep1_buffer[EP1_SIZE];
__xdata __at (EP2_ADDR) uint8_t ep2_buffer[EP2_SIZE];

// active_usb_configuration is 0 if the device is not configured (not
// connected to a host), or the current active configuration (usually 1 since
// devices most of the time only have one configuration)
uint8_t active_usb_configuration;

// Current request
static uint8_t bRequest;
static uint8_t requestType;
static uint8_t recipient;

// Maximum count of data for the current request
static uint16_t wSetupLength;

// wValue and wIndex field for standard requests
static uint16_t wValue;
static uint16_t wIndex;

// Pointer to the next descriptor byte to send. Required to be persistent because
// multiple transfers may be needed for long descriptors.
const uint8_t *pDescr;

// Location of the first chip ID byte, which we use to create the unique USB
// serial number in serial_string_descriptor
__code __at (ROM_CHIP_ID_HX+1) uint8_t uniqueID;


// ****************************************************************************
static void send_serial_string_descriptor(void)
{
    uint8_t len;
    uint8_t digit;
    uint8_t i = 0;

    const char *lookup = "0123456789ABCDEF";
    const uint8_t *pUniqueID = &uniqueID;

    // IMPORTANT: this code only works if the whole serial number string
    // descriptor fits into EP0 in one go!
    // Ensure that EP0_SIZE is >= 22 in the Makefile!
    //
    // Alternatively don't use this function and use a hard-coded serial
    // number string in usb_descriptors.c

    len = 22;                   // = 2 + 2*5 (wide) hexdigits

    ep0_buffer[i++] = len;      // bLength
    ep0_buffer[i++] = 0x03;     // USB string descriptor type

    while (i < 22) {
        digit = *(pUniqueID++);
        ep0_buffer[i++] = lookup[digit >> 4];
        ep0_buffer[i++] = 0;
        ep0_buffer[i++] = lookup[digit & 0x0f];
        ep0_buffer[i++] = 0;
    }

    if (len > wSetupLength) {
        len = wSetupLength;
    }
    UEP0_T_LEN = len;
    UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;

    // Remainding data length must always be 0
    wSetupLength = 0;
}


// ****************************************************************************
static void USB_ep0_stall()
{
    bRequest = 0xff;
    UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL;
}


// ****************************************************************************
static void USB_ep0_send(uint8_t *descriptor, uint16_t length)
{
    uint8_t i;

    pDescr = descriptor;

    if (wSetupLength > length) {
        wSetupLength = length;
    }
    length = (wSetupLength >= EP0_SIZE) ? EP0_SIZE : wSetupLength;

    for (i = 0; i < length; i++) {
        ep0_buffer[i] = pDescr[i];
    }

    wSetupLength -= length;
    pDescr += length;

    UEP0_T_LEN = length;
    UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
}

// ****************************************************************************
static void USB_ep0_ack()
{
    UEP0_T_LEN = 0;
    UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
}


// ****************************************************************************
static void USB_EP0_SETUP(void)
{
    if (USB_RX_LEN != sizeof(USB_SETUP_REQ)) {
        USB_ep0_stall();
        return;
    }

    recipient = usb_setup->bRequestType & USB_REQTYPE_RECIPIENT_MASK;
    requestType = usb_setup->bRequestType & USB_REQTYPE_TYPE_MASK;

    bRequest = usb_setup->bRequest;
    wSetupLength = ((uint16_t)usb_setup->wLengthH << 8) | (usb_setup->wLengthL);
    wValue = ((uint16_t)usb_setup->wValueH << 8) | usb_setup->wValueL;
    wIndex = ((uint16_t)usb_setup->wIndexH << 8) | usb_setup->wIndexL;


    // Standard request
    if (requestType == USB_REQTYPE_STANDARD && recipient == USB_RECIPIENT_DEVICE) {
        switch (bRequest) {
            case USB_REQ_GET_DESCRIPTOR:
                switch (usb_setup->wValueH) {
                    case USB_DTYPE_DEVICE:
                        USB_ep0_send(device_descriptor, device_descriptor_length);
                        return;

                    case USB_DTYPE_CONFIGURATION:
                        USB_ep0_send(configuration_descriptor, configuration_descriptor_length);
                        return;

                    case USB_DTYPE_STRING:
                        switch (usb_setup->wValueL) {
                            case USB_STRING_LANGUAGE:
                                USB_ep0_send(language_descriptor, language_descriptor_length);
                                return;

                            case USB_STRING_MANUFACTURER:
                                USB_ep0_send(manufacturer_string_descriptor, manufacturer_string_descriptor_length);
                                return;

                            case USB_STRING_PRODUCT:
                                USB_ep0_send(product_string_descriptor, product_string_descriptor_length);
                                return;

                            case USB_STRING_SERIAL:
                                // pDescr = (__code uint8_t *)serial_string_descriptor;
                                // len = serial_string_descriptor_length;
                                send_serial_string_descriptor();
                                return;

                            case USB_STRING_DFU:
                                USB_ep0_send(dfu_string_descriptor, dfu_string_descriptor_length);
                                return;

                            case USB_STRING_TEST:
                                USB_ep0_send(test_string_descriptor, test_string_descriptor_length);
                                return;

                            default:
                                USB_ep0_stall();
                                return;
                        }

                    case USB_DTYPE_BOS:
                        USB_ep0_send(bos_descriptor, bos_descriptor_length);
                        return;

                    default:
                        USB_ep0_stall();
                        return;
                }

            case USB_REQ_SET_ADDRESS:
                USB_ep0_ack();
                return;

            case USB_REQ_SET_CONFIGURATION:
                active_usb_configuration = wValue;
                USB_ep0_ack();
                return;

            case USB_REQ_GET_CONFIGURATION:
                USB_ep0_send(&active_usb_configuration, 1);
                return;

            case USB_REQ_GET_STATUS:
                ep0_buffer[0] = 0x00;
                ep0_buffer[1] = 0x00;
                USB_ep0_send(ep0_buffer, 2);
                return;

            case USB_REQ_SET_FEATURE:
            case USB_REQ_CLEAR_FEATURE:
            case USB_REQ_SET_INTERFACE:
                USB_ep0_ack();
                return;

            default:
                USB_ep0_stall();
                return;
        }
    }

    // Vendor specific request
    else if (requestType == USB_REQTYPE_VENDOR && recipient == USB_RECIPIENT_DEVICE) {
        switch (bRequest) {
            case VENDOR_CODE_COMMAND:
                if (COMMAND_handler(wValue)) {
                    USB_ep0_ack();
                    return;
                }
                USB_ep0_stall();
                return;

            case VENDOR_CODE_WEBUSB:
                if (wIndex == WEBUSB_REQUEST_GET_URL) {
                    USB_ep0_send(landing_page_descriptor, landing_page_descriptor_length);
                    return;
                }
                USB_ep0_stall();
                return;

            case VENDOR_CODE_MS:
                if (wIndex == WINUSB_REQUEST_DESCRIPTOR) {
                    USB_ep0_send(ms_os_20_descriptor, ms_os_20_descriptor_length);
                    return;
                }
                USB_ep0_stall();
                return;

            default:
                USB_ep0_stall();
                return;
        }
    }

    // DFU
    else if (recipient == USB_RECIPIENT_INTERFACE) {
        if (wIndex == USB_INTERFACE_DFU) {
            switch (bRequest) {
                case DFU_DETACH:
                    BOOTLOADER_start();
                    USB_ep0_ack();
                    return;

                case DFU_GETSTATUS:
                    ep0_buffer[0] = 0;      // bStatus = OK
                    ep0_buffer[1] = 0;      // bwPollTimeout[0]
                    ep0_buffer[2] = 0;      // bwPollTimeout[1]
                    ep0_buffer[3] = 0;      // bwPollTimeout[2]
                    ep0_buffer[4] = 0;      // bState = APP IDLE
                    ep0_buffer[5] = 0;      // iString
                    USB_ep0_send(ep0_buffer, 6);
                    return;

                default:
                    break;
            }
        }

        USB_ep0_stall();
        return;
    }

    USB_ep0_stall();
    return;
}


// ****************************************************************************
static void USB_EP0_IN(void)
{
    uint8_t i;
    uint8_t count;

    switch (bRequest) {
        case USB_REQ_GET_DESCRIPTOR:
        case VENDOR_CODE_WEBUSB:
        case VENDOR_CODE_MS:
            count = (wSetupLength >= EP0_SIZE) ? EP0_SIZE : wSetupLength;

            for (i = 0; i < count; i++){
                ep0_buffer[i] = pDescr[i];
            }

            wSetupLength -= count;
            pDescr += count;

            UEP0_T_LEN = count;
            UEP0_CTRL ^= bUEP_T_TOG;
            break;

        case USB_REQ_SET_ADDRESS:
            USB_DEV_AD = wValue;
            UEP0_T_LEN = 0;
            UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
            break;

        default:
            break;
    }
}


// ****************************************************************************
static void USB_EP0_OUT(void)
{
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
}


// ****************************************************************************
static void USB_EP1_OUT(void)
{
    if (U_TOG_OK) {
        DATA_received(USB_RX_LEN);
        UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_R_RES | UEP_R_RES_NAK;
    }
}


// ****************************************************************************
void USB_EP1_ack(void)
{
    UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_R_RES | UEP_R_RES_ACK;
}


// ****************************************************************************
static void USB_EP2_IN(void)
{
    UEP2_T_LEN = 0;
    UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;
    DATA_sent();
}


// ****************************************************************************
void USB_EP2_send(uint8_t byte_count)
{
    UEP2_T_LEN = byte_count;
    UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;
}


// ****************************************************************************
void USB_init(void)
{
    // Enable internal pull-up
    // Automatically return to NAK until interrupt flag is cleared
    // Enable DMA mode
    USB_CTRL = bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;

    // Clear the USB device address
    USB_DEV_AD = 0x00;

    // Disable DP/DM pull-down resistor, enable the physical port
    UDEV_CTRL = bUD_PD_DIS | bUD_PORT_EN;

    UEP0_DMA = (uint16_t)ep0_buffer;
    UEP1_DMA = (uint16_t)ep1_buffer;
    UEP2_DMA = (uint16_t)ep2_buffer;

    // EP0 manual sync flag, OUT returns ACK, IN returns NAK
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;

    UEP4_1_MOD = bUEP1_RX_EN;
    UEP1_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK;

    UEP2_3_MOD = bUEP2_TX_EN;
    UEP2_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK;

    // Clear all interrupt flags
    USB_INT_FG = 0xff;

    UEP0_T_LEN = 0;
    UEP1_T_LEN = 0;
    UEP2_T_LEN = 0;
}


// ****************************************************************************
void USB_handle_events(void)
{
    if ((USB_INT_FG & 0x07) == 0) {
        return;
    }

    //---------------------------------
    if (UIF_TRANSFER) {
        uint8_t token = USB_INT_ST & MASK_UIS_TOKEN;
        uint8_t ep = USB_INT_ST & MASK_UIS_ENDP;

        switch (token)
            case UIS_TOKEN_OUT: {
                switch (ep) {
                    case 0:
                        USB_EP0_OUT();
                        break;
                    case 1:
                        USB_EP1_OUT();
                        break;
                    default:
                        break;
                }
                break;

            case UIS_TOKEN_IN:
                switch (ep) {
                    case 0:
                        USB_EP0_IN();
                        break;
                    case 2:
                        USB_EP2_IN();
                        break;
                    default:
                        break;
                }
                break;

            case UIS_TOKEN_SETUP:
                if (ep == 0) {
                    USB_EP0_SETUP();
                }
                break;

            default:
                break;
        }

        UIF_TRANSFER = 0;
    }

    //---------------------------------
    if (UIF_BUS_RST) {
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        UEP1_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK;
        UEP2_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK;

        USB_DEV_AD = 0x00;

        UIF_SUSPEND = 0;
        UIF_TRANSFER = 0;
        UIF_BUS_RST = 0;

        active_usb_configuration = 0;
    }

    //---------------------------------
    if (UIF_SUSPEND) {
        UIF_SUSPEND = 0;
        return;
    }
}
