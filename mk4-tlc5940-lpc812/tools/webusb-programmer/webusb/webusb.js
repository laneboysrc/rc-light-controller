/*

API:
  get_available_devices()
  open(device)
  close()
  write()
  read()
  readline()
  flush()

  set connected_callback
  set disconnected_callback
  get is_open

*/
class WebUSB_programmer {
  VENDOR_ID = 0x6666;
  VENDOR_CODE_COMMAND = 72;

  TEST_INTERFACE = 0;
  TEST_EP_OUT = 1;
  TEST_EP_IN = 2;
  EP_SIZE = 64;

  constructor() {
    this.active_device = undefined;
    this.stdin_buffer = '';

    this.connectedCallback = null;
    this.disconnectedCallback = null;

    navigator.usb.addEventListener('connect', this._webusb_device_connected.bind(this));
    navigator.usb.addEventListener('disconnect', this._webusb_device_disconnected.bind(this));
  }

  _webusb_device_connected(connection_event) {
    const device = connection_event.device;
    console.log('USB device connected:', device);
    if (!this.active_device) {
      if (device && device.vendorId == this.VENDOR_ID) {
        if (this.connectedCallback) {
          this.connectedCallback(device);
        }
      }
    }
  }

  async _webusb_device_disconnected(connection_event) {
    console.log('USB device disconnected:', connection_event);
    const disconnected_device = connection_event.device;
    if (this.active_device &&  disconnected_device == this.active_device) {
      await this.close();
      if (this.disconnectedCallback) {
        this.disconnectedCallback();
      }
    }
  }

  _make_crlf_visible(string) {
    return string.replace(/\r/g, '\\r').replace(/\n/g, '\\n');
  }

  _string2arraybuffer(str) {
    var buf = new ArrayBuffer(str.length);
    var bufView = new Uint8Array(buf);
    for (var i=0, strLen=str.length; i<strLen; i++) {
      bufView[i] = str.charCodeAt(i);
    }
    return bufView;
  }

  async get_available_devices() {
    return await navigator.usb.getDevices();
  }

  async webusb_request_device() {
    let device;

    const filters = [
      { 'vendorId': this.VENDOR_ID }
    ];

    try {
      device = await navigator.usb.requestDevice({ 'filters': filters });
    }
    catch (e) {
      console.log("requestDevice() failed: " + e);
    }

    return device;
  }

  async open(device) {
    console.log("open()", device);

    // If no device given, open the first available.
    // If none available, show the request device dialog
    if (!device || !device,open) {
      const devices = await this.get_available_devices();
      if (devices.length) {
        device = devices[0];
      }
      else {
        device = await this.webusb_request_device();
      }
    }
    if (!device) {
      return;
    }


    try {
      await device.open();
      if (device.configuration === null) {
        await device.selectConfiguration(1);
      }

      await device.claimInterface(this.TEST_INTERFACE);
    }
    catch (e) {
      console.error('Failed to open the device', e);
      return;
    }

    this.active_device = device;
    this._background_receive();
  }

  async close() {
    if (this.active_device) {
      try {
        await this.active_device.close();
      }
      catch (e) {
        // console.info('close() exception:', e);
      }
      finally {
        this.active_device = undefined;
      }
    }
  }

  async _background_receive() {
    while (true) {
      try {
        let result = await this.active_device.transferIn(this.TEST_EP_IN, this.EP_SIZE);
        if (result.status == 'ok') {
          const decoder = new TextDecoder('utf-8');
          const value = decoder.decode(result.data);

          this.stdin_buffer += value;
          console.log('USB R: ' + this._make_crlf_visible(value));
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

  flush() {
    this.stdin_buffer = '';
  }

  readline(terminator, tries) {
    const self = this;

    return new Promise((resolve, reject) => {

      if (typeof tries === 'undefined') {
        tries = 10;
      }

      function check() {
        let pos = self.stdin_buffer.search(terminator);
        if (pos >= 0) {
          pos += terminator.length;
          const line = self.stdin_buffer.substring(0, pos);
          self.stdin_buffer = self.stdin_buffer.substring(pos);
          resolve(line);
          return;
        }

        tries -= 1;
        if (tries <= 0) {
          reject("Timeout in readline()");
          return;
        }
        setTimeout(check, 100);
      }
      check();
    });
  }

  async send_command(cmd) {
    if (!this.is_open) {
      return;
    }

    const setup = {
      "requestType": "vendor",
      "recipient": "device",
      "request": this.VENDOR_CODE_COMMAND,
      "value": cmd,
      "index": 0
    };

    let result = await this.active_device.controlTransferOut(setup);
    if (result.status != "ok") {
      console.log("send_command(" + cmd + ") FAIL: " + result.staus);
    }
    else {
      console.log("send_command(" + cmd + ") OK");
    }
  }

  async write(data) {
    const transfer_size = this.EP_SIZE;

    if (data instanceof Uint8Array) {
      console.log('USB W: <data len=' + data.length + '>');
    }
    else {
      console.log('USB W: ' + this._make_crlf_visible(data));
      data = this._string2arraybuffer(data);
    }

    while (data.length) {
      const bytes = data.slice(0, transfer_size);
      data = data.slice(transfer_size);
      try {
        const result = await this.active_device.transferOut(this.TEST_EP_OUT, bytes);
        if (result.status != 'ok') {
          console.error('transferOut() failed:', result.status);
        }
      }
      catch (e) {
        console.error('transferOut() exception:', e);
        return;
      }

      // Delay to allow the other side to process the data
      // Experimenation showed that when setting the timeout to 5 ms the transfer
      // rate went up significantly compared to 1 ms. This happened regardless
      // whether the endpoint interval was set to 5 or 1 ?!?
      await new Promise(resolve => {setTimeout(() => {resolve()}, 5)});
    }
  }

  set onConnectedCallback(fn) {
    this.connectedCallback = fn;
  }

  set onDisconnectedCallback(fn) {
    this.disconnectedCallback = fn;
  }

  get is_open() {
    return (typeof this.active_device !== 'undefined');
  }

  get serial_number() {
    if (this.is_open) {
      return this.active_device.serialNumber;
    }
    return '';
  }
}
