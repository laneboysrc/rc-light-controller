/*

API:

How do we separate the UI elements from the function?

ST slider (neutral button can be local to UI)
TH slider (neutral button can be local to UI)
Startup mode
AUX slider
AUX type
AUX2 slider
AUX2 type
AUX3 slider
AUX3 type

Simulate no signal?



channel_changed(channel, value)

*/


class Preprocessor_simulator {
  ST = 'ST';
  TH = 'TH';
  AUX = 'AUX';
  AUX2 = 'AUX2';
  AUX3 = 'AUX3';
  STARTUP_MODE = 'STARTUP_MODE';
  NO_SIGNAL = 'NO_SIGNAL';

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

  MULTI_AUX = 'multi_aux';

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

    this.config = this.default_config;

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
      if (this.startup_mode) {
          mode_byte += 0x10;
      }
      if (this.config[this.MULTI_AUX]) {
          mode_byte += 0x08;
      }

      let data;
      if (this.config[this.MULTI_AUX]) {
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
  }

  set onMessageCallback(fn) {
    this.messageCallback = fn;
  }


  _parse_config(msg) {
    if (msg == this.config_string) {
        return;
    }

    this.config_string = msg;
    console.log('New config: ' + msg);

    let values = msg.split(' ');
    if (values.length < 1) {
        return;
    }

    values.unshift('A');

    /*

    CONFIG array elements:
    ======================
    0: A=automatic 3=force_3ch 5=force_5ch
    1: CONFIG
    2: config.flags2.multi_aux
    3: config.flags.ch3_is_momentary
    4: config.flags.ch3_is_two_button,
    5: config.aux_type
    6: config.aux_function
    7: config.aux2_type
    8: config.aux2_function
    9: config.aux3_type
    10: config.aux3_function

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

    if (values[0] == '3') {
      // const elements = document.querySelectorAll('.multi-aux');
      // elements.forEach(e => {
      //   e.classList.add('hidden');
      // });

      // [aux, aux2, aux3].forEach(a => {
      //   a.elType.disabled = false;
      // });
    }

    else if (values[0] == '5') {
      // const elements = document.querySelectorAll('.multi-aux');
      // elements.forEach(e => {
      //     e.classList.remove('hidden');
      // });

      // [aux, aux2, aux3].forEach(a => {
      //     a.elType.disabled = false;
      // });
    }

    else if (values.length >= 11) {
      // 2:  config.flags2.multi_aux
      if ((values[2] != '0')) {
        this.config[this.MULTI_AUX] = true;

        this.config[this.AUX][this.AUX_TYPE] = values[5];
        this.config[this.AUX2][this.AUX_TYPE] = values[7];
        this.config[this.AUX3][this.AUX_TYPE] = values[9];

        this.config[this.AUX][this.AUX_FUNCTION] = values[6];
        this.config[this.AUX2][this.AUX_FUNCTION] = values[8];
        this.config[this.AUX3][this.AUX_FUNCTION] = values[10];
      }
      else {
        this.config[this.MULTI_AUX] = false;

        // 3: config.flags.ch3_is_momentary
        if (values[3] != '0') {
            this.config[this.AUX][this.AUX_TYPE] = this.AUX_TYPE_MOMENTARY;
        }
        // 4: config.flags.ch3_is_two_button
        else if (values[4] != '0') {
            this.config[this.AUX][this.AUX_TYPE] = this.AUX_TYPE_TWO_POSITION_UP_DOWN;
        }
        else {
            this.config[this.AUX][this.AUX_TYPE] = this.AUX_TYPE_TWO_POSITION;
        }

        this.config[this.AUX2][this.AUX_TYPE] = values[7];
        this.config[this.AUX3][this.AUX_TYPE] = values[9];
        this.config[this.AUX2][this.AUX_FUNCTION] = values[8];
        this.config[this.AUX3][this.AUX_FUNCTION] = values[10];
      }
    }

    else {
      this.config = this.default_config;
      // const elements = document.querySelectorAll('.multi-aux');
      // elements.forEach(e => {
      //     e.classList.add('hidden');
      // });

      // [aux, aux2, aux3].forEach(a => {
      //     a.elType.disabled = false;
      // });
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

  get default_config() {
    const config = {};
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
}