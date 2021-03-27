#pragma once
#include <stdint.h>
#include <stdbool.h>


// Endpoint data buffer definition
#define MAX_PACKET_SIZE (64)

// Interface numbers
#define USB_INTERFACE_TEST (0)
#define USB_INTERFACE_DFU (1)

// String descriptor indices
#define USB_STRING_LANGUAGE (0)
#define USB_STRING_MANUFACTURER (1)
#define USB_STRING_PRODUCT (2)
#define USB_STRING_SERIAL (3)
#define USB_STRING_DFU (4)
#define USB_STRING_TEST (5)

// Vendor specific request codes
#define VENDOR_CODE_COMMAND (72)    // Light Controller programmer command interface
#define VENDOR_CODE_MS (42)         // Retrieve the Microsoft OS 2.0 descriptor
#define WINUSB_REQUEST_DESCRIPTOR (7)
#define VENDOR_CODE_WEBUSB (69)     // Retrieve WebUSB landing page URL
#define WEBUSB_REQUEST_GET_URL (2)

// String descriptor indices
#define USB_STRING_LANGUAGE (0)
#define USB_STRING_MANUFACTURER (1)
#define USB_STRING_PRODUCT (2)
#define USB_STRING_SERIAL (3)
#define USB_STRING_INTERFACE (4)

// Descriptor types
#define USB_DTYPE_DEVICE (1)
#define USB_DTYPE_CONFIGURATION (2)
#define USB_DTYPE_STRING (3)
#define USB_DTYPE_INTERFACE (4)
#define USB_DTYPE_ENDPOINT (5)
#define USB_DTYPE_IAD (11)
#define USB_DTYPE_BOS (15)
#define USB_DTYPE_DFU (33)

// Request types
#define USB_REQTYPE_DIRECTION_MASK (0x80)
#define USB_REQTYPE_TYPE_MASK (0x60)
#define USB_REQTYPE_RECIPIENT_MASK (0x1f)
#define USB_REQTYPE_STANDARD (0 << 5)
#define USB_REQTYPE_CLASS (1 << 5)
#define USB_REQTYPE_VENDOR (2 << 5)
#define USB_RECIPIENT_DEVICE (0 << 0)
#define USB_RECIPIENT_INTERFACE (1 << 0)
#define USB_RECIPIENT_ENDPOINT (2 << 0)
#define USB_RECIPIENT_OTHER (3 << 0)

// USB Standard requests
#define USB_REQ_GET_STATUS (0)
#define USB_REQ_CLEAR_FEATURE (1)
#define USB_REQ_SET_FEATURE (3)
#define USB_REQ_SET_ADDRESS (5)
#define USB_REQ_GET_DESCRIPTOR (6)
#define USB_REQ_SET_DESCRIPTOR (7)
#define USB_REQ_GET_CONFIGURATION (8)
#define USB_REQ_SET_CONFIGURATION (9)
#define USB_REQ_GET_INTERFACE (10)
#define USB_REQ_SET_INTERFACE (11)
#define USB_REQ_SYNC_FRAME (12)

// DFU API
#define DFU_DETACH (0)
#define DFU_DNLOAD (1)
#define DFU_UPLOAD (2)
#define DFU_GETSTATUS (3)
#define DFU_CLRSTATUS (4)
#define DFU_GETSTATE (5)
#define DFU_ABORT (6)


// USB descriptors
extern __code uint8_t device_descriptor[];
extern __code uint16_t device_descriptor_length;

extern __code uint8_t configuration_descriptor[];
extern __code uint16_t configuration_descriptor_length;

extern __code uint8_t language_descriptor[];
extern __code uint16_t language_descriptor_length;

extern __code uint8_t manufacturer_string_descriptor[];
extern __code uint16_t manufacturer_string_descriptor_length;

extern __code uint8_t product_string_descriptor[];
extern __code uint16_t product_string_descriptor_length;

extern __code uint8_t dfu_string_descriptor[];
extern __code uint16_t dfu_string_descriptor_length;

extern __code uint8_t test_string_descriptor[];
extern __code uint16_t test_string_descriptor_length;

extern __code uint8_t bos_descriptor[];
extern __code uint16_t bos_descriptor_length;

extern __code uint8_t ms_os_20_descriptor[];
extern __code uint16_t ms_os_20_descriptor_length;

extern __code uint8_t landing_page_descriptor[];
extern __code uint16_t landing_page_descriptor_length;

// Currently active configuration
extern uint8_t active_usb_configuration;

// USB API
extern void USB_init(void);
extern void USB_handle_events(void);
extern void USB_EP1_ack(void);
extern void USB_EP2_send(uint8_t);

extern __xdata __at (EP0_ADDR) uint8_t ep0_buffer[EP0_SIZE];
extern __xdata __at (EP1_ADDR) uint8_t ep1_buffer[EP1_SIZE];
extern __xdata __at (EP2_ADDR) uint8_t ep2_buffer[EP2_SIZE];

// WebUSB programmer API
extern bool COMMAND_handler(uint8_t value);
extern void BOOTLOADER_start(void);
extern void DATA_sent(void);
extern void DATA_received(uint8_t);
