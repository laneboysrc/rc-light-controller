/*

This is the mid-level interface to the light controller. It simulates
a Pre-Processor, where ST, TH and AUX data is sent over a UART to the light
controller.

It also reads diagnostics messages from the light controller to show them
to the user.

This module expects operating on a (virtual) UART that has already been opened
and configured with the right baudrate.
The uart object must support the following functions:
* uart.write(data)
* uart.readline(terminator, number_of_retries_before_timeout)

The preprocessor-simulator object provides the following methods:

  channel_changed(channel, value)
    channel: one of ST, TH, AUX, AUX2, AUX3, STARTUP_MODE, NO_SIGNAL and
             MULTI_AUX
             Together they determine the output of the Pre-Processor simulator
             as sent to the light controller
    value: the value to set <channel> to. For servo channels this is
           -100..0..+100. For STARTUP_MODE, NO_SIGNAL and MULTI_AUX it is
           0 (off) or 1 (on)`

  set onMessageCallback(fn)
    Set a callback function that will receive a string whenever a diagnostics
    line (terminated by \n) was read from the light controller.
    Note that also the CONFIG (which determines the detailed configuration the
    AUX channels in the light controller have been configured) are sent via
    this method.

  getAuxFunctionLabel(aux_function_number)
    Retrieve a human readable text for the given AUX function number.
    Example: For AUX_FUNCTION_MULTI_FUNCTION, which is 1, this function would
    return "Multi-function switch"

  get config_default
  get config_manual_3ch
  get config_manual_5ch
    Retrieve default AUX type and AUX function values for various use-cases.
    Useful to apply as UI defaults.

*/

class Preprocessor_simulator {
  ST = 'ST';
  TH = 'TH';
  AUX = 'AUX';
  AUX2 = 'AUX2';
  AUX3 = 'AUX3';
  STARTUP_MODE = 'STARTUP_MODE';
  NO_SIGNAL = 'NO_SIGNAL';
  MULTI_AUX = 'MULTI_AUX';

  AUX_TYPE = 'AUX_TYPE';
  AUX_TYPE_TWO_POSITION = 0;
  AUX_TYPE_TWO_POSITION_UP_DOWN = 1;
  AUX_TYPE_MOMENTARY = 2;
  AUX_TYPE_THREE_POSITION = 3;
  AUX_TYPE_ANALOG = 4;

  AUX_FUNCTION = 'AUX_FUNCTION';
  AUX_FUNCTION_NOT_USED = 0;
  AUX_FUNCTION_MULTI_FUNCTION = 1;
  AUX_FUNCTION_GEARBOX = 2;
  AUX_FUNCTION_WINCH = 3;
  AUX_FUNCTION_SERVO = 4;
  AUX_FUNCTION_INDICATORS = 5;
  AUX_FUNCTION_HAZARD = 6;
  AUX_FUNCTION_LIGHT_SWITCH = 7;

  CONFIG_TYPE = 'CONFIG_TYPE';
  CONFIG_TYPE_DEFAULT = 'DEFAULT';
  CONFIG_TYPE_AUTO = 'AUTO';
  CONFIG_TYPE_MANUAL = 'MANUAL';

  SLAVE_MAGIC_BYTE = 0x87;

  constructor(uart) {
    this.uart = uart;

    this.channels = {};
    this.channels[this.ST] = 0;
    this.channels[this.TH] = 0;
    this.channels[this.AUX] = 0;
    this.channels[this.AUX2] = 0;
    this.channels[this.AUX3] = 0;
    this.channels[this.STARTUP_MODE] = 0;
    this.channels[this.NO_SIGNAL] = 0;

    this.config = this.config_default;

    this.configChangedCallback = undefined;

    this.reader_active = true;
    this.transmitter_active = true;
    this._reader();
    this._transmitter();
  }

  close() {
    this.reader_active = false;
    this.transmitter_active = false;
  }

  async _reader() {
    console.log('Preprocessor_simulator.reader() start');
    this.uart.flush();

    while (this.reader_active) {
      let line = '';
      try {
        line = await this.uart.readline('\n', 0);
      }
      catch (e) {
        await new Promise(resolve => {setTimeout(() => {resolve()}, 100)});;
      }

      if (line) {
        if (line.startsWith('CONFIG')) {
          this._parse_config(line);
        }

        if (this.messageCallback) {
          this.messageCallback(line);
        }
      }
    }

    console.log('Preprocessor_simulator.reader() end');
  }

  async _transmitter() {
      let st = this.channels[this.ST];
      if (st < 0) {
          st = 256 + st;
      }

      let th = this.channels[this.TH];
      if (th < 0) {
          th = 256 + th;
      }

      let aux = this.channels[this.AUX];
      if (aux < 0) {
          aux = 256 + aux;
      }

      let aux2 = this.channels[this.AUX2];
      if (aux2 < 0) {
          aux2 = 256 + aux2;
      }

      let aux3 = this.channels[this.AUX3];
      if (aux3 < 0) {
          aux3 = 256 + aux3;
      }

      let mode_byte = 0;
      if (this.channels[this.AUX] > 0) {
          mode_byte += 0x01;
      }
      if (this.channels[this.STARTUP_MODE]) {
          mode_byte += 0x10;
      }
      if (this.channels[this.MULTI_AUX]) {
          mode_byte += 0x08;
      }

      let data;
      if (this.channels[this.MULTI_AUX]) {
        data = new Uint8Array(7);
        data[0] = this.SLAVE_MAGIC_BYTE;
        data[1] = st;
        data[2] = th;
        data[3] = mode_byte;
        data[4] = aux;
        data[5] = aux2;
        data[6] = aux3;
      }
      else {
        data = new Uint8Array(4);
        data[0] = this.SLAVE_MAGIC_BYTE;
        data[1] = st;
        data[2] = th;
        data[3] = mode_byte;
      }

      try {
        this.uart.write(data, 'dont-log');
      }
      catch (e) {
          console.error('uart.write exception:', e);
          return;
      }

      if (this.transmitter_active) {
        setTimeout(this._transmitter.bind(this), 20);
      }
  }

  channel_changed(channel, value) {
    value = parseInt(value, 10);

    if (value < - 100 || value > 100) {
      throw 'value ' + value + ' is out of range -100..0..+100';
    }

    this.channels[channel] = value;

    if (this.channels[this.NO_SIGNAL]) {
      this.transmitter_active = false;
    }
    else {
      if (!this.transmitter_active) {
        this.transmitter_active = true;
        this._transmitter();
      }
    }
  }

  set onMessageCallback(fn) {
    this.messageCallback = fn;
  }


  _parse_config(msg) {
    if (msg == this.last_received_config) {
      return;
    }

    this.last_received_config = msg;
    console.log('New config: ' + msg);

    let values = msg.split(' ');
    if (values.length < 1) {
      return;
    }

    /*

    CONFIG array elements:
    ======================
    0: CONFIG
    1: config.flags2.multi_aux
    2: config.flags.ch3_is_momentary
    3: config.flags.ch3_is_two_button,
    4: config.aux_type
    5: config.aux_function
    6: config.aux2_type
    7: config.aux2_function
    8: config.aux3_type
    9: config.aux3_function

    AUX_TYPE_T:
    -----------
    TWO_POSITION = 0,
    TWO_POSITION_UP_DOWN = 1,
    MOMENTARY = 2,
    THREE_POSITION = 3,
    ANALOG = 4

    AUX_FUNCTION_T:
    ---------------
    NOT_USED = 0,
    MULTI_FUNCTION = 1,
    GEARBOX = 2,
    WINCH = 3,
    SERVO = 4,
    INDICATORS = 5,
    HAZARD = 6,
    LIGHT_SWITCH = 7

    */

    if (values.length < 10) {
      console.error('CONFIG has less than 10 entries: "' + msg + '" (length=' + values.length + ')');
      return;
    }

    this.config[this.CONFIG_TYPE] = this.CONFIG_TYPE_AUTO;

    if ((values[1] != '0')) {
      this.config[this.MULTI_AUX] = true;

      this.config[this.AUX][this.AUX_TYPE] = values[4];
      this.config[this.AUX2][this.AUX_TYPE] = values[6];
      this.config[this.AUX3][this.AUX_TYPE] = values[8];

      this.config[this.AUX][this.AUX_FUNCTION] = values[5];
      this.config[this.AUX2][this.AUX_FUNCTION] = values[7];
      this.config[this.AUX3][this.AUX_FUNCTION] = values[9];
    }
    else {
      this.config[this.MULTI_AUX] = false;

      // 2: config.flags.ch3_is_momentary
      if (values[2] != '0') {
          this.config[this.AUX][this.AUX_TYPE] = this.AUX_TYPE_MOMENTARY;
      }
      // 3: config.flags.ch3_is_two_button
      else if (values[3] != '0') {
          this.config[this.AUX][this.AUX_TYPE] = this.AUX_TYPE_TWO_POSITION_UP_DOWN;
      }
      else {
          this.config[this.AUX][this.AUX_TYPE] = this.AUX_TYPE_TWO_POSITION;
      }

      this.config[this.AUX2][this.AUX_TYPE] = values[6];
      this.config[this.AUX3][this.AUX_TYPE] = values[8];
      this.config[this.AUX2][this.AUX_FUNCTION] = values[7];
      this.config[this.AUX3][this.AUX_FUNCTION] = values[9];
    }

    if (this.configChangedCallback) {
      this.configChangedCallback(this.config);
    }
  }

  getAuxFunctionLabel(aux_function_number) {
    switch (parseInt(aux_function_number)) {
    case 0:
      return 'Not used';

    case 1:
      return 'Multi-function switch';

    case 2:
      return 'Gearbox';

    case 3:
      return 'Winch';

    case 4:
      return 'Servo';

    case 5:
      return 'Indicators';

    case 6:
      return 'Hazard';

    case 7:
      return 'Light switch';

    default:
      return 'UNDEFINED aux_function_number ' + aux_function_number;
    }
  }

  set onConfigChangedCallback(fn) {
    this.configChangedCallback = fn;
  }

  get config_default() {
    const config = {};
    config[this.CONFIG_TYPE] = this.CONFIG_TYPE_DEFAULT;
    config[this.MULTI_AUX] = false;
    config[this.AUX] = {};
    config[this.AUX][this.AUX_TYPE] = this.AUX_TYPE_TWO_POSITION;
    config[this.AUX][this.AUX_FUNCTION] = this.AUX_FUNCTION_MULTI_FUNCTION;
    config[this.AUX2] = {};
    config[this.AUX2][this.AUX_TYPE] = this.AUX_TYPE_ANALOG;
    config[this.AUX2][this.AUX_FUNCTION] = this.AUX_FUNCTION_NOT_USED;
    config[this.AUX3] = {};
    config[this.AUX3][this.AUX_TYPE] = this.AUX_TYPE_ANALOG;
    config[this.AUX3][this.AUX_FUNCTION] = this.AUX_FUNCTION_NOT_USED;
    return config;
  }

  get config_manual_3ch() {
    const config = {};
    config[this.CONFIG_TYPE] = this.CONFIG_TYPE_MANUAL;
    config[this.MULTI_AUX] = false;
    config[this.AUX] = {};
    config[this.AUX][this.AUX_TYPE] = this.AUX_TYPE_TWO_POSITION;
    config[this.AUX][this.AUX_FUNCTION] = this.AUX_FUNCTION_MULTI_FUNCTION;
    config[this.AUX2] = {};
    config[this.AUX2][this.AUX_TYPE] = this.AUX_TYPE_ANALOG;
    config[this.AUX2][this.AUX_FUNCTION] = this.AUX_FUNCTION_NOT_USED;
    config[this.AUX3] = {};
    config[this.AUX3][this.AUX_TYPE] = this.AUX_TYPE_ANALOG;
    config[this.AUX3][this.AUX_FUNCTION] = this.AUX_FUNCTION_NOT_USED;
    return config;
  }

  get config_manual_5ch() {
    const config = {};
    config[this.CONFIG_TYPE] = this.CONFIG_TYPE_MANUAL;
    config[this.MULTI_AUX] = true;
    config[this.AUX] = {};
    config[this.AUX][this.AUX_TYPE] = this.AUX_TYPE_TWO_POSITION;
    config[this.AUX][this.AUX_FUNCTION] = this.AUX_FUNCTION_MULTI_FUNCTION;
    config[this.AUX2] = {};
    config[this.AUX2][this.AUX_TYPE] = this.AUX_TYPE_ANALOG;
    config[this.AUX2][this.AUX_FUNCTION] = this.AUX_FUNCTION_NOT_USED;
    config[this.AUX3] = {};
    config[this.AUX3][this.AUX_TYPE] = this.AUX_TYPE_ANALOG;
    config[this.AUX3][this.AUX_FUNCTION] = this.AUX_FUNCTION_NOT_USED;
    return config;
  }
}