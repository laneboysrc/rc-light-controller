#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <hal_usb.h>


#define CDC_DEVICE_CLASS 2                    // USB Communication Device Class
#define CDC_COMM_CLASS 2                      // CDC Communication Class Interface
#define CDC_DATA_CLASS 10                     // CDC Data Class Interface

#define CDC_DLCM_SUBCLASS 1                   // Direct Line Control Model
#define CDC_ACM_SUBCLASS 2                    // Abstract Control Model
#define CDC_TCM_SUBCLASS 3                    // Telephone Control Model
#define CDC_MCCM_SUBCLASS 4                   // Multi-Channel Control Model
#define CDC_CCM_SUBCLASS 5                    // CAPI Control Model
#define CDC_ETH_SUBCLASS 6                    // Ethernet Networking Control Model
#define CDC_ATM_SUBCLASS 7                    // ATM Networking Control Model

#define CDC_HEADER_SUBTYPE 0                  // Header Functional Descriptor
#define CDC_CALL_MGMT_SUBTYPE 1               // Call Management
#define CDC_ACM_SUBTYPE 2                     // Abstract Control Management
#define CDC_UNION_SUBTYPE 6                   // Union Functional Descriptor

#define CDC_CALL_MGMT_SUPPORTED 1
#define CDC_CALL_MGMT_OVER_DCI 2

#define CDC_ACM_SUPPORT_FEATURE_REQUESTS 1
#define CDC_ACM_SUPPORT_LINE_REQUESTS 2
#define CDC_ACM_SUPPORT_SENDBREAK_REQUESTS 4
#define CDC_ACM_SUPPORT_NOTIFY_REQUESTS 8


typedef struct __attribute__((packed)) {
  uint8_t bFunctionalLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint16_t bcdCDC;
} cdc_header_functional_descriptor_t;

typedef struct __attribute__((packed)) {
  uint8_t bFunctionalLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bmCapabilities;
} cdc_abstract_control_managment_descriptor_t;

typedef struct __attribute__((packed)) {
  uint8_t bFunctionalLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bmCapabilities;
  uint8_t bDataInterface;
} cdc_call_managment_functional_descriptor_t;

typedef struct __attribute__((packed)) {
  uint8_t bFunctionalLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bMasterInterface;
  uint8_t bSlaveInterface0;
} cdc_union_functional_descriptor_t;


void usb_cdc_init(void);
void usb_cdc_configuration_callback(uint8_t config);
bool usb_cdc_handle_class_request(usb_request_t *request);


