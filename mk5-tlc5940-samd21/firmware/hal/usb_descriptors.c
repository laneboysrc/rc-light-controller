#include <stdalign.h>
#include <hal.h>

#include "hal_usb.h"
#include "usb_descriptors.h"

const alignas(4) usb_device_descriptor_t usb_device_descriptor =
{
  .bLength            = sizeof(usb_device_descriptor_t),
  .bDescriptorType    = USB_DEVICE_DESCRIPTOR,
  .bcdUSB             = 0x0110,
  .bDeviceClass       = USB_CDC_DEVICE_CLASS,
  .bDeviceSubClass    = 0,
  .bDeviceProtocol    = 0,
  .bMaxPacketSize0    = 64,
  .idVendor           = 0x6666,
  .idProduct          = 0x8888,
  .bcdDevice          = 0x0100,
  .iManufacturer      = USB_STR_MANUFACTURER,
  .iProduct           = USB_STR_PRODUCT,
  .iSerialNumber      = USB_STR_SERIAL_NUMBER,
  .bNumConfigurations = 1,
};

const alignas(4) usb_configuration_hierarchy_t usb_configuration_hierarchy =
{
  .configuration =
  {
    .bLength             = sizeof(usb_configuration_descriptor_t),
    .bDescriptorType     = USB_CONFIGURATION_DESCRIPTOR,
    .wTotalLength        = sizeof(usb_configuration_hierarchy_t),
    .bNumInterfaces      = 2,
    .bConfigurationValue = 1,
    .iConfiguration      = 0,
    .bmAttributes        = 0x80,
    .bMaxPower           = 50, // 100 mA
  },

  .interface_comm =
  {
    .bLength             = sizeof(usb_interface_descriptor_t),
    .bDescriptorType     = USB_INTERFACE_DESCRIPTOR,
    .bInterfaceNumber    = 0,
    .bAlternateSetting   = 0,
    .bNumEndpoints       = 1,
    .bInterfaceClass     = USB_CDC_COMM_CLASS,
    .bInterfaceSubClass  = USB_CDC_ACM_SUBCLASS,
    .bInterfaceProtocol  = 0,
    .iInterface          = 0,
  },

  .cdc_header =
  {
    .bFunctionalLength   = sizeof(usb_cdc_header_functional_descriptor_t),
    .bDescriptorType     = USB_CS_INTERFACE_DESCRIPTOR,
    .bDescriptorSubtype  = USB_CDC_HEADER_SUBTYPE,
    .bcdCDC              = 0x0110,
  },

  .cdc_acm =
  {
    .bFunctionalLength   = sizeof(usb_cdc_abstract_control_managment_descriptor_t),
    .bDescriptorType     = USB_CS_INTERFACE_DESCRIPTOR,
    .bDescriptorSubtype  = USB_CDC_ACM_SUBTYPE,
    .bmCapabilities      = USB_CDC_ACM_SUPPORT_LINE_REQUESTS,
  },

  .cdc_call_mgmt =
  {
    .bFunctionalLength   = sizeof(usb_cdc_call_managment_functional_descriptor_t),
    .bDescriptorType     = USB_CS_INTERFACE_DESCRIPTOR,
    .bDescriptorSubtype  = USB_CDC_CALL_MGMT_SUBTYPE,
    .bmCapabilities      = USB_CDC_CALL_MGMT_OVER_DCI,
    .bDataInterface      = 1,
  },

  .cdc_union =
  {
    .bFunctionalLength   = sizeof(usb_cdc_union_functional_descriptor_t),
    .bDescriptorType     = USB_CS_INTERFACE_DESCRIPTOR,
    .bDescriptorSubtype  = USB_CDC_UNION_SUBTYPE,
    .bMasterInterface    = 0,
    .bSlaveInterface0    = 1,
  },

  .ep_comm =
  {
    .bLength             = sizeof(usb_endpoint_descriptor_t),
    .bDescriptorType     = USB_ENDPOINT_DESCRIPTOR,
    .bEndpointAddress    = USB_IN_ENDPOINT | USB_CDC_EP_COMM,
    .bmAttributes        = USB_INTERRUPT_ENDPOINT,
    .wMaxPacketSize      = 64,
    .bInterval           = 1,
  },

  .interface_data =
  {
    .bLength             = sizeof(usb_interface_descriptor_t),
    .bDescriptorType     = USB_INTERFACE_DESCRIPTOR,
    .bInterfaceNumber    = 1,
    .bAlternateSetting   = 0,
    .bNumEndpoints       = 2,
    .bInterfaceClass     = USB_CDC_DATA_CLASS,
    .bInterfaceSubClass  = 0,
    .bInterfaceProtocol  = 0,
    .iInterface          = 0,
  },

  .ep_in =
  {
    .bLength             = sizeof(usb_endpoint_descriptor_t),
    .bDescriptorType     = USB_ENDPOINT_DESCRIPTOR,
    .bEndpointAddress    = USB_IN_ENDPOINT | USB_CDC_EP_SEND,
    .bmAttributes        = USB_BULK_ENDPOINT,
    .wMaxPacketSize      = 64,
    .bInterval           = 0,
  },

  .ep_out =
  {
    .bLength             = sizeof(usb_endpoint_descriptor_t),
    .bDescriptorType     = USB_ENDPOINT_DESCRIPTOR,
    .bEndpointAddress    = USB_OUT_ENDPOINT | USB_CDC_EP_RECV,
    .bmAttributes        = USB_BULK_ENDPOINT,
    .wMaxPacketSize      = 64,
    .bInterval           = 0,
  },
};

const alignas(4) usb_string_descriptor_zero_t usb_string_descriptor_zero =
{
  .bLength               = sizeof(usb_string_descriptor_zero_t),
  .bDescriptorType       = USB_STRING_DESCRIPTOR,
  .wLANGID               = 0x0409, // English (United States)
};

char usb_serial_number[16];

const char *usb_strings[] =
{
  [USB_STR_MANUFACTURER]  = "LANE Boys RC",
  [USB_STR_PRODUCT]       = "Virtual COM-Port",
  [USB_STR_SERIAL_NUMBER] = usb_serial_number,
};

alignas(4) uint8_t usb_string_descriptor_buffer[64];

