'use strict';

const MAX_UART_LOG_LINES = 20;

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

let firmware_image;
let log_stdin_running = false;

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
const el_program = document.querySelector('#program');
const el_progress = document.querySelector('#progress');
const el_initialize = document.querySelector('#initialize');

function log(msg) {
  const content = new Date(Date.now()).toISOString() + ' ' + msg;
  const el = document.createElement('div');
  el.textContent = content;

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

function log_stdin_stop() {
  log_stdin_running = false;
}

async function log_stdin_start() {
  if (log_stdin_running) {
    console.warn('log_stdin_start() called while log_stdin already running!')
    return;
  }

  log_stdin_running = true;
  flush()
  while (log_stdin_running) {
    try {
      log(await readline('\n'));
    }
    catch (e) {
      ;
    }
  }
}

function load_file_from_disk() {
  firmware_image = undefined;

  if (this.files.length < 1) {
      return;
  }

  const reader = new FileReader();
  reader.onload = (e) => {
    const contents = e.target.result;

    let contentsString = String.fromCharCode.apply(null, new Uint8Array(contents));

    if (is_intel_hex(contentsString)) {
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

async function program() {
  if (!firmware_image) {
    log('Please load a firmware image');
    return;
  }

  try {
    progressCallback(0);
    await isp_initialization_sequence();
    await isp_program(firmware_image);

    // We let the downloaded isp_program run for 200 ms. This causes the voltage to
    // drop off sharply after we switch it off. If we don't do this then
    // the capacitor on the light controller stays at a low voltage, causing
    // the MCU to not properly reset (most likely because the ISP isp_program does
    // not use brown-out detection and maybe sleeps the CPU?)
    await isp_reset_mcu();
    await delay(200);
  }
  catch(e) {
    log(e);
    console.error(e);
  }
  finally {
    await send_set_command(CMD_DUT_POWER_OFF);
    await send_set_command(CMD_OUT_ISP_LOW);
  }
}

function send_textfield_to_programmer() {
  send_isp(el_send_text.value + '\r\n');
}

function progressCallback(progress) {
  el_progress.value = progress * 100;
  // log("Progress: " + Math.round(progress * 100));
}

function checkbox_handler() {
  if (this.checked) {
    send_set_command(this.CMD_ON);
  }
  else {
    send_set_command(this.CMD_OFF);
  }
}

function init() {
  if (window.location.protocol != 'https:') {
    if (window.location.protocol != 'http:' || window.location.hostname != 'localhost') {
      document.querySelector('#protocol-error').classList.remove('hidden');
    }
  }

  el_connect_button.addEventListener('click', webusb_request_device);
  el_send_button.addEventListener('click', send_textfield_to_programmer);
  el_send_questionmark_button.addEventListener('click', () => { send_isp('?') });
  el_send_synchronized_button.addEventListener('click', () => { send_isp('Synchronized\r\n') });
  el_send_crystal_button.addEventListener('click', () => { send_isp('12000\r\n') });
  el_send_a0_button.addEventListener('click', () => { send_isp('A 0\r\n') });
  el_send_unlock_button.addEventListener('click', () => { send_isp('U 23130\r\n') });
  el_send_prepare_button.addEventListener('click', () => { send_isp('P 0 15\r\n') });
  el_send_erase_button.addEventListener('click', () => { send_isp('E 0 15\r\n') });

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

  el_program.addEventListener('click', () => { program() }, false);
  el_initialize.addEventListener('click', () => { isp_initialization_sequence(); }, false);

  webusb_init();
}

init();

