#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


#define USB_DEVICE_DESCRIPTOR 1
#define USB_CONFIGURATION_DESCRIPTOR 2
#define USB_STRING_DESCRIPTOR 3
#define USB_INTERFACE_DESCRIPTOR 4
#define USB_ENDPOINT_DESCRIPTOR 5
#define USB_DEVICE_QUALIFIER_DESCRIPTOR 6
#define USB_OTHER_SPEED_CONFIGURATION_DESCRIPTOR 7
#define USB_INTERFACE_POWER_DESCRIPTOR 8
#define USB_OTG_DESCRIPTOR 9
#define USB_DEBUG_DESCRIPTOR 10
#define USB_INTERFACE_ASSOCIATION_DESCRIPTOR 11
#define USB_BINARY_OBJECT_STORE_DESCRIPTOR 15
#define USB_DEVICE_CAPABILITY_DESCRIPTOR 16
#define USB_CS_INTERFACE_DESCRIPTOR 36

#define USB_DEVICE_RECIPIENT 0
#define USB_INTERFACE_RECIPIENT 1
#define USB_ENDPOINT_RECIPIENT 2
#define USB_OTHER_RECIPIENT 3

#define USB_STANDARD_REQUEST 0
#define USB_CLASS_REQUEST 1
#define USB_VENDOR_REQUEST 2

#define USB_OUT_TRANSFER 0
#define USB_IN_TRANSFER 1

#define USB_IN_ENDPOINT 0x80
#define USB_OUT_ENDPOINT 0x00
#define USB_INDEX_MASK 0x7f
#define USB_DIRECTION_MASK 0x80

#define USB_CONTROL_ENDPOINT (0 << 0)
#define USB_ISOCHRONOUS_ENDPOINT (1 << 0)
#define USB_BULK_ENDPOINT (2 << 0)
#define USB_INTERRUPT_ENDPOINT (3 << 0)
#define USB_NO_SYNCHRONIZATION (0 << 2)
#define USB_ASYNCHRONOUS (1 << 2)
#define USB_ADAPTIVE (2 << 2)
#define USB_SYNCHRONOUS (3 << 2)
#define USB_DATA_ENDPOINT (0 << 4)
#define USB_FEEDBACK_ENDPOINT (1 << 4)
#define USB_IMP_FB_DATA_ENDPOINT (2 << 4)

#define USB_GET_STATUS 0
#define USB_CLEAR_FEATURE 1
#define USB_SET_FEATURE 3
#define USB_SET_ADDRESS 5
#define USB_GET_DESCRIPTOR 6
#define USB_SET_DESCRIPTOR 7
#define USB_GET_CONFIGURATION 8
#define USB_SET_CONFIGURATION 9
#define USB_GET_INTERFACE 10
#define USB_SET_INTERFACE 11

#define USB_REQUEST_HOSTTODEVICE 0x00
#define USB_REQUEST_DEVICETOHOST 0x80
#define USB_REQUEST_DIRECTION 0x80
#define USB_REQUEST_STANDARD 0x00
#define USB_REQUEST_CLASS 0x20
#define USB_REQUEST_VENDOR 0x40
#define USB_REQUEST_TYPE 0x60
#define USB_REQUEST_DEVICE 0x00
#define USB_REQUEST_INTERFACE 0x01
#define USB_REQUEST_ENDPOINT 0x02
#define USB_REQUEST_OTHER 0x03
#define USB_REQUEST_RECIPIENT 0x1f


typedef struct __attribute__((packed)) {
  uint8_t bmRequestType;
  uint8_t bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
} usb_request_t;

typedef struct __attribute__((packed)) {
  uint8_t bLength;
  uint8_t bDescriptorType;
} usb_descriptor_header_t;

typedef struct __attribute__((packed)) {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t bcdUSB;
  uint8_t bDeviceClass;
  uint8_t bDeviceSubClass;
  uint8_t bDeviceProtocol;
  uint8_t bMaxPacketSize0;
  uint16_t idVendor;
  uint16_t idProduct;
  uint16_t bcdDevice;
  uint8_t iManufacturer;
  uint8_t iProduct;
  uint8_t iSerialNumber;
  uint8_t bNumConfigurations;
} usb_device_descriptor_t;

typedef struct __attribute__((packed)) {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t wTotalLength;
  uint8_t bNumInterfaces;
  uint8_t bConfigurationValue;
  uint8_t iConfiguration;
  uint8_t bmAttributes;
  uint8_t bMaxPower;
} usb_configuration_descriptor_t;

typedef struct __attribute__((packed)) {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bInterfaceNumber;
  uint8_t bAlternateSetting;
  uint8_t bNumEndpoints;
  uint8_t bInterfaceClass;
  uint8_t bInterfaceSubClass;
  uint8_t bInterfaceProtocol;
  uint8_t iInterface;
} usb_interface_descriptor_t;

typedef struct __attribute__((packed)) {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bEndpointAddress;
  uint8_t bmAttributes;
  uint16_t wMaxPacketSize;
  uint8_t bInterval;
} usb_endpoint_descriptor_t;

typedef struct __attribute__((packed)) {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t wLANGID;
} usb_language_descriptor_t;

typedef struct __attribute__((packed)) {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t bString;
} usb_string_descriptor_t;


typedef void (* usb_ep_callback_t)(size_t size);
typedef void (* usb_receive_callback_t)(uint8_t *data, size_t size);

extern void USB_init(void);
extern void USB_set_endpoint_callback(uint8_t ep, usb_ep_callback_t callback);
extern void USB_service(void);
extern void USB_send(uint8_t ep, uint8_t *data, size_t size);
extern void USB_receive(uint8_t ep, uint8_t *data, size_t size);
extern void USB_control_send(uint8_t *data, size_t size);
extern void USB_control_send_zlp(void);
extern void USB_control_receive(usb_receive_callback_t callback);
extern void USB_control_stall(void);
