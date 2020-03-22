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

const MENU_CONNECTION = 0;
const MENU_PROGRAMMING = 1;
const MENU_TESTING = 2;

let firmware_image;
let log_stdin_running = false;
let programming_active = false;
let is_connected = false;
let has_webusb = false;
let last_programming_failed = false;

const el = {};

el.menu_buttons = document.querySelectorAll('nav button');

el.connect_button = document.querySelector("#webusb_connect_button");
el.disconnect_button = document.querySelector("#webusb_disconnect_button");
el.connection_info = document.querySelector("#connection_info");
el.send_button = document.querySelector("#send");
el.send_text =  document.querySelector("#send-text");
el.dut_power = document.querySelector("#dut-power");
el.led_ok = document.querySelector("#led-ok");
el.led_busy = document.querySelector("#led-busy");
el.led_error = document.querySelector("#led-error");
el.status = document.querySelector('#status');
el.isp = document.querySelector('#isp');
el.send_questionmark_button = document.querySelector('#send-questionmark');
el.send_synchronized_button = document.querySelector('#send-synchronized');
el.send_crystal_button = document.querySelector('#send-crystal');
el.send_a0_button = document.querySelector('#send-a0');
el.send_unlock_button = document.querySelector('#send-unlock');
el.send_prepare_button = document.querySelector('#send-prepare');
el.send_erase_button = document.querySelector('#send-erase');
el.load = document.querySelector('#load');
el.load_input = document.querySelector('#load-input');
el.program = document.querySelector('#program');
el.progress = document.querySelector('#progress');
el.initialize = document.querySelector('#initialize');
el.filename = document.querySelector('#filename');

function log(msg, displayClass) {
  const content = new Date(Date.now()).toISOString() + ' ' + msg;
  const div = document.createElement('div');
  div.textContent = content;
  console.log("log: " + content);

  if (displayClass) {
    div.classList.add(displayClass);
  }

  if (el.status.firstChild) {
    el.status.insertBefore(div, el.status.firstChild);
    while (el.status.childElementCount > MAX_UART_LOG_LINES) {
      el.status.removeChild(el.status.lastChild);
    }
  }
  else {
    el.status.appendChild(div);
  }
}

function show(element) {
  element.classList.remove('hidden');
}

function hide(element) {
  element.classList.add('hidden');
}

function enable(element) {
  element.disabled = false;
}

function disable(element) {
  element.disabled = true;
}

function update_ui() {
  if (!is_connected) {
    hide(el.connection_info);
    hide(el.disconnect_button);
    show(el.connect_button);
    disable(el.menu_buttons[MENU_PROGRAMMING]);
    disable(el.menu_buttons[MENU_TESTING]);
  }
  else {
    show(el.connection_info);
    show(el.disconnect_button);
    hide(el.connect_button);
    enable(el.menu_buttons[MENU_PROGRAMMING]);
    enable(el.menu_buttons[MENU_TESTING]);
  }

  if (programming_active) {
    disable(el.load);
    disable(el.menu_buttons[MENU_CONNECTION]);
    send_set_command(CMD_LED_OK_OFF);
    send_set_command(CMD_LED_BUSY_ON);
    send_set_command(CMD_LED_ERROR_OFF);
  }
  else {
    enable(el.load);
    enable(el.menu_buttons[MENU_CONNECTION]);
    send_set_command(CMD_LED_BUSY_OFF);
    if (last_programming_failed) {
      send_set_command(CMD_LED_OK_OFF);
      send_set_command(CMD_LED_ERROR_ON);
    }
    else {
      send_set_command(CMD_LED_OK_ON);
      send_set_command(CMD_LED_ERROR_OFF);
    }
  }

  if (firmware_image && !programming_active) {
    enable(el.program);
  }
  else {
    disable(el.program);
  }

  if (!has_webusb) {
    hide(document.querySelector('.body'));
  }
}

function update_programmer_connection(device_serial) {
  if (typeof device_serial === 'undefined') {
    is_connected = false;
    select_page('tab_connection');
  }
  else {
    is_connected = true;
    el.connection_info.textContent = 'Connected to Light Controller Programmer with serial number ' + device_serial;
    select_page('tab_programming');
  }
  last_programming_failed = false;
  update_ui();
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
    update_ui();
  };

  const fileobject = this.files[0];
  firmware_image = undefined;
  el.filename.textContent = fileobject.name;
  reader.readAsArrayBuffer(fileobject);
}

async function program() {
  if (!firmware_image) {
    log('Please load a firmware image', 'fail');
    return;
  }

  try {
    programming_active = true;
    last_programming_failed = false;
    update_ui();
    progressCallback(0);
    await isp_initialization_sequence();
    await isp_program(firmware_image);

    // UPDATE: the code below is not needed anymore.
    // Instead we added a MOSFET that pulls the supply voltage of the light
    // controller low when it is powered off, bleeding off any voltage
    // stored in the light controller.

    // We let the downloaded isp_program run for 200 ms. This causes the voltage to
    // drop off sharply after we switch it off. If we don't do this then
    // the capacitor on the light controller stays at a low voltage, causing
    // the MCU to not properly reset (most likely because the ISP isp_program does
    // not use brown-out detection and maybe sleeps the CPU?)
    // await isp_reset_mcu();
    // await delay(200);
    log("SUCCESS: programming complete", "success");
  }
  catch(e) {
    progressCallback(0);
    last_programming_failed = true;
    log("ERROR: programming failed", "fail");
  }
  finally {
    programming_active = false;
    update_ui();
    await send_set_command(CMD_DUT_POWER_OFF);
    await send_set_command(CMD_OUT_ISP_LOW);
  }
}

function send_textfield_to_programmer() {
  send_isp(el.send_text.value + '\r\n');
}

function progressCallback(progress) {
  el.progress.value = progress * 100;
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

function select_page(selected_page) {
  for (let index = 0; index < el.menu_buttons.length; index += 1) {
    let button = el.menu_buttons[index];
    let page_name = button.getAttribute('data');
    let page = document.querySelector('#' + page_name);
    if (page) {
      if (page_name == selected_page) {
          page.classList.remove('hidden');
          button.classList.add('selected');
      }
      else {
          page.classList.add('hidden');
          button.classList.remove('selected');
      }
    }
  }
};

function init() {
  if (window.location.protocol != 'https:') {
    if (window.location.protocol != 'http:' || window.location.hostname != 'localhost') {
      show(document.querySelector('#error_https'));
      show(document.querySelector('#error'));
    }
  }

  has_webusb = true;
  if (!navigator || !navigator.usb) {
      show(document.querySelector('#error_webusb'));
      show(document.querySelector('#error'));
    has_webusb = false;
  }


  el.connect_button.addEventListener('click', webusb_request_device);
  el.disconnect_button.addEventListener('click', webusb_disconnect);

  // el.send_button.addEventListener('click', send_textfield_to_programmer);
  // el.send_questionmark_button.addEventListener('click', () => { send_isp('?') });
  // el.send_synchronized_button.addEventListener('click', () => { send_isp('Synchronized\r\n') });
  // el.send_crystal_button.addEventListener('click', () => { send_isp('12000\r\n') });
  // el.send_a0_button.addEventListener('click', () => { send_isp('A 0\r\n') });
  // el.send_unlock_button.addEventListener('click', () => { send_isp('U 23130\r\n') });
  // el.send_prepare_button.addEventListener('click', () => { send_isp('P 0 15\r\n') });
  // el.send_erase_button.addEventListener('click', () => { send_isp('E 0 15\r\n') });
  // el.initialize.addEventListener('click', () => { isp_initialization_sequence(); }, false);

  // el.dut_power.CMD_ON = CMD_DUT_POWER_ON;
  // el.dut_power.CMD_OFF = CMD_DUT_POWER_OFF;
  // el.dut_power.addEventListener('change', checkbox_handler);

  // el.isp.CMD_ON = CMD_OUT_ISP_LOW;
  // el.isp.CMD_OFF = CMD_OUT_ISP_TRISTATE;
  // el.isp.addEventListener('change', checkbox_handler);

  // el.led_ok.CMD_ON = CMD_LED_OK_ON;
  // el.led_ok.CMD_OFF = CMD_LED_OK_OFF;
  // el.led_ok.addEventListener('change', checkbox_handler);

  // el.led_busy.CMD_ON = CMD_LED_BUSY_ON;
  // el.led_busy.CMD_OFF = CMD_LED_BUSY_OFF;
  // el.led_busy.addEventListener('change', checkbox_handler);

  // el.led_error.CMD_ON = CMD_LED_ERROR_ON;
  // el.led_error.CMD_OFF = CMD_LED_ERROR_OFF;
  // el.led_error.addEventListener('change', checkbox_handler);



  el.load.addEventListener('click', () => {el.load_input.click(); }, false);
  el.load_input.addEventListener('change', load_file_from_disk, false);

  el.program.addEventListener('click', () => { program() }, false);

  for (let index = 0; index < el.menu_buttons.length; index += 1) {
    const button = el.menu_buttons[index];
    button.addEventListener('click', (event) => {
      const selected_page = event.currentTarget.getAttribute('data');
      select_page(selected_page);
    });
  }

  select_page("tab_connection");
  update_ui();
  webusb_init();
}

init();

