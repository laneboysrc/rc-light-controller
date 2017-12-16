#include <stdbool.h>
#include <string.h>
#include <stdalign.h>
#include <hal.h>

#include "hal_usb.h"
#include "usb_cdc.h"
#include "usb_descriptors.h"
#include <printf.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))



void usb_init(void);
// void usb_reset_endpoint(int ep, int dir);
// void usb_configure_endpoint(usb_endpoint_descriptor_t *desc);
// bool usb_endpoint_configured(int ep, int dir);
// int usb_endpoint_get_status(int ep, int dir);
// void usb_endpoint_set_feature(int ep, int dir);
// void usb_endpoint_clear_feature(int ep, int dir);
void usb_send(int ep, uint8_t *data, int size);
void usb_recv(int ep, uint8_t *data, int size);
void usb_control_send_zlp(void);
void usb_control_stall(void);
void usb_control_send(uint8_t *data, int size);
void usb_control_recv(void (*callback)(uint8_t *data, int size));
void usb_task(void);
// bool usb_handle_standard_request(usb_request_t *request);
void usb_set_callback(int ep, void (*callback)(int size));
bool usb_class_handle_request(usb_request_t *request);
void usb_configuration_callback(int config);

//  Standard requests
#define GET_STATUS                  0
#define CLEAR_FEATURE               1
#define SET_FEATURE                 3
#define SET_ADDRESS                 5
#define GET_DESCRIPTOR              6
#define SET_DESCRIPTOR              7
#define GET_CONFIGURATION           8
#define SET_CONFIGURATION           9
#define GET_INTERFACE               10
#define SET_INTERFACE               11

// bmRequestType
#define REQUEST_HOSTTODEVICE        0x00
#define REQUEST_DEVICETOHOST        0x80
#define REQUEST_DIRECTION           0x80

#define REQUEST_STANDARD            0x00
#define REQUEST_CLASS               0x20
#define REQUEST_VENDOR              0x40
#define REQUEST_TYPE                0x60

#define REQUEST_DEVICE              0x00
#define REQUEST_INTERFACE           0x01
#define REQUEST_ENDPOINT            0x02
#define REQUEST_OTHER               0x03
#define REQUEST_RECIPIENT           0x1F

typedef void (*usb_ep_callback_t)(int size);

static usb_ep_callback_t usb_ep_callbacks[USB_EPT_NUM];


enum
{
  USB_DEVICE_EPCFG_EPTYPE_DISABLED    = 0,
  USB_DEVICE_EPCFG_EPTYPE_CONTROL     = 1,
  USB_DEVICE_EPCFG_EPTYPE_ISOCHRONOUS = 2,
  USB_DEVICE_EPCFG_EPTYPE_BULK        = 3,
  USB_DEVICE_EPCFG_EPTYPE_INTERRUPT   = 4,
  USB_DEVICE_EPCFG_EPTYPE_DUAL_BANK   = 5,
};

enum
{
  USB_DEVICE_PCKSIZE_SIZE_8    = 0,
  USB_DEVICE_PCKSIZE_SIZE_16   = 1,
  USB_DEVICE_PCKSIZE_SIZE_32   = 2,
  USB_DEVICE_PCKSIZE_SIZE_64   = 3,
  USB_DEVICE_PCKSIZE_SIZE_128  = 4,
  USB_DEVICE_PCKSIZE_SIZE_256  = 5,
  USB_DEVICE_PCKSIZE_SIZE_512  = 6,
  USB_DEVICE_PCKSIZE_SIZE_1023 = 7,
};

typedef union
{
  UsbDeviceDescBank    bank[2];
  struct
  {
    UsbDeviceDescBank  out;
    UsbDeviceDescBank  in;
  } dir;
} udc_mem_t;

static alignas(4) udc_mem_t udc_mem[USB_EPT_NUM];
static alignas(4) uint8_t usb_ctrl_in_buf[64];
static alignas(4) uint8_t usb_ctrl_out_buf[64];
static void (*usb_control_recv_callback)(uint8_t *data, int size);




//-----------------------------------------------------------------------------
static void usb_reset_endpoint(int ep, int dir)
{
  if (USB_IN_ENDPOINT == dir)
    USB->DEVICE.DeviceEndpoint[ep].EPCFG.bit.EPTYPE1 = USB_DEVICE_EPCFG_EPTYPE_DISABLED;
  else
    USB->DEVICE.DeviceEndpoint[ep].EPCFG.bit.EPTYPE0 = USB_DEVICE_EPCFG_EPTYPE_DISABLED;
}

//-----------------------------------------------------------------------------
static void usb_configure_endpoint(usb_endpoint_descriptor_t *desc)
{
    int ep, dir, type, size;

    ep = desc->bEndpointAddress & USB_INDEX_MASK;
    dir = desc->bEndpointAddress & USB_DIRECTION_MASK;
    type = desc->bmAttributes & 0x03;
    size = desc->wMaxPacketSize & 0x3ff;

    usb_reset_endpoint(ep, dir);

    if (size <= 8) {
        size = USB_DEVICE_PCKSIZE_SIZE_8;
    }
    else if (size <= 16) {
        size = USB_DEVICE_PCKSIZE_SIZE_16;
    }
    else if (size <= 32) {
        size = USB_DEVICE_PCKSIZE_SIZE_32;
    }
    else if (size <= 64) {
        size = USB_DEVICE_PCKSIZE_SIZE_64;
    }
    else if (size <= 128) {
        size = USB_DEVICE_PCKSIZE_SIZE_128;
    }
    else if (size <= 256) {
        size = USB_DEVICE_PCKSIZE_SIZE_256;
    }
    else if (size <= 512) {
        size = USB_DEVICE_PCKSIZE_SIZE_512;
    }
    else if (size <= 1023) {
        size = USB_DEVICE_PCKSIZE_SIZE_1023;
    }
    else {
        size = USB_DEVICE_PCKSIZE_SIZE_8;
    }

    if (type == USB_CONTROL_ENDPOINT) {
        type = USB_DEVICE_EPCFG_EPTYPE_CONTROL;
    }
    else if (type == USB_ISOCHRONOUS_ENDPOINT) {
        type = USB_DEVICE_EPCFG_EPTYPE_ISOCHRONOUS;
    }
    else if (type == USB_BULK_ENDPOINT) {
        type = USB_DEVICE_EPCFG_EPTYPE_BULK;
    }
    else {
        type = USB_DEVICE_EPCFG_EPTYPE_INTERRUPT;
    }

    if (USB_IN_ENDPOINT == dir) {
        USB->DEVICE.DeviceEndpoint[ep].EPCFG.bit.EPTYPE1 = type;
        USB->DEVICE.DeviceEndpoint[ep].EPINTENSET.bit.TRCPT1 = 1;
        USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.DTGLIN = 1;
        USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.BK1RDY = 1;
        udc_mem[ep].dir.in.PCKSIZE.bit.SIZE = size;
    }
    else {
        USB->DEVICE.DeviceEndpoint[ep].EPCFG.bit.EPTYPE0 = type;
        USB->DEVICE.DeviceEndpoint[ep].EPINTENSET.bit.TRCPT0 = 1;
        USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.DTGLOUT = 1;
        USB->DEVICE.DeviceEndpoint[ep].EPSTATUSSET.bit.BK0RDY = 1;
        udc_mem[ep].dir.out.PCKSIZE.bit.SIZE = size;
    }
}


//-----------------------------------------------------------------------------
static bool usb_endpoint_configured(int ep, int dir)
{
    if (USB_IN_ENDPOINT == dir) {
        return (USB_DEVICE_EPCFG_EPTYPE_DISABLED != USB->DEVICE.DeviceEndpoint[ep].EPCFG.bit.EPTYPE1);
    }
    else {
        return (USB_DEVICE_EPCFG_EPTYPE_DISABLED != USB->DEVICE.DeviceEndpoint[ep].EPCFG.bit.EPTYPE0);
    }
}

//-----------------------------------------------------------------------------
static int usb_endpoint_get_status(int ep, int dir)
{
    if (USB_IN_ENDPOINT == dir) {
        return USB->DEVICE.DeviceEndpoint[ep].EPSTATUS.bit.STALLRQ1;
    }
    else {
        return USB->DEVICE.DeviceEndpoint[ep].EPSTATUS.bit.STALLRQ0;
    }
}

//-----------------------------------------------------------------------------
static void usb_endpoint_set_feature(int ep, int dir)
{
    if (USB_IN_ENDPOINT == dir) {
        USB->DEVICE.DeviceEndpoint[ep].EPSTATUSSET.bit.STALLRQ1 = 1;
    }
    else {
        USB->DEVICE.DeviceEndpoint[ep].EPSTATUSSET.bit.STALLRQ0 = 1;
    }
}

//-----------------------------------------------------------------------------
static void usb_endpoint_clear_feature(int ep, int dir)
{
    if (USB_IN_ENDPOINT == dir) {
        if (USB->DEVICE.DeviceEndpoint[ep].EPSTATUS.bit.STALLRQ1) {
            USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.STALLRQ1 = 1;

            if (USB->DEVICE.DeviceEndpoint[ep].EPINTFLAG.bit.STALL1) {
                USB->DEVICE.DeviceEndpoint[ep].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_STALL1;
                USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.DTGLIN = 1;
            }
        }
    }
    else {
        if (USB->DEVICE.DeviceEndpoint[ep].EPSTATUS.bit.STALLRQ0) {
            USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.STALLRQ0 = 1;

            if (USB->DEVICE.DeviceEndpoint[ep].EPINTFLAG.bit.STALL0) {
                USB->DEVICE.DeviceEndpoint[ep].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_STALL0;
                USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.DTGLOUT = 1;
            }
        }
    }
}

//-----------------------------------------------------------------------------
static bool send_descriptor(usb_request_t *request)
{
    int type = request->wValue >> 8;
    int index = request->wValue & 0xff;
    unsigned length = request->wLength;


    switch (type) {
        case USB_DEVICE_DESCRIPTOR:
            length = MIN(length, usb_device_descriptor.bLength);
            fprintf(STDOUT_DEBUG, "Get dev desc %d\n", length);

            usb_control_send((uint8_t *)&usb_device_descriptor, length);
            return true;

        case USB_CONFIGURATION_DESCRIPTOR:
            length = MIN(length, usb_configuration_hierarchy.configuration.wTotalLength);
            fprintf(STDOUT_DEBUG, "Get config desc %d\n", length);

            usb_control_send((uint8_t *)&usb_configuration_hierarchy, length);
            return true;

        case USB_STRING_DESCRIPTOR:
            if (index == 0) {
                length = MIN(length, sizeof(usb_language_descriptor));
                fprintf(STDOUT_DEBUG, "Get language descriptor %d\n", length);
                usb_control_send((uint8_t *)&usb_language_descriptor, length);
                return true;
            }

            if (index < USB_STR_COUNT) {
                const char *str = usb_strings[index];
                int len;

                fprintf(STDOUT_DEBUG, "Get string desc index %d %d 0x%x\n", index, length, str);

                for (len = 0; *str; len++, str++) {
                    usb_string_descriptor_buffer[2 + len*2] = *str;
                    usb_string_descriptor_buffer[3 + len*2] = 0;
                }

                usb_string_descriptor_buffer[0] = len*2 + 2;
                usb_string_descriptor_buffer[1] = USB_STRING_DESCRIPTOR;

                length = MIN(length, usb_string_descriptor_buffer[0]);

                usb_control_send(usb_string_descriptor_buffer, length);
                return true;
            }
            return false;

        default:
            return false;
    }


    // if (USB_DEVICE_DESCRIPTOR == type)
    // {
    //   length = MIN(length, usb_device_descriptor.bLength);
    //   fprintf(STDOUT_DEBUG, "Get dev desc %d\n", length);

    //   usb_control_send((uint8_t *)&usb_device_descriptor, length);
    // }

    // else if (USB_CONFIGURATION_DESCRIPTOR == type)
    // {

    //   length = MIN(length, usb_configuration_hierarchy.configuration.wTotalLength);
    //   fprintf(STDOUT_DEBUG, "Get config desc %d\n", length);

    //   usb_control_send((uint8_t *)&usb_configuration_hierarchy, length);
    // }

    // else if (USB_STRING_DESCRIPTOR == type)
    // {
    //   if (0 == index)
    //   {
    //     length = MIN(length, sizeof(usb_language_descriptor));
    //     fprintf(STDOUT_DEBUG, "Get string desc index 0 %d\n", length);

    //     usb_control_send((uint8_t *)&usb_language_descriptor, length);
    //   }
    //   else if (index < USB_STR_COUNT)
    //   {
    //     const char *str = usb_strings[index];
    //     int len;

    //     fprintf(STDOUT_DEBUG, "Get string desc index %d %d 0x%x\n", index, length, str);

    //     for (len = 0; *str; len++, str++) {
    //       usb_string_descriptor_buffer[2 + len*2] = *str;
    //       usb_string_descriptor_buffer[3 + len*2] = 0;
    //     }

    //     usb_string_descriptor_buffer[0] = len*2 + 2;
    //     usb_string_descriptor_buffer[1] = USB_STRING_DESCRIPTOR;

    //     length = MIN(length, usb_string_descriptor_buffer[0]);

    //     usb_control_send(usb_string_descriptor_buffer, length);
    //   }
    //   else
    //   {
    //     return false;
    //   }
    // }
    // else
    // {
    //   return false;
    // }

    // return true;
}


//-----------------------------------------------------------------------------
static bool usb_handle_standard_request(usb_request_t *request)
{
    static uint8_t usb_config = 0;

    if ((request->bmRequestType & REQUEST_TYPE) == REQUEST_STANDARD) {
        switch (request->bRequest) {
            case GET_CONFIGURATION:
                usb_control_send(&usb_config, sizeof(usb_config));
                fprintf(STDOUT_DEBUG, "Get config\n");
                return true;

            case GET_DESCRIPTOR:
                return send_descriptor(request);


            case SET_ADDRESS:
                usb_control_send_zlp();
                USB->DEVICE.DADD.reg = USB_DEVICE_DADD_ADDEN | USB_DEVICE_DADD_DADD(request->wValue);
                fprintf(STDOUT_DEBUG, "SET_ADDRESS 0x%x\n", request->wValue);
                return true;

            case SET_CONFIGURATION:
                if ((request->bmRequestType & REQUEST_RECIPIENT) == REQUEST_DEVICE) {
                    int size;
                    usb_descriptor_header_t *desc;

                    usb_config = request->wValue;

                    fprintf(STDOUT_DEBUG, "Set config desc %d\n", usb_config);

                    size = usb_configuration_hierarchy.configuration.wTotalLength;
                    desc = (usb_descriptor_header_t *)&usb_configuration_hierarchy;

                    while (size) {
                        if (USB_ENDPOINT_DESCRIPTOR == desc->bDescriptorType) {
                            usb_configure_endpoint((usb_endpoint_descriptor_t *)desc);
                        }

                        size -= desc->bLength;
                        desc = (usb_descriptor_header_t *)((uint8_t *)desc + desc->bLength);
                    }

                    usb_configuration_callback(usb_config);
                    usb_control_send_zlp();
                    return true;
                }
                return false;

            case GET_STATUS:
                if (request->bmRequestType == REQUEST_DEVICE) {
                    uint16_t status = 0;

                    usb_control_send((uint8_t *)&status, sizeof(status));
                    fprintf(STDOUT_DEBUG, "Get Device status\n");
                    return true;
                }
                else if (request->bmRequestType == REQUEST_ENDPOINT) {
                    int ep = request->wIndex & USB_INDEX_MASK;
                    int dir = request->wIndex & USB_DIRECTION_MASK;
                    uint16_t status = 0;

                    if (usb_endpoint_configured(ep, dir)) {
                        fprintf(STDOUT_DEBUG, "Get EP status %d\n", ep);
                        status = usb_endpoint_get_status(ep, dir);
                        usb_control_send((uint8_t *)&status, sizeof(status));
                        return true;
                    }
                    else {
                        return false;
                    }
                }
                break;

            case SET_FEATURE:
                fprintf(STDOUT_DEBUG, "SET_FEATURE %d\n", request->bmRequestType);
                if (request->bmRequestType == REQUEST_DEVICE) {
                    return false;
                }
                else if (request->bmRequestType == REQUEST_INTERFACE) {
                    usb_control_send_zlp();
                    return true;
                }
                else if (request->bmRequestType == REQUEST_ENDPOINT) {
                    int ep = request->wIndex & USB_INDEX_MASK;
                    int dir = request->wIndex & USB_DIRECTION_MASK;

                    if (0 == request->wValue && ep && usb_endpoint_configured(ep, dir)) {
                        usb_endpoint_set_feature(ep, dir);
                        usb_control_send_zlp();
                        return true;
                    }
                    return false;
                }

            case CLEAR_FEATURE:
                fprintf(STDOUT_DEBUG, "CLEAR_FEATURE %d\n", request->bmRequestType);
                if (request->bmRequestType == REQUEST_DEVICE) {
                    return false;
                }
                else if (request->bmRequestType == REQUEST_INTERFACE) {
                    usb_control_send_zlp();
                    return true;
                }
                else if (request->bmRequestType == REQUEST_ENDPOINT) {
                    int ep = request->wIndex & USB_INDEX_MASK;
                    int dir = request->wIndex & USB_DIRECTION_MASK;

                    if (0 == request->wValue && ep && usb_endpoint_configured(ep, dir)) {
                        usb_endpoint_clear_feature(ep, dir);
                        usb_control_send_zlp();
                        return true;
                    }
                    return false;
                }
                break;

            default:
                break;
        }
    }

    // FIXME: move that in the calling function
    return usb_class_handle_request(request);

  //       reurn false;

  // switch ((request->bRequest << 8) | request->bmRequestType)
  // {
  //   // case USB_CMD(IN, DEVICE, STANDARD, GET_DESCRIPTOR):
  //   // {
  //   //   int type = request->wValue >> 8;
  //   //   int index = request->wValue & 0xff;
  //   //   int length = request->wLength;

  //   //   if (USB_DEVICE_DESCRIPTOR == type)
  //   //   {
  //   //     length = MIN(length, usb_device_descriptor.bLength);
  //   //     fprintf(STDOUT_DEBUG, "Get dev desc %d\n", length);

  //   //     usb_control_send((uint8_t *)&usb_device_descriptor, length);
  //   //   }

  //   //   else if (USB_CONFIGURATION_DESCRIPTOR == type)
  //   //   {

  //   //     length = MIN(length, usb_configuration_hierarchy.configuration.wTotalLength);
  //   //     fprintf(STDOUT_DEBUG, "Get config desc %d\n", length);

  //   //     usb_control_send((uint8_t *)&usb_configuration_hierarchy, length);
  //   //   }

  //   //   else if (USB_STRING_DESCRIPTOR == type)
  //   //   {
  //   //     if (0 == index)
  //   //     {
  //   //       length = MIN(length, usb_string_descriptor_zero.bLength);
  //   //     fprintf(STDOUT_DEBUG, "Get string desc index 0 %d\n", length);

  //   //       usb_control_send((uint8_t *)&usb_string_descriptor_zero, length);
  //   //     }
  //   //     else if (index < USB_STR_COUNT)
  //   //     {
  //   //       const char *str = usb_strings[index];
  //   //       int len;

  //   //       fprintf(STDOUT_DEBUG, "Get string desc index %d %d 0x%x\n", index, length, str);

  //   //       for (len = 0; *str; len++, str++) {
  //   //         usb_string_descriptor_buffer[2 + len*2] = *str;
  //   //         usb_string_descriptor_buffer[3 + len*2] = 0;
  //   //       }

  //   //       usb_string_descriptor_buffer[0] = len*2 + 2;
  //   //       usb_string_descriptor_buffer[1] = USB_STRING_DESCRIPTOR;

  //   //       length = MIN(length, usb_string_descriptor_buffer[0]);

  //   //       usb_control_send(usb_string_descriptor_buffer, length);
  //   //     }
  //   //     else
  //   //     {
  //   //       return false;
  //   //     }
  //   //   }
  //   //   else
  //   //   {
  //   //     return false;
  //   //   }
  //   // } break;

  //   // case USB_CMD(OUT, DEVICE, STANDARD, SET_ADDRESS):
  //   // {
  //   //   usb_control_send_zlp();
  //   //   USB->DEVICE.DADD.reg = USB_DEVICE_DADD_ADDEN | USB_DEVICE_DADD_DADD(request->wValue);
  //   //     fprintf(STDOUT_DEBUG, "SET_ADDRESS 0x%x\n", request->wValue);

  //   // } break;

  //   // case USB_CMD(OUT, DEVICE, STANDARD, SET_CONFIGURATION):
  //   // {
  //   //   usb_config = request->wValue;

  //   //     fprintf(STDOUT_DEBUG, "Set config desc %d\n", usb_config);

  //   //   usb_control_send_zlp();

  //   //   // FIXME: usb_config == 1?
  //   //   if (usb_config) {
  //   //     int size = usb_configuration_hierarchy.configuration.wTotalLength;
  //   //     usb_descriptor_header_t *desc = (usb_descriptor_header_t *)&usb_configuration_hierarchy;

  //   //     while (size)
  //   //     {
  //   //       if (USB_ENDPOINT_DESCRIPTOR == desc->bDescriptorType)
  //   //         usb_configure_endpoint((usb_endpoint_descriptor_t *)desc);

  //   //       size -= desc->bLength;
  //   //       desc = (usb_descriptor_header_t *)((uint8_t *)desc + desc->bLength);
  //   //     }

  //   //     usb_configuration_callback(usb_config);
  //   //   }
  //   // } break;

  //   // case USB_CMD(IN, DEVICE, STANDARD, GET_CONFIGURATION):
  //   // {
  //   //   uint8_t config = usb_config;
  //   //   usb_control_send(&config, sizeof(config));
  //   //     fprintf(STDOUT_DEBUG, "Get config\n");
  //   // } break;

  //   // case USB_CMD(IN, DEVICE, STANDARD, GET_STATUS):
  //   // case USB_CMD(IN, INTERFACE, STANDARD, GET_STATUS):
  //   // {
  //   //     uint16_t status = 0;
  //   //     usb_control_send((uint8_t *)&status, sizeof(status));
  //   //     fprintf(STDOUT_DEBUG, "Get status\n");
  //   // } break;

  //   // case USB_CMD(OUT, DEVICE, STANDARD, SET_FEATURE):
  //   // case USB_CMD(OUT, DEVICE, STANDARD, CLEAR_FEATURE):
  //   //     return false;

  //   // case USB_CMD(OUT, INTERFACE, STANDARD, SET_FEATURE):
  //   // case USB_CMD(OUT, INTERFACE, STANDARD, CLEAR_FEATURE):
  //   //     usb_control_send_zlp();
  //   //     fprintf(STDOUT_DEBUG, "Set or Clear feature\n");
  //   //     break;

  //   // case USB_CMD(IN, ENDPOINT, STANDARD, GET_STATUS):
  //   // {
  //   //   int ep = request->wIndex & USB_INDEX_MASK;
  //   //   int dir = request->wIndex & USB_DIRECTION_MASK;
  //   //   uint16_t status = 0;

  //   //   if (usb_endpoint_configured(ep, dir))
  //   //   {
  //   //     status = usb_endpoint_get_status(ep, dir);
  //   //     usb_control_send((uint8_t *)&status, sizeof(status));
  //   //   }
  //   //   else
  //   //   {
  //   //     return false;
  //   //   }
  //   // } break;

  //   // case USB_CMD(OUT, ENDPOINT, STANDARD, SET_FEATURE):
  //   // {
  //   //   int ep = request->wIndex & USB_INDEX_MASK;
  //   //   int dir = request->wIndex & USB_DIRECTION_MASK;

  //   //   if (0 == request->wValue && ep && usb_endpoint_configured(ep, dir))
  //   //   {
  //   //     usb_endpoint_set_feature(ep, dir);
  //   //     usb_control_send_zlp();
  //   //   }
  //   //   else
  //   //   {
  //   //     return false;
  //   //   }
  //   // } break;

  //   // case USB_CMD(OUT, ENDPOINT, STANDARD, CLEAR_FEATURE):
  //   // {
  //   //   int ep = request->wIndex & USB_INDEX_MASK;
  //   //   int dir = request->wIndex & USB_DIRECTION_MASK;

  //   //   if (0 == request->wValue && ep && usb_endpoint_configured(ep, dir))
  //   //   {
  //   //     usb_endpoint_clear_feature(ep, dir);
  //   //     usb_control_send_zlp();
  //   //   }
  //   //   else
  //   //   {
  //   //     return false;
  //   //   }
  //   // } break;

  //   default:
  //   {
  //     if (!usb_class_handle_request(request))
  //       return false;
  //   } break;
  // }

  // return true;
}





//-----------------------------------------------------------------------------
static void service_end_of_reset(void)
{
    if (USB->DEVICE.INTFLAG.bit.EORST)
    {
        USB->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_EORST;
        USB->DEVICE.DADD.reg = USB_DEVICE_DADD_ADDEN;

        for (int i = 0; i < USB_EPT_NUM; i++) {
            usb_reset_endpoint(i, USB_IN_ENDPOINT);
            usb_reset_endpoint(i, USB_OUT_ENDPOINT);
        }

        USB->DEVICE.DeviceEndpoint[0].EPCFG.reg =
            USB_DEVICE_EPCFG_EPTYPE0(USB_DEVICE_EPCFG_EPTYPE_CONTROL) |
            USB_DEVICE_EPCFG_EPTYPE1(USB_DEVICE_EPCFG_EPTYPE_CONTROL);
        USB->DEVICE.DeviceEndpoint[0].EPSTATUSSET.bit.BK0RDY = 1;
        USB->DEVICE.DeviceEndpoint[0].EPSTATUSCLR.bit.BK1RDY = 1;

        udc_mem[0].dir.in.PCKSIZE.bit.SIZE = USB_DEVICE_PCKSIZE_SIZE_64;

        udc_mem[0].dir.out.ADDR.reg = (uint32_t)usb_ctrl_out_buf;
        udc_mem[0].dir.out.PCKSIZE.bit.SIZE = USB_DEVICE_PCKSIZE_SIZE_64;
        udc_mem[0].dir.out.PCKSIZE.bit.MULTI_PACKET_SIZE = 64;
        udc_mem[0].dir.out.PCKSIZE.bit.BYTE_COUNT = 0;

        USB->DEVICE.DeviceEndpoint[0].EPINTENSET.bit.RXSTP = 1;
        USB->DEVICE.DeviceEndpoint[0].EPINTENSET.bit.TRCPT0 = 1;
    }
}


//-----------------------------------------------------------------------------
static void service_ep0_receive_setup(void)
{
    usb_request_t *request;

    if (!USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.bit.RXSTP) {
        return;
    }

    request = (usb_request_t *)usb_ctrl_out_buf;

    if (sizeof(usb_request_t) == udc_mem[0].dir.out.PCKSIZE.bit.BYTE_COUNT) {
        if (usb_handle_standard_request(request)) {
            udc_mem[0].dir.out.PCKSIZE.bit.BYTE_COUNT = 0;
            USB->DEVICE.DeviceEndpoint[0].EPSTATUSCLR.bit.BK0RDY = 1;
            USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;
        }
        else {
            usb_control_stall();
        }
    }
    else {
        usb_control_stall();
    }

    USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_RXSTP;
}


//-----------------------------------------------------------------------------
static void service_ep0_transmit_complete(void)
{
    if (!USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.bit.TRCPT0) {
        return;
    }

    if (usb_control_recv_callback) {
        usb_control_recv_callback(usb_ctrl_out_buf, udc_mem[0].dir.out.PCKSIZE.bit.BYTE_COUNT);
        usb_control_recv_callback = NULL;
        usb_control_send_zlp();
    }

    USB->DEVICE.DeviceEndpoint[0].EPSTATUSSET.bit.BK0RDY = 1;
    USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;
}


//-----------------------------------------------------------------------------
static void service_endpoints(void)
{
    int epints;

    epints = USB->DEVICE.EPINTSMRY.reg;

    for (int i = 1; i < USB_EPT_NUM && epints > 0; i++) {
        int flags;

        flags = USB->DEVICE.DeviceEndpoint[i].EPINTFLAG.reg;
        epints &= ~(1 << i);

        // Transmit Complete 0 (OUT / data sent from host to device)
        if (flags & USB_DEVICE_EPINTFLAG_TRCPT0) {
            USB->DEVICE.DeviceEndpoint[i].EPSTATUSSET.bit.BK0RDY = 1;
            USB->DEVICE.DeviceEndpoint[i].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;

            if (usb_ep_callbacks[i]) {
            usb_ep_callbacks[i](udc_mem[i].dir.out.PCKSIZE.bit.BYTE_COUNT);
            }
        }

        // Transmit Complete 1 (IN / data device to host)
        if (flags & USB_DEVICE_EPINTFLAG_TRCPT1) {
            USB->DEVICE.DeviceEndpoint[i].EPSTATUSCLR.bit.BK1RDY = 1;
            USB->DEVICE.DeviceEndpoint[i].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;

            if (usb_ep_callbacks[i]) {
                usb_ep_callbacks[i](0);
            }
        }
    }
}

//-----------------------------------------------------------------------------
void usb_init(void)
{
    GCLK->CLKCTRL.reg =
      GCLK_CLKCTRL_ID(USB_GCLK_ID) |
      GCLK_CLKCTRL_GEN(0) |
      GCLK_CLKCTRL_CLKEN;

    USB->DEVICE.CTRLA.bit.SWRST = 1;
    while (USB->DEVICE.SYNCBUSY.bit.SWRST);

    for (uint32_t i = 0; i < (sizeof(udc_mem) / sizeof(uint32_t)); i++) {
        ((uint32_t *)udc_mem)[i] = 0;
    }

    USB->DEVICE.DESCADD.reg = (uint32_t)udc_mem;

    USB->DEVICE.CTRLA.bit.MODE = USB_CTRLA_MODE_DEVICE_Val;
    USB->DEVICE.CTRLA.bit.RUNSTDBY = 1;
    USB->DEVICE.CTRLB.bit.SPDCONF = USB_DEVICE_CTRLB_SPDCONF_FS_Val;
    USB->DEVICE.CTRLB.bit.DETACH = 0;

    USB->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_EORST;
    USB->DEVICE.DeviceEndpoint[0].EPINTENSET.bit.RXSTP = 1;

    USB->DEVICE.CTRLA.reg |= USB_CTRLA_ENABLE;

    for (int i = 0; i < USB_EPT_NUM; i++) {
        usb_ep_callbacks[i] = NULL;
        usb_reset_endpoint(i, USB_IN_ENDPOINT);
        usb_reset_endpoint(i, USB_OUT_ENDPOINT);
    }

    // FIXME: usb_serial_number from device unique ID
}


//-----------------------------------------------------------------------------
void usb_task(void)
{
    service_end_of_reset();
    service_ep0_receive_setup();
    service_ep0_transmit_complete();
    service_endpoints();
}

//-----------------------------------------------------------------------------
void usb_send(int ep, uint8_t *data, int size)
{
    udc_mem[ep].dir.in.ADDR.reg = (uint32_t)data;
    udc_mem[ep].dir.in.PCKSIZE.bit.BYTE_COUNT = size;
    udc_mem[ep].dir.in.PCKSIZE.bit.MULTI_PACKET_SIZE = 0;

    USB->DEVICE.DeviceEndpoint[ep].EPSTATUSSET.bit.BK1RDY = 1;
}

//-----------------------------------------------------------------------------
void usb_recv(int ep, uint8_t *data, int size)
{
    udc_mem[ep].dir.out.ADDR.reg = (uint32_t)data;
    udc_mem[ep].dir.out.PCKSIZE.bit.MULTI_PACKET_SIZE = size;
    udc_mem[ep].dir.out.PCKSIZE.bit.BYTE_COUNT = 0;

    USB->DEVICE.DeviceEndpoint[ep].EPSTATUSCLR.bit.BK0RDY = 1;
}

//-----------------------------------------------------------------------------
void usb_control_send_zlp(void)
{
    udc_mem[0].dir.in.PCKSIZE.bit.BYTE_COUNT = 0;
    USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;
    USB->DEVICE.DeviceEndpoint[0].EPSTATUSSET.bit.BK1RDY = 1;

    while (0 == USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.bit.TRCPT1);
}

//-----------------------------------------------------------------------------
void usb_control_stall(void)
{
    USB->DEVICE.DeviceEndpoint[0].EPSTATUSSET.bit.STALLRQ1 = 1;
}

//-----------------------------------------------------------------------------
void usb_control_send(uint8_t *data, int size)
{
    // USB controller does not have access to the flash memory, so here we do
    // a manual multi-packet transfer. This way data can be located in in
    // the flash memory (big constant descriptors).

    while (size) {
        int transfer_size = MIN(size, usb_device_descriptor.bMaxPacketSize0);

        for (uint16_t i = 0; i < transfer_size; i++) {
            usb_ctrl_in_buf[i] = data[i];
        }

        udc_mem[0].dir.in.ADDR.reg = (uint32_t)usb_ctrl_in_buf;
        udc_mem[0].dir.in.PCKSIZE.bit.BYTE_COUNT = transfer_size;
        udc_mem[0].dir.in.PCKSIZE.bit.MULTI_PACKET_SIZE = 0;

        USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;
        USB->DEVICE.DeviceEndpoint[0].EPSTATUSSET.bit.BK1RDY = 1;

        while (!USB->DEVICE.DeviceEndpoint[0].EPINTFLAG.bit.TRCPT1);

        size -= transfer_size;
        data += transfer_size;
    }
}


//-----------------------------------------------------------------------------
void usb_control_recv(void (* callback)(uint8_t *data, int size))
{
    usb_control_recv_callback = callback;
}


//-----------------------------------------------------------------------------
void usb_set_callback(int ep, void (*callback)(int size))
{
    usb_ep_callbacks[ep] = callback;
}






