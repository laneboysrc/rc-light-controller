'use strict';

const MAX_UART_LOG_LINES = 20;

const MENU_CONNECTION = 0;
const MENU_PROGRAMMING = 1;

let firmware_image;
let log_stdin_running = false;
let programming_active = false;
let is_connected = false;
let has_webserial = false;
let last_programming_failed = false;
let programmer;

const el = {};

el.menu_buttons = document.querySelectorAll('nav button');

el.connect_button = document.querySelector("#connect_button");
el.disconnect_button = document.querySelector("#disconnect_button");
el.connection_info = document.querySelector("#connection_info");
el.load = document.querySelector('#load');
el.load_input = document.querySelector('#load-input');
el.program = document.querySelector('#program');
el.progress = document.querySelector('#progress');
el.filename = document.querySelector('#filename');
el.status = document.querySelector('#status');


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
  if (!has_webserial) {
    hide(document.querySelector('.body'));
    return;
  }

  if (!is_connected) {
    hide(el.connection_info);
    hide(el.disconnect_button);
    show(el.connect_button);
    disable(el.menu_buttons[MENU_PROGRAMMING]);
    return;
  }

  show(el.connection_info);
  show(el.disconnect_button);
  hide(el.connect_button);

  if (programming_active) {
    disable(el.load);
    disable(el.menu_buttons[MENU_CONNECTION]);
  }
  else {
    enable(el.load);
    enable(el.menu_buttons[MENU_CONNECTION]);
    enable(el.menu_buttons[MENU_PROGRAMMING]);
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
    el.connection_info.textContent = 'Connected to USB-to-serial adapter ' + device_serial;
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
  console.log('load_file_from_disk');

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

    // Reset the input element, because otherwise it would not trigger
    // if the user tries to load the same configuration file again!
    el.load_input.value = '';

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

    await isp.initialization_sequence();

    const part_id = await isp.read_part_id();
    log("MCU part number: " + part_id.part_name);

    const flash_size = await isp.get_flash_size();
    log("Flash size: " + flash_size / 1024 + " Kbytes");
    if (firmware_image.length > flash_size) {
      log('Firmware size (' + firmware_image.length + ') exceeds flash size (' + flash_size + ')', 'fail');
      throw 'Firmware too large';
    }

    await isp.program(firmware_image);

    // We let the downloaded isp_program run for 200 ms. This causes the voltage to
    // drop off sharply after we switch it off. If we don't do this then
    // the capacitor on the light controller stays at a low voltage, causing
    // the MCU to not properly reset (most likely because the ISP isp_program does
    // not use brown-out detection and maybe sleeps the CPU?)
    log("Resetting the light controller ...");
    await isp.reset_mcu();
    await delay(200);
    log("SUCCESS: programming complete", "success");
  }
  catch(e) {
    console.error(e);
    progressCallback(0);
    last_programming_failed = true;
    log("ERROR: programming failed", "fail");
  }
  finally {
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

  update_ui();
};

async function connect(device) {
  await programmer.open(device);
  if (programmer.is_open) {
    const msg = 'Connected to USB-to-serial adapter ' + programmer.serial_number;
    console.log(msg);
    update_programmer_connection(programmer.serial_number);
  }
}

async function disconnect() {
  await programmer.close();
  update_programmer_connection();
}

async function onDeviceConnected(device) {
  connect(device);
}

async function onDeviceDisconnected() {
  update_programmer_connection();
}

async function init() {
  if (window.location.protocol != 'https:' && window.location.protocol != 'file:') {
    if (window.location.protocol != 'http:' || window.location.hostname != 'localhost') {
      show(document.querySelector('#error_https'));
      show(document.querySelector('#error'));
    }
  }

  has_webserial = true;
  if (!("serial" in navigator)) {
    show(document.querySelector('#error_webserial'));
    show(document.querySelector('#error'));
    has_webserial = false;
  }

  if (has_webserial) {
    programmer = new WebSerial_programmer();
    programmer.onConnectedCallback = onDeviceConnected;
    programmer.onDisconnectedCallback = onDeviceDisconnected;
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
      console.log('Paired USB-to-serial devices: ', available_devices);
      connect(available_devices[0]);
    }
  }
}

init();

