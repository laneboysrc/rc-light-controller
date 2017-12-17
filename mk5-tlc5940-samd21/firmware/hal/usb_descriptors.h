#pragma once

#include <usb_api.h>
#include <usb_cdc.h>

#define MAX_PACKET_SIZE_0 64

#define USB_STR_MANUFACTURER 1
#define USB_STR_PRODUCT 2
#define USB_STR_SERIAL_NUMBER 3
#define USB_STR_COUNT 4

#define USB_CDC_EP_SEND 1
#define USB_CDC_EP_RECV 2
#define USB_CDC_EP_COMM 3


typedef struct __attribute__((packed))
{
    usb_configuration_descriptor_t configuration;

    usb_interface_descriptor_t interface_comm;
    cdc_header_functional_descriptor_t cdc_header;
    cdc_abstract_control_managment_descriptor_t cdc_acm;
    cdc_call_managment_functional_descriptor_t cdc_call_mgmt;
    cdc_union_functional_descriptor_t cdc_union;
    usb_endpoint_descriptor_t ep_comm;

    usb_interface_descriptor_t interface_data;
    usb_endpoint_descriptor_t ep_in;
    usb_endpoint_descriptor_t ep_out;
} usb_configuration_hierarchy_t;


//-----------------------------------------------------------------------------
extern const usb_device_descriptor_t usb_device_descriptor;
extern const usb_configuration_hierarchy_t usb_configuration_hierarchy;
extern const usb_language_descriptor_t usb_language_descriptor;
extern const char * const usb_strings[];

extern char usb_serial_number[];


