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
class WebSerial_device {

  constructor() {
    this.active_device = undefined;
    this.stdin_buffer = '';
    this.serial = '';

    this.connectedCallback = null;
    this.disconnectedCallback = null;

    navigator.serial.addEventListener('disconnect', this._webserial_device_disconnected);
    navigator.serial.addEventListener('connect', this._webserial_device_connected);
  }

  _webserial_device_connected = (connection_event) => {
    console.log('test');
    const device = connection_event.device;
    console.log('UART device connected:', device);
    if (!this.active_device) {
      if (this.connectedCallback) {
        this.connectedCallback(device);
      }
    }
  }

  _webserial_device_disconnected = async (connection_event) => {
    console.log('UART device disconnected:', connection_event);
    const disconnected_device = connection_event.target;
    if (this.active_device &&  disconnected_device == this.active_device) {
      try {
        console.log('about to close')
        await this.close();
      }
      catch(e) {
        console.log(e);
      }
      if (this.disconnectedCallback) {
        this.disconnectedCallback();
      }
    }
  }

  _make_crlf_visible(string) {
    return string.replace(/\r/g, '\\r').replace(/\n/g, '\\n');
  }

  _to_hex(num, pad_length=2) {
    return num.toString(16).padStart(pad_length, '0');
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
    return await navigator.serial.getPorts();
  }

  async webserial_request_device() {
    let device;

    try {
      device = await navigator.serial.requestPort();
    }
    catch (e) {
      console.log("requestDevice() failed: " + e);
    }

    return device;
  }

  async open(device) {
    console.log("open()", device);

    // If none given, show the request device dialog
    if (!device || !device.open) {
      device = await this.webserial_request_device();
    }
    if (!device) {
      return;
    }

    try {
      await device.open({ baudRate: 100000, parity: 'even', dataBits: 8, stopBits: 2 });

      const info = await device.getInfo();
      this.serial = `${this._to_hex(info.usbVendorId)}:${this._to_hex(info.usbProductId)}`;
    }
    catch (e) {
      console.error('Failed to open the device', e);
      return;
    }

    this.active_device = device;
    this.keep_reading = true;
    this._background_receive();
  }

  async close() {
    console.log("close()");

    if (this.active_device) {
      try {
        this.keep_reading = false;
        if (this.reader) {
          this.reader.cancel();
        }
        await this.readableStreamClosed.catch(() => {}); // Ignore the error
        await this.active_device.close();
        console.log("close() successful");
      }
      catch (e) {
        console.info('close() exception:', e);
      }
      finally {
        this.active_device = undefined;
      }
    }
  }

  async _background_receive() {
    const decoder = new TextDecoderStream();
    this.readableStreamClosed = this.active_device.readable.pipeTo(decoder.writable);
    const inputStream = decoder.readable;

    while (this.active_device.readable && this.keep_reading) {
      this.reader = inputStream.getReader();
      try {
        while (true) {
          const { value, done } = await this.reader.read();
            if (done) {
              console.log('_background_receive: cancel() has been called');
              break;
            }

            this.stdin_buffer += value;
            console.log('UART R: ' + this._make_crlf_visible(value));
        }
      }
      catch (e) {
        console.info('_background_receive exception:', e);
      }
      finally {
        console.log('reader.releaseLock()');
        this.reader.releaseLock();
        this.reader = undefined;
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

    console.log("send_command(" + cmd + ") IGNORED");
  }

  async write(data, dont_log) {
    const transfer_size = this.EP_SIZE;

    if (!dont_log) {
      if (data instanceof Uint8Array) {
        console.log('UART W: <data len=' + data.length + '>');
      }
      else {
        console.log('UART W: ' + this._make_crlf_visible(data));
        data = this._string2arraybuffer(data);
      }
    }

    try {
      const writer = this.active_device.writable.getWriter();
      await writer.write(data);
      writer.releaseLock();
    }
    catch (e) {
      console.error('transferOut() exception:', e);
      return;
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
      return this.serial;
    }
    return '';
  }
}
