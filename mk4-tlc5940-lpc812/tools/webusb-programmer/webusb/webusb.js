'use strict';

const VENDOR_CODE_COMMAND = 72;

const CMD_DUT_POWER_OFF = 10;
const CMD_DUT_POWER_ON = 11;
const CMD_OUT_ISP_LOW = 20;
const CMD_OUT_ISP_HIGH = 21;
const CMD_OUT_ISP_TRISTATE = 22;
const CMD_BAUDRATE_38400 = 30;
const CMD_BAUDRATE_115200 = 31;
const CMD_LED_OK_OFF = 40;
const CMD_LED_OK_ON = 41;
const CMD_LED_BUSY_OFF = 42;
const CMD_LED_BUSY_ON = 43;
const CMD_LED_ERROR_OFF = 44;
const CMD_LED_ERROR_ON = 45;

let device;

const connectButton = document.querySelector("#connect");
const el_dut_power = document.querySelector("#dut-power");
const el_led_ok = document.querySelector("#led-ok");
const el_led_busy = document.querySelector("#led-busy");
const el_led_error = document.querySelector("#led-error");
const statusDisplay = document.querySelector('#status');


function string2arraybuffer(str) {
  var buf = new ArrayBuffer(str.length);
  var bufView = new Uint8Array(buf);
  for (var i=0, strLen=str.length; i<strLen; i++) {
    bufView[i] = str.charCodeAt(i);
  }
  return buf;
}

async function send_set_command(cmd) {
  const setup = {
    "requestType": "vendor",
    "recipient": "device",
    "request": VENDOR_CODE_COMMAND,
    "value": cmd,
    "index": 0
  };

  let result = await device.controlTransferOut(setup);
  if (result.status != "ok") {
    console.log("send_set_command(" + cmd + ") FAIL: " + result.staus);
  }
  else {
    console.log("send_set_command(" + cmd + ") OK");
  }
}

async function connect() {
  console.log("connect()");
  console.log(device);
  try {
    await device.open();
  }
  catch (e) {
    console.log("device.open() failed: ", e);
    return;
  }

  if (device.configuration === null) {
    await device.selectConfiguration(1);
  }
  console.log("Device ready!");


  console.log("controlTransferOut() ...");
  await send_set_command(0);

  console.log("controlTransferIn() ...");
  const setup = {
    "requestType": "vendor",
    "recipient": "device",
    "request": 72,
    "value": 1,
    "index": 0
  };

  let controlIn = await device.controlTransferIn(setup, 1);
  console.log(controlIn);
  console.log(controlIn.data.getUint8(0));

  // await device.close();
}

 async function request_device() {
  const filters = [
    { 'vendorId': 0x6666 }
  ];

  try {
    device = await navigator.usb.requestDevice({ 'filters': filters });
  }
  catch (e) {
    console.log("requestDevice() failed: " + e);
  }

  if (device) {
    connect()
  }
}


function checkbox_handler() {
  if (this.checked) {
    send_set_command(this.CMD_ON);
  }
  else {
    send_set_command(this.CMD_OFF);
  }
}


async function init() {

  if (window.location.protocol != 'https:') {
    if (window.location.protocol != 'http:' || window.location.hostname != 'localhost') {
      document.querySelector('#protocol-error').classList.remove('hidden');
    }
  }

  connectButton.addEventListener('click', request_device);

  el_dut_power.CMD_ON = CMD_DUT_POWER_ON;
  el_dut_power.CMD_OFF = CMD_DUT_POWER_OFF;
  el_dut_power.addEventListener('change', checkbox_handler);

  el_led_ok.CMD_ON = CMD_LED_OK_ON;
  el_led_ok.CMD_OFF = CMD_LED_OK_OFF;
  el_led_ok.addEventListener('change', checkbox_handler);

  el_led_busy.CMD_ON = CMD_LED_BUSY_ON;
  el_led_busy.CMD_OFF = CMD_LED_BUSY_OFF;
  el_led_busy.addEventListener('change', checkbox_handler);

  el_led_error.CMD_ON = CMD_LED_ERROR_ON;
  el_led_error.CMD_OFF = CMD_LED_ERROR_OFF;
  el_led_error.addEventListener('change', checkbox_handler);


  let devices = await navigator.usb.getDevices();
  console.log('Paired USB devices: ', devices);
  device = devices[0];
  if (device) {
    connect()
  }
}

init();

