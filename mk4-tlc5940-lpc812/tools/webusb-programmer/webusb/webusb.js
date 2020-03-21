'use strict';

const MAX_UART_LOG_LINES = 20;

const VENDOR_ID = 0x6666;
const VENDOR_CODE_COMMAND = 72;

const TEST_INTERFACE = 1;
const TEST_EP_IN = 1;
const TEST_EP_OUT = 2;
const EP_SIZE = 64;

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

let webusb_device;
let firmware_image;

const el_connect_button = document.querySelector("#connect");
const el_send_button = document.querySelector("#send");
const el_send_text =  document.querySelector("#send-text");
const el_dut_power = document.querySelector("#dut-power");
const el_led_ok = document.querySelector("#led-ok");
const el_led_busy = document.querySelector("#led-busy");
const el_led_error = document.querySelector("#led-error");
const el_status = document.querySelector('#status');
const el_isp = document.querySelector('#isp');
const el_send_questionmark_button = document.querySelector('#send-questionmark');
const el_send_synchronized_button = document.querySelector('#send-synchronized');
const el_send_crystal_button = document.querySelector('#send-crystal');
const el_send_a0_button = document.querySelector('#send-a0');
const el_send_unlock_button = document.querySelector('#send-unlock');
const el_send_prepare_button = document.querySelector('#send-prepare');
const el_send_erase_button = document.querySelector('#send-erase');
const el_load = document.querySelector('#load');
const el_load_input = document.querySelector('#load-input');


function string2arraybuffer(str) {
  var buf = new ArrayBuffer(str.length);
  var bufView = new Uint8Array(buf);
  for (var i=0, strLen=str.length; i<strLen; i++) {
    bufView[i] = str.charCodeAt(i);
  }
  return buf;
}

function checkbox_handler() {
  if (this.checked) {
    send_set_command(this.CMD_ON);
  }
  else {
    send_set_command(this.CMD_OFF);
  }
}

async function send_set_command(cmd) {
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

async function init_webusb() {
  navigator.usb.addEventListener('connect', webusb_device_connected);
  navigator.usb.addEventListener('disconnect', webusb_device_disconnected);

  let devices = await navigator.usb.getDevices();
  console.log('Paired USB devices: ', devices);
  const device = devices[0];
  if (device) {
    webusb_connect(device)
  }
}

async function request_device() {
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
  console.log('Connected to Light Controller Programmer with serial number ' + device.serialNumber);

  webusb_device = device;
  controlTransferTest();
  webusb_receive_data();
}

async function controlTransferTest() {
  console.log("controlTransferOut() ...");
  await send_set_command(0);

  console.log("controlTransferIn() ...");
  const setup = {
    "requestType": "vendor",
    "recipient": "device",
    "request": VENDOR_CODE_COMMAND,
    "value": 1,
    "index": 0
  };

  let controlIn = await webusb_device.controlTransferIn(setup, 1);
  console.log(controlIn);
  console.log(controlIn.data.getUint8(0));
}

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

        let msg = new Date(Date.now()).toISOString() + ' ' + value;

        console.log(msg);
        const el = document.createElement('div');
        el.textContent += msg;
        if (el_status.firstChild) {
          el_status.insertBefore(el, el_status.firstChild);
          while (el_status.childElementCount > MAX_UART_LOG_LINES) {
            el_status.removeChild(el_status.lastChild);
          }
        }
        else {
          el_status.appendChild(el);
        }
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

async function send(string) {
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

function send_textfield_to_programmer() {
  send(el_send_text.value + '\r\n');
}

function hex_to_bin (intel_hex_data) {
    return intel_hex.parse(intel_hex_data).data;
}

function load_file_from_disk() {
  firmware_image = undefined;

  if (this.files.length < 1) {
      return;
  }

  const intelHex = /^:[0-9a-fA-F][0-9a-fA-F]/;

  const reader = new FileReader();
  reader.onload = (e) => {
    const contents = e.target.result;

    let contentsString = String.fromCharCode.apply(null, new Uint8Array(contents));

    if (contentsString.match(intelHex)) {
      firmware_image = hex_to_bin(contentsString);
    }
    else {
      console.warn('Loading BINARY image');
      let data = new Uint8Array(8192 + contents.byteLength);
      data.fill(0xff);
      data.set(new Uint8Array(contents), 8192);
      firmware_image = data;
    }
    console.log('Firmware loaded; ' + firmware_image.length + ' Bytes');
  };
  reader.readAsArrayBuffer(this.files[0]);
}


function init() {
  if (window.location.protocol != 'https:') {
    if (window.location.protocol != 'http:' || window.location.hostname != 'localhost') {
      document.querySelector('#protocol-error').classList.remove('hidden');
    }
  }

  el_connect_button.addEventListener('click', request_device);
  el_send_button.addEventListener('click', send_textfield_to_programmer);
  el_send_questionmark_button.addEventListener('click', () => { send('?') });
  el_send_synchronized_button.addEventListener('click', () => { send('Synchronized\r\n') });
  el_send_crystal_button.addEventListener('click', () => { send('12000\r\n') });
  el_send_a0_button.addEventListener('click', () => { send('A 0\r\n') });
  el_send_unlock_button.addEventListener('click', () => { send('U 23130\r\n') });
  el_send_prepare_button.addEventListener('click', () => { send('P 0 15\r\n') });
  el_send_erase_button.addEventListener('click', () => { send('E 0 15\r\n') });

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

  el_isp.CMD_ON = CMD_OUT_ISP_LOW;
  el_isp.CMD_OFF = CMD_OUT_ISP_TRISTATE;
  el_isp.addEventListener('change', checkbox_handler);

  el_load.addEventListener('click', () => {el_load_input.click(); }, false);
  el_load_input.addEventListener('change', load_file_from_disk, false);

  init_webusb();
}

init();

