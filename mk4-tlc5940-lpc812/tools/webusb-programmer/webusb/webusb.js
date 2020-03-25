'use strict';


const VENDOR_ID = 0x6666;
const VENDOR_CODE_COMMAND = 72;

const TEST_INTERFACE = 0;
const TEST_EP_OUT = 1;
const TEST_EP_IN = 2;
const EP_SIZE = 64;

let webusb_device;


async function webusb_init() {
  navigator.usb.addEventListener('connect', webusb_device_connected);
  navigator.usb.addEventListener('disconnect', webusb_device_disconnected);

  let devices = await navigator.usb.getDevices();
  console.log('Paired USB devices: ', devices);
  const device = devices[0];
  if (device) {
    webusb_connect(device)
  }
}

async function webusb_request_device() {
  let device;

  const filters = [
    { 'vendorId': VENDOR_ID }
  ];

  try {
    device = await navigator.usb.requestDevice({ 'filters': filters });
  }
  catch (e) {
    console.log("requestDevice() failed: " + e);
  }

  if (device) {
    webusb_connect(device)
  }
}

async function webusb_connect(device) {
  console.log("webusb_connect()", device);

  try {
    await device.open();
    if (device.configuration === null) {
      await device.selectConfiguration(1);
    }

    await device.claimInterface(TEST_INTERFACE);
  }
  catch (e) {
    console.error('Failed to open the device', e);
    return;
  }

  webusb_device = device;
  // controlTransferTest();
  webusb_receive_data();

  await send_set_command(CMD_DUT_POWER_OFF);
  await send_set_command(CMD_OUT_ISP_LOW);

  const msg = 'Connected to Light Controller Programmer with serial number ' + device.serialNumber;
  console.log(msg);
  update_programmer_connection(device.serialNumber);
}

// async function controlTransferTest() {
//   console.log("controlTransferOut() ...");
//   await send_set_command(0);

//   console.log("controlTransferIn() ...");
//   const setup = {
//     "requestType": "vendor",
//     "recipient": "device",
//     "request": VENDOR_CODE_COMMAND,
//     "value": 1,
//     "index": 0
//   };

//   let controlIn = await webusb_device.controlTransferIn(setup, 1);
//   console.log(controlIn);
//   console.log(controlIn.data.getUint8(0));
// }

async function webusb_disconnect() {
  if (webusb_device) {
    try {
      await webusb_device.close();
    }
    catch (e) {
      // console.info('close() exception:', e);
    }
    finally {
      webusb_device = undefined;
      update_programmer_connection();
    }
  }
}

function webusb_device_connected(connection_event) {
    const device = connection_event.device;
    console.log('USB device connected:', device);
    if (!webusb_device) {
        if (device && device.vendorId == VENDOR_ID) {
            webusb_connect(device);
        }
    }
}

function webusb_device_disconnected(connection_event) {
    console.log('USB device disconnected:', connection_event);
    const disconnected_device = connection_event.device;
    if (webusb_device &&  disconnected_device == webusb_device) {
        webusb_disconnect();
    }
}

async function webusb_receive_data() {
  while (true) {
    try {
      let result = await webusb_device.transferIn(TEST_EP_IN, EP_SIZE);
      if (result.status == 'ok') {
        const decoder = new TextDecoder('utf-8');
        const value = decoder.decode(result.data);

        stdin_buffer += value;
        console.log('USB R: ' + make_crlf_visible(value));
      }
      else {
        console.info('transferIn() failed:', result.status);
      }
    }
    catch (e) {
      console.info('transferIn() exception:', e);
      return;
    }
  }
}

async function send_set_command(cmd) {
  if (typeof webusb_device === 'undefined') {
    return;
  }

  const setup = {
    "requestType": "vendor",
    "recipient": "device",
    "request": VENDOR_CODE_COMMAND,
    "value": cmd,
    "index": 0
  };

  let result = await webusb_device.controlTransferOut(setup);
  if (result.status != "ok") {
    console.log("send_set_command(" + cmd + ") FAIL: " + result.staus);
  }
  else {
    console.log("send_set_command(" + cmd + ") OK");
  }
}

async function send_isp_command(string) {
  flush();
  await send_isp(string + '\r\n');
  await expect('0\r\n');
}

async function send_isp(string) {
  console.log('USB W: ' + make_crlf_visible(string));

  try {
    const data = string2arraybuffer(string);

    const result = await webusb_device.transferOut(TEST_EP_OUT, data);
    if (result.status != 'ok') {
      console.error('transferOut() failed:', result.status);
    }
  }
  catch (e) {
    console.error('transferOut() exception:', e);
    return;
  }
}

async function send_isp_data(data) {
  const transfer_size = EP_SIZE;

  while (data.length) {
    const bytes = data.slice(0, transfer_size);
    data = data.slice(transfer_size);
    try {
      const result = await webusb_device.transferOut(TEST_EP_OUT, bytes);
      if (result.status != 'ok') {
        console.error('transferOut() failed:', result.status);
      }
    }
    catch (e) {
      console.error('transferOut() exception:', e);
      return;
    }

    // Delay to allow the UART to send_isp the bytes
    await delay(1);
  }
}
