'use strict';

const MAX_UART_LOG_LINES = 20;

const MENU_CONNECTION = 0;
const MENU_TESTING = 1;

let is_connected = false;
let has_webserial = false;
let testing_active = false;
let simulator_ui;
let simulator;
let uart;



const el = {};

el.menu_buttons = document.querySelectorAll('nav button');

el.connect_button = document.querySelector("#connect_button");
el.disconnect_button = document.querySelector("#disconnect_button");
el.connection_info = document.querySelector("#connection_info");


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
    disable(el.menu_buttons[MENU_TESTING]);
    return;
  }

  show(el.connection_info);
  show(el.disconnect_button);
  hide(el.connect_button);
  enable(el.menu_buttons[MENU_TESTING]);
}

async function update_programmer_connection(device_serial) {
  if (typeof device_serial === 'undefined') {
    is_connected = false;
    await select_page('tab_connection');
  }
  else {
    is_connected = true;
    el.connection_info.textContent = 'Connected to USB-to-serial adapter ' + device_serial;
    await select_page('tab_testing');
  }
  update_ui();
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

  if (selected_page == 'tab_connection') {
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

      simulator = new Simulator(uart);
      simulator_ui = new Simulator_ui(simulator);
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
  }
  update_ui();
};

async function connect(device) {
  await uart.open(device);
  if (uart.is_open) {
    const msg = 'Connected to USB-to-serial adapter ' + uart.serial_number;
    console.log(msg);
    update_programmer_connection(uart.serial_number);
  }
}

async function disconnect() {
  await uart.close();
  update_programmer_connection();
}

async function onDeviceConnected(device) {
  connect(device);
}

async function onDeviceDisconnected() {
  update_programmer_connection();
}

async function init() {
  if (window.location.protocol != 'https:'  &&  window.location.protocol != 'file:') {
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
    uart = new WebSerial_device();
    uart.onConnectedCallback = onDeviceConnected;
    uart.onDisconnectedCallback = onDeviceDisconnected;
  }

  el.connect_button.addEventListener('click', connect);
  el.disconnect_button.addEventListener('click', disconnect);

  for (let index = 0; index < el.menu_buttons.length; index += 1) {
    const button = el.menu_buttons[index];
    button.addEventListener('click', async (event) => {
      const selected_page = event.currentTarget.getAttribute('data');
      await select_page(selected_page);
    });
  }

  await select_page("tab_connection");
  update_ui();

  if (uart) {
    const available_devices = await uart.get_available_devices();
    if (available_devices.length) {
      console.log('Paired USB-to-serial devices: ', available_devices);
      connect(available_devices[0]);
    }
  }
}

init();

