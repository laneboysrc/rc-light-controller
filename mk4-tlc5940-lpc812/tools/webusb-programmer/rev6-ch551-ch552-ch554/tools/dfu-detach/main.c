#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

#define DFU_DETACH (0)
#define USB_INTERFACE_DFU (1)
#define USB_REQTYPE_STANDARD (0 << 5)
#define USB_RECIPIENT_INTERFACE (1 << 0)

libusb_device_handle *device_handle;

uint16_t vid = 0x6666;
uint16_t pid = 0xcab7;

int main(int argc, char const *argv[])
{
	int ret = 0;

	printf("Detach the CH552-based WebUSB programmer for flashing\n\n");

	libusb_init(NULL);
	libusb_set_debug(NULL, 3);

	device_handle = libusb_open_device_with_vid_pid(NULL, vid, pid);

	if (device_handle == NULL) {
		printf("Error: Device 0x%04x:0x%04x device not found\n", vid, pid);
		// Exit SUCCESS in case we are already in the bootloader
		return 0;
	}

	// Send the DFU detach command
   	int r = libusb_control_transfer(device_handle, USB_REQTYPE_STANDARD | USB_RECIPIENT_INTERFACE, DFU_DETACH, 0, USB_INTERFACE_DFU, NULL, 0, 0);
    if (r == LIBUSB_SUCCESS) {
        printf("Sucess booted into the bootloader\n");
    }
    else {
        printf("ERROR: %s\n", libusb_error_name(r));
        ret = 1;
    }

	if (device_handle) {
		libusb_close(device_handle);
	}
	libusb_exit(NULL);

	return ret;
}
