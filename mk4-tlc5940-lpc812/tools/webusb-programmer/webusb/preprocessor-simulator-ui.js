class Preprocessor_simulator_ui {
  MAX_LOG_LINES = 20;
  BOUND_EVENT = '_preprocessor_sim_event';
  BOUND_LISTENER = '_preprocessor_sim_listener';
  SIMULATOR_CHANNEL = '_preprocessor_sim_channel';

  AUX = 'AUX';
  AUX2 = 'AUX2';
  AUX3 = 'AUX3';

  UP_DOWN_TIMEOUT = 500;

  constructor(simulator) {
    this.simulator = simulator;

    this.el = {};
    this.el.aux = {};
    this.el.aux[this.AUX] = {};
    this.el.aux[this.AUX2] = {};
    this.el.aux[this.AUX3] = {};

    this.el.diagnostics = document.querySelector('#diagnostics-messages');
    this.el.startup_mode = document.querySelector('#startup-mode');
    this.el.no_signal = document.querySelector('#no-signal');

    this.el.steering = document.querySelector('#steering');
    this.el.throttle = document.querySelector('#throttle');
    this.el.steering_neutral = document.querySelector('#steering-neutral');
    this.el.throttle_neutral = document.querySelector('#throttle-neutral');
    this.el.steering[this.SIMULATOR_CHANNEL] = this.simulator.ST;
    this.el.throttle[this.SIMULATOR_CHANNEL] = this.simulator.TH;

    this.el.aux[this.AUX].channel = this.simulator.AUX;
    this.el.aux[this.AUX].type = document.querySelector('#aux-type');
    this.el.aux[this.AUX].function = document.querySelector('#aux-function');
    this.el.aux[this.AUX].slider = document.querySelector('#aux-slider');
    this.el.aux[this.AUX].toggle = document.querySelector('#aux-toggle');
    this.el.aux[this.AUX2].channel = this.simulator.AUX2;
    this.el.aux[this.AUX2].type = document.querySelector('#aux2-type');
    this.el.aux[this.AUX2].function = document.querySelector('#aux2-function');
    this.el.aux[this.AUX2].slider = document.querySelector('#aux2-slider');
    this.el.aux[this.AUX2].toggle = document.querySelector('#aux2-toggle');
    this.el.aux[this.AUX3].channel = this.simulator.AUX3;
    this.el.aux[this.AUX3].type = document.querySelector('#aux3-type');
    this.el.aux[this.AUX3].function = document.querySelector('#aux3-function');
    this.el.aux[this.AUX3].slider = document.querySelector('#aux3-slider');
    this.el.aux[this.AUX3].toggle = document.querySelector('#aux3-toggle');

    this._addEventListener(this.el.startup_mode, 'change', this.startup_mode_changed.bind(this));
    this._addEventListener(this.el.no_signal, 'change', this.no_signal_changed.bind(this));

    this._addEventListener(this.el.steering, 'change', this.slider_changed.bind(this));
    this._addEventListener(this.el.steering, 'input', this.slider_changed.bind(this));
    this._addEventListener(this.el.throttle, 'change', this.slider_changed.bind(this));
    this._addEventListener(this.el.throttle, 'input', this.slider_changed.bind(this));

    this._addEventListener(this.el.steering_neutral, 'click', this.slider_center.bind(this, this.el.steering));
    this._addEventListener(this.el.throttle_neutral, 'click', this.slider_center.bind(this, this.el.throttle));

    this._addEventListener(this.el.aux[this.AUX].slider, 'change', this.aux_slider_changed.bind(this, this.el.aux[this.AUX]));
    this._addEventListener(this.el.aux[this.AUX].slider, 'input', this.aux_slider_changed.bind(this, this.el.aux[this.AUX]));
    this._addEventListener(this.el.aux[this.AUX2].slider, 'change', this.aux_slider_changed.bind(this, this.el.aux[this.AUX2]));
    this._addEventListener(this.el.aux[this.AUX2].slider, 'input', this.aux_slider_changed.bind(this, this.el.aux[this.AUX2]));
    this._addEventListener(this.el.aux[this.AUX3].slider, 'change', this.aux_slider_changed.bind(this, this.el.aux[this.AUX3]));
    this._addEventListener(this.el.aux[this.AUX3].slider, 'input', this.aux_slider_changed.bind(this, this.el.aux[this.AUX3]));

    this._addEventListener(this.el.aux[this.AUX].toggle, 'mousedown', this.aux_toggle.bind(this, this.el.aux[this.AUX]));
    this._addEventListener(this.el.aux[this.AUX].toggle, 'mouseup', this.aux_toggle.bind(this, this.el.aux[this.AUX]));
    this._addEventListener(this.el.aux[this.AUX2].toggle, 'mousedown', this.aux_toggle.bind(this, this.el.aux[this.AUX2]));
    this._addEventListener(this.el.aux[this.AUX2].toggle, 'mouseup', this.aux_toggle.bind(this, this.el.aux[this.AUX2]));
    this._addEventListener(this.el.aux[this.AUX3].toggle, 'mousedown', this.aux_toggle.bind(this, this.el.aux[this.AUX3]));
    this._addEventListener(this.el.aux[this.AUX3].toggle, 'mouseup', this.aux_toggle.bind(this, this.el.aux[this.AUX3]));

    this._addEventListener(this.el.aux[this.AUX].type, 'change', this.aux_type_changed.bind(this, this.el.aux[this.AUX]));
    this._addEventListener(this.el.aux[this.AUX2].type, 'change', this.aux_type_changed.bind(this, this.el.aux[this.AUX2]));
    this._addEventListener(this.el.aux[this.AUX3].type, 'change', this.aux_type_changed.bind(this, this.el.aux[this.AUX3]));


    this.el.no_signal.checked = false;
    this.el.startup_mode.checked = true;
    setTimeout(this.startup_mode_auto_timeout.bind(this), 1000);

    this.config_changed(this.simulator.default_config);

    this.log_testing_clear();
    simulator.onMessageCallback = this.log_testing.bind(this);
    simulator.onConfigChangedCallback = this.config_changed.bind(this);
  }

  _addEventListener(element, event, listener) {
    element[this.BOUND_EVENT] = event;
    element[this.BOUND_LISTENER] = listener;
    element.addEventListener(event, element[this.BOUND_LISTENER], false);
  }

  slider_changed(event) {
    if (this.SIMULATOR_CHANNEL in event.target) {
      this.simulator.channel_changed(event.target[this.SIMULATOR_CHANNEL], event.target.value);
    }
  }

  slider_center(target, event) {
    if (this.SIMULATOR_CHANNEL in target) {
      target.value = 0;
      this.simulator.channel_changed(target[this.SIMULATOR_CHANNEL], target.value);
    }
  }

  aux_slider_changed(aux, event) {
    this.simulator.channel_changed(aux.channel, aux.slider.value);
  }

  aux_toggle(aux, event) {
    const s = this.simulator;

    if (event.type == 'keydown') {
      if (aux.buttonDown) {
        return;
      }
      aux.buttonDown = true;
    }
    else if (event.type == 'keyup') {
      aux.buttonDown = false;
    }

    if (aux.type.value == s.AUX_TYPE_ANALOG) {
      if (event.type == 'keydown' || event.type == 'mousedown') {
        this.update_aux_value(aux, 0);
      }
    }
    else if (aux.type.value == s.AUX_TYPE_MOMENTARY) {
      if (event.type == 'keydown' || event.type == 'mousedown') {
        this.update_aux_value(aux, +100);
      }
      else {
        this.update_aux_value(aux, -100);
      }
    }
    else if (aux.type.value == s.AUX_TYPE_THREE_POSITION) {
      if (event.type == 'keydown' || event.type == 'mousedown') {
        if (parseInt(aux.slider.value, 10) > 0) {
          this.update_aux_value(aux, 0);
          aux.direction_down = true;
        }
        else if (parseInt(aux.slider.value, 10) < 0) {
          this.update_aux_value(aux, 0);
          aux.direction_down = false;
        }
        else {
          if (aux.direction_down) {
            this.update_aux_value(aux, -100);
          }
          else {
            this.update_aux_value(aux, 100);
          }
        }
      }
    }
    else {
      if (event.type == 'keydown' || event.type == 'mousedown') {
        if (aux.type.value == s.AUX_TYPE_TWO_POSITION_UP_DOWN) {
          if (Date.now() > aux.upDownTimeout) {
            this.update_aux_value(aux, -100);
            aux.upDownTimeout = Date.now() + this.UP_DOWN_TIMEOUT;
            return;
          }
        }

        aux.upDownTimeout = Date.now() + this.UP_DOWN_TIMEOUT;
        if (parseInt(aux.slider.value, 10) > 0) {
          this.update_aux_value(aux, -100);
        }
        else  {
          this.update_aux_value(aux, +100);
        }
      }
    }
  }

  aux_type_changed(aux) {
    const s = this.simulator;

    const type = parseInt(aux.type.value);

    switch (type) {
    case s.AUX_TYPE_TWO_POSITION:
    case s.AUX_TYPE_TWO_POSITION_UP_DOWN:
      if (aux.slider.value <= 0) {
        this.update_aux_value(aux, -100);
      }
      else {
        this.update_aux_value(aux, 100);
      }

      aux.slider.step = 200;
      aux.toggle.textContent = 'toggle';
      break;

    case s.AUX_TYPE_MOMENTARY:
      aux.slider.step = 200;
      this.update_aux_value(aux, -100);
      aux.toggle.textContent = 'toggle';
      break;

    case s.AUX_TYPE_THREE_POSITION:
      aux.slider.step = 100;
      aux.toggle.textContent = 'toggle';
      break;

    case s.AUX_TYPE_ANALOG:
      aux.slider.step = 1;
      aux.toggle.textContent = 'neutral';
      break;
    }
  }

  update_aux_value(aux, value) {
    aux.slider.value = value;
    this.simulator.channel_changed(aux.channel, aux.slider.value);
  }

  reset_aux(aux) {
    const s = this.simulator;

    const type = parseInt(aux.type.value);

    switch (type) {
    case s.AUX_TYPE_TWO_POSITION:
    case s.AUX_TYPE_TWO_POSITION_UP_DOWN:
    case s.AUX_TYPE_MOMENTARY:
      this.update_aux_value(aux, -100);
      break;

    case s.AUX_TYPE_THREE_POSITION:
    case s.AUX_TYPE_ANALOG:
    default:
      this.update_aux_value(aux, 0);
      break;
    }
  }

  startup_mode_changed() {
    if (this.el.startup_mode.checked) {
      this.el.steering.value = 0;
      this.el.throttle.value = 0;

      this.reset_aux(this.el.aux[this.AUX]);
      this.reset_aux(this.el.aux[this.AUX2]);
      this.reset_aux(this.el.aux[this.AUX3]);

      this.simulator.channel_changed(this.el.steering.channel, this.el.steering.value);
      this.simulator.channel_changed(this.el.throttle.channel, this.el.throttle.value);
      this.simulator.channel_changed(this.el.aux[this.AUX].channel, this.el.aux[this.AUX].slider.value);
      this.simulator.channel_changed(this.el.aux[this.AUX2].channel, this.el.aux[this.AUX2].slider.value);
      this.simulator.channel_changed(this.el.aux[this.AUX3].channel, this.el.aux[this.AUX3].slider.value);
    }
    this.update_ui_state();
  }

  startup_mode_auto_timeout() {
    if (this.el.startup_mode.checked) {
      this.el.startup_mode.checked = false;
      this.startup_mode_changed();
    }
  }

  no_signal_changed() {
    if (this.el.no_signal.checked) {
      this.simulator.channel_changed(this.simulator.NO_SIGNAL, 100);
    }
    else {
      this.simulator.channel_changed(this.simulator.NO_SIGNAL, 0);
    }
    this.update_ui_state();
  }

  config_changed(new_config) {
    console.log(new_config);
    this.config = new_config;
    this.update_ui_state();
  }

  update_ui_state() {
    const s = this.simulator;

    if (this.el.startup_mode.checked || this.el.no_signal.checked) {
      this.el.steering.disabled = true;
      this.el.steering_neutral.disabled = true;
      this.el.throttle.disabled = true;
      this.el.throttle_neutral.disabled = true;
      this.el.aux[this.AUX].type.disabled = true;
      this.el.aux[this.AUX].function.disabled = true;
      this.el.aux[this.AUX].slider.disabled = true;
      this.el.aux[this.AUX].toggle.disabled = true;
      this.el.aux[this.AUX2].type.disabled = true;
      this.el.aux[this.AUX2].function.disabled = true;
      this.el.aux[this.AUX2].slider.disabled = true;
      this.el.aux[this.AUX2].toggle.disabled = true;
      this.el.aux[this.AUX3].type.disabled = true;
      this.el.aux[this.AUX3].function.disabled = true;
      this.el.aux[this.AUX3].slider.disabled = true;
      this.el.aux[this.AUX3].toggle.disabled = true;
    }
    else {
      this.el.steering.disabled = false;
      this.el.steering_neutral.disabled = false;
      this.el.throttle.disabled = false;
      this.el.throttle_neutral.disabled = false;
      this.el.aux[this.AUX].type.disabled = false;
      this.el.aux[this.AUX].function.disabled = false;
      this.el.aux[this.AUX].slider.disabled = false;
      this.el.aux[this.AUX].toggle.disabled = false;
      this.el.aux[this.AUX2].type.disabled = !this.config[s.MULTI_AUX];
      this.el.aux[this.AUX2].function.disabled = !this.config[s.MULTI_AUX];
      this.el.aux[this.AUX2].slider.disabled = !this.config[s.MULTI_AUX];
      this.el.aux[this.AUX2].toggle.disabled = !this.config[s.MULTI_AUX];
      this.el.aux[this.AUX3].type.disabled = !this.config[s.MULTI_AUX];
      this.el.aux[this.AUX3].function.disabled = !this.config[s.MULTI_AUX];
      this.el.aux[this.AUX3].slider.disabled = !this.config[s.MULTI_AUX];
      this.el.aux[this.AUX3].toggle.disabled = !this.config[s.MULTI_AUX];
    }

    this.el.aux[this.AUX].function.textContent = s.getAuxFunctionLabel(this.config[s.AUX][s.AUX_FUNCTION]);
    this.el.aux[this.AUX2].function.textContent = s.getAuxFunctionLabel(this.config[s.AUX2][s.AUX_FUNCTION]);
    this.el.aux[this.AUX3].function.textContent = s.getAuxFunctionLabel(this.config[s.AUX3][s.AUX_FUNCTION]);

    this.el.aux[this.AUX].type.value = this.config[s.AUX][s.AUX_TYPE];
    this.el.aux[this.AUX2].type.value = this.config[s.AUX2][s.AUX_TYPE];
    this.el.aux[this.AUX3].type.value = this.config[s.AUX3][s.AUX_TYPE];

    this.aux_type_changed(this.el.aux[this.AUX]);
    this.aux_type_changed(this.el.aux[this.AUX2]);
    this.aux_type_changed(this.el.aux[this.AUX3]);
  }

  close() {
    // Remove all event listeners that we have set
    for (name in this.el) {
      const element = this.el[name];
      if (this.BOUND_EVENT in element) {
        element.removeEventListener(element[this.BOUND_EVENT], element[this.BOUND_LISTENER], false);
        delete element[this.BOUND_EVENT];
        delete element[this.BOUND_LISTENER];
      }
    };
  }

  log_testing(msg, displayClass) {
    const content = new Date(Date.now()).toISOString() + ' ' + msg;
    const div = document.createElement('div');
    div.textContent = content;
    console.log("sim: " + content);

    if (displayClass) {
      div.classList.add(displayClass);
    }

    if (this.el.diagnostics.firstChild) {
      this.el.diagnostics.insertBefore(div, this.el.diagnostics.firstChild);
      while (this.el.diagnostics.childElementCount > this.MAX_LOG_LINES) {
        this.el.diagnostics.removeChild(this.el.diagnostics.lastChild);
      }
    }
    else {
      this.el.diagnostics.appendChild(div);
    }
  }

  log_testing_clear() {
    while (this.el.diagnostics.childElementCount > 0) {
      this.el.diagnostics.removeChild(this.el.diagnostics.lastChild);
    }
  }


  aux_toggle_handler() {
    if (!this.simulator) {
      return;
    }

    if (this.aux_value) {
      this.aux_value = false;
      this.simulator.channel_changed(this.simulator.AUX, -100);
    }
    else {
      this.aux_value = true;
      this.simulator.channel_changed(this.simulator.AUX, 100);
    }
  }
}