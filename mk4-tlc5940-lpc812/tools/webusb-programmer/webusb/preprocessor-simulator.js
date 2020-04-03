class Preprocessor_simulator {
  constructor(uart) {
    this.uart = uart;

    this.active = true;
    this._reader();
  }

  close() {
    this.active = false;
  }

  async _reader() {
    console.log('Preprocessor_simulator.reader() start');

    while (this.active) {
      let line = '';
      try {
        line = await programmer.readline('\n', 0);
      }
      catch (e) {
        await new Promise(resolve => {setTimeout(() => {resolve()}, 100)});;
      }

      if (line && this.messageCallback) {
        this.messageCallback(line);
      }
    }

    console.log('Preprocessor_simulator.reader() end');
  }

  set onMessageCallback(fn) {
    this.messageCallback = fn;
  }

}