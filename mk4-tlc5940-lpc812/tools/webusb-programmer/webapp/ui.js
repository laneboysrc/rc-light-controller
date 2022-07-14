'use strict';

const MAX_UART_LOG_LINES = 20;

const CMD_DUT_POWER_OFF = 10;
const CMD_DUT_POWER_ON = 11;
const CMD_OUT_ISP_LOW = 20;
const CMD_OUT_ISP_HIGH = 21;
const CMD_OUT_ISP_TRISTATE = 22;
const CMD_CH3_LOW = 23;
const CMD_CH3_HIGH = 24;
const CMD_CH3_TRISTATE = 25;
const CMD_BAUDRATE_38400 = 30;
const CMD_BAUDRATE_115200 = 31;
const CMD_LED_OK_OFF = 40;
const CMD_LED_OK_ON = 41;
const CMD_LED_BUSY_OFF = 42;
const CMD_LED_BUSY_ON = 43;
const CMD_LED_ERROR_OFF = 44;
const CMD_LED_ERROR_ON = 45;
const CMD_PING = 99;

const MENU_CONNECTION = 0;
const MENU_PROGRAMMING = 1;
const MENU_TESTING = 2;

let firmware_image;
let log_stdin_running = false;
let programming_active = false;
let is_connected = false;
let has_webusb = false;
let last_programming_failed = false;
let programmer;
let simulator_ui;
let simulator;
let power_is_on = false;

let testing_active = false;

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
  if (!has_webusb) {
    hide(document.querySelector('.body'));
    return;
  }

  if (!is_connected) {
    hide(el.connection_info);
    hide(el.disconnect_button);
    show(el.connect_button);
    disable(el.menu_buttons[MENU_PROGRAMMING]);
    disable(el.menu_buttons[MENU_TESTING]);
    return;
  }

  show(el.connection_info);
  show(el.disconnect_button);
  hide(el.connect_button);

  if (programming_active) {
    disable(el.load);
    disable(el.menu_buttons[MENU_CONNECTION]);
    disable(el.menu_buttons[MENU_TESTING]);
    programmer.send_command(CMD_LED_OK_OFF);
    programmer.send_command(CMD_LED_BUSY_ON);
    programmer.send_command(CMD_LED_ERROR_OFF);
  }
  else if (testing_active) {
    programmer.send_command(CMD_LED_OK_OFF);
    programmer.send_command(CMD_LED_BUSY_ON);
    programmer.send_command(CMD_LED_ERROR_OFF);
  }
  else {
    enable(el.load);
    enable(el.menu_buttons[MENU_CONNECTION]);
    enable(el.menu_buttons[MENU_PROGRAMMING]);
    enable(el.menu_buttons[MENU_TESTING]);
    programmer.send_command(CMD_LED_BUSY_OFF);
    if (last_programming_failed) {
      programmer.send_command(CMD_LED_OK_OFF);
      programmer.send_command(CMD_LED_ERROR_ON);
    }
    else {
      programmer.send_command(CMD_LED_OK_ON);
      programmer.send_command(CMD_LED_ERROR_OFF);
    }
  }

  if (firmware_image && !programming_active) {
    enable(el.program);
  }
  else {
    disable(el.program);
  }
}

async function update_programmer_connection(device_serial) {
  if (typeof device_serial === 'undefined') {
    is_connected = false;
    await select_page('tab_connection');
  }
  else {
    is_connected = true;
    el.connection_info.textContent = 'Connected to Light Controller Programmer with serial number ' + device_serial;
    await select_page('tab_programming');
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
    el.program.focus();
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

  const isp = new flash_lpc8xx(programmer);
  isp.messageCallback = log;
  isp.progressCallback = progressCallback;

  try {
    programming_active = true;
    last_programming_failed = false;
    update_ui();
    progressCallback(0);

    log("Power-cycling the light controller ...");
    await programmer.send_command(CMD_DUT_POWER_OFF);
    await programmer.send_command(CMD_OUT_ISP_LOW);
    await delay(200);
    await programmer.send_command(CMD_DUT_POWER_ON);
    power_is_on = true;
    await delay(100);
    progressCallback(0.02);
    await isp.initialization_sequence();
    await programmer.send_command(CMD_OUT_ISP_TRISTATE);

    const flash_size = await isp.get_flash_size();
    log("Flash size: " + flash_size / 1024 + " Kbytes");
    if (firmware_image.length > flash_size) {
      log('Firmware size (' + firmware_image.length + ') exceeds flash size (' + flash_size + ')', 'fail');
      throw 'Firmware too large';
    }

    await isp.program(firmware_image);

    // UPDATE: the code below is not needed anymore.
    // Instead we added a MOSFET that pulls the supply voltage of the light
    // controller low when it is powered off, bleeding off any voltage
    // stored in the light controller.

    // We let the downloaded isp_program run for 200 ms. This causes the voltage to
    // drop off sharply after we switch it off. If we don't do this then
    // the capacitor on the light controller stays at a low voltage, causing
    // the MCU to not properly reset (most likely because the ISP isp_program does
    // not use brown-out detection and maybe sleeps the CPU?)
    // await programmer.reset_mcu();
    // await delay(200);
    log("SUCCESS: programming complete", "success");
  }
  catch(e) {
    console.error(e);
    progressCallback(0);
    last_programming_failed = true;
    log("ERROR: programming failed", "fail");
  }
  finally {
    await programmer.send_command(CMD_DUT_POWER_OFF);
    await programmer.send_command(CMD_OUT_ISP_LOW);
    power_is_on = false;
    // Wait 500 ms for the voltage to bled fully, otherwise when quickly
    // switching to Pre-Processor simulator mode the MCU would still be in ISP
    // state.
    await delay(500);
    programming_active = false;
    update_ui();
    el.program.focus();
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

async function select_page(selected_page) {
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

  if (selected_page == 'tab_programming') {
    if (firmware_image) {
      el.program.focus();
    }
    else {
      el.load.focus();
    }
  }
  else if (selected_page == 'tab_connection') {
    if (is_connected) {
      el.disconnect_button.focus();
    }
    else {
      el.connect_button.focus();
    }
  }

  if (selected_page == 'tab_testing') {
    if (!testing_active) {
      testing_active = true;
      last_programming_failed = false;
      if (programmer) {
        await programmer.send_command(CMD_OUT_ISP_TRISTATE);
        await programmer.send_command(CMD_CH3_TRISTATE);
        await programmer.send_command(CMD_DUT_POWER_ON);
        power_is_on = true;
      }

      simulator = new Preprocessor_simulator(programmer);
      simulator_ui = new Preprocessor_simulator_ui(simulator);
    }
  }
  else {
    testing_active = false;
    if (simulator) {
      simulator.close();
      simulator_ui.close();
    }
    simulator = undefined;
    simulator_ui = undefined;

    if (programmer && power_is_on) {
      await programmer.send_command(CMD_DUT_POWER_OFF);
      await programmer.send_command(CMD_OUT_ISP_LOW);
      await programmer.send_command(CMD_CH3_LOW);
      power_is_on = false;
    }
  }

  update_ui();
};

async function connect(device) {
  await programmer.open(device);
  if (programmer.is_open) {
    await programmer.send_command(CMD_DUT_POWER_OFF);
    await programmer.send_command(CMD_OUT_ISP_LOW);
    await programmer.send_command(CMD_CH3_LOW);
    power_is_on = false;

    const msg = 'Connected to Light Controller Programmer with serial number ' + programmer.serial_number;
    console.log(msg);
    update_programmer_connection(programmer.serial_number);
    keep_programmer_alive();
  }
}

async function keep_programmer_alive() {
  // Send a PING command to the programmer every one second.
  // The programmer uses this command to see whether the host is still connected
  // to it, as the USB stack does not provide any info when e.g. when the webpage
  // with the programmer has been closed.
  try {
    await programmer.send_command(CMD_PING);
  }
  catch (e) {
    return;
  }
  setTimeout(keep_programmer_alive, 1000);
}

async function disconnect() {
  await programmer.close();
  update_programmer_connection();
}

async function onWebusbDeviceConnected(device) {
  connect(device);
}

async function onWebusbDeviceDisconnected() {
  update_programmer_connection();
}

async function init() {
  if (window.location.protocol != 'https:' && window.location.protocol != 'file:') {
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

  if (has_webusb) {
    programmer = new WebUSB_programmer();
    programmer.onConnectedCallback = onWebusbDeviceConnected;
    programmer.onDisconnectedCallback = onWebusbDeviceDisconnected;
  }

  el.connect_button.addEventListener('click', connect);
  el.disconnect_button.addEventListener('click', disconnect);

  el.load.addEventListener('click', () => {el.load_input.click(); }, false);
  el.load_input.addEventListener('change', load_file_from_disk, false);

  el.program.addEventListener('click', () => { program() }, false);


  for (let index = 0; index < el.menu_buttons.length; index += 1) {
    const button = el.menu_buttons[index];
    button.addEventListener('click', async (event) => {
      const selected_page = event.currentTarget.getAttribute('data');
      await select_page(selected_page);
    });
  }

  await select_page("tab_connection");
  update_ui();

  if (programmer) {
    const available_devices = await programmer.get_available_devices();
    if (available_devices.length) {
      console.log('Paired USB devices: ', available_devices);
      connect(available_devices[0]);
    }
  }
}

init();

