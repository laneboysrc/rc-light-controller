/*global chrome_uart */

const SLAVE_MAGIC_BYTE = 0x87;

// Pressing any of those keys triggers CH3
const CH3_KEYS = '3acux';

const VENDOR_ID = 0x6666;
const TEST_INTERFACE = 1;
const TEST_EP_IN = 1;
const TEST_EP_OUT = 2;
const EP_SIZE = 64;

const MAX_UART_LOG_LINES = 20;

class _preprocessor_uart {
    constructor(parent) {
        this.parent = parent;
        this.uart;
    }

    async init(port, baudrate) {
        this.uart = new chrome_uart();
        await this.uart.open(port, baudrate, 8, 'n', 1);
        this.uart.setTimeout(0.1);

        this.send_data();
        this.receive_data();
    }

    async disconnect() {
        if (this.uart) {
            this.uart.close();
            this.uart = undefined;
        }
    }

    async send_data() {
        if (this.uart) {
            let data = this.parent.build_preprocessor_packet();
            await this.uart.write(data);
            setTimeout(this.send_data.bind(this), 20);
        }
    }

    async receive_data() {
        while (this.uart) {
            let result = await this.uart.readline();
            if (result.length > 0) {
                this.parent.diagnostics(result);
            }
        }
    }
}


class _preprocessor_webusb {
    constructor(parent) {
        this.parent = parent;
        this.elWebusbConnect = document.getElementById('webusb-connect');
        this.elWebusbConnectButton = document.getElementById('webusb-connect-button');
        this.webusb_device;
    }

    async init() {
        navigator.usb.addEventListener('connect', this.webusb_device_connected.bind(this));
        navigator.usb.addEventListener('disconnect', this.webusb_device_disconnected.bind(this));

        let devices = await navigator.usb.getDevices();
        if (devices.length) {
            this.connect(devices[0]);
        }
        else {
            // Show the connect button
            this.elWebusbConnect.classList.remove('hidden');
            this.elWebusbConnectButton.addEventListener('click', this.pair_device.bind('this'));
        }
    }

    async disconnect() {
        if (this.webusb_device) {
            try {
                await this.webusb_device.close();
            }
            finally {
                this.webusb_device = undefined;
            }
        }

        navigator.usb.removeEventListener('connect', this.webusb_device_connected.bind(this));
        navigator.usb.removeEventListener('disconnect', this.webusb_device_disconnected.bind(this));

        this.elWebusbConnect.classList.add('hidden');
        this.elWebusbConnectButton.removeEventListener('click', this.pair_device.bind('this'));
    }

    async send_data() {
        let data = this.parent.build_preprocessor_packet();

        try {
            let result = await this.webusb_device.transferOut(TEST_EP_OUT, data);
            if (result.status == 'ok') {
                setTimeout(this.send_data.bind(this), 20);
            }
            else {
                this.parent.log.error('transferOut() failed:', result.status);
            }
        }
        catch (e) {
            this.parent.log.error('transferOut() exception:', e);
            return;
        }
    }

    async receive_data() {
        for (;;) {
            try {
                let result = await this.webusb_device.transferIn(TEST_EP_IN, EP_SIZE);
                if (result.status == 'ok') {
                    const decoder = new TextDecoder('utf-8');
                    const value = decoder.decode(result.data);
                    this.parent.diagnostics(value);
                }
                else {
                    this.parent.log.error('transferIn() failed:', result.status);
                }
            }
            catch (e) {
                this.parent.log.error('transferIn() exception:', e);
                return;
            }
        }
    }

    async pair_device(event) {
        event.preventDefault();
        const options = {filters:[{vendorId: VENDOR_ID}]};
        let device;
        try {
            device = await navigator.usb.requestDevice(options);
            if (device) {
                await this.connect(device);
                if (this.webusb_device) {
                    this.elWebusbConnect.classList.add('hidden');
                }
            }
        }
        catch (e) {
            this.parent.log.log('requestDevice failed:' + e);
        }
    }

    async connect(device) {
        try {
            await device.open();
            if (device.configuration === null) {
                await device.selectConfiguration(1);
            }

            await device.claimInterface(TEST_INTERFACE);
        }
        catch (e) {
            this.parent.log.error('Failed to open the device', e);
            return;
        }

        this.parent.log.log('Connected to Light Controller with serial number ' + device.serialNumber);
        this.webusb_device = device;
        this.send_data();
        this.receive_data();
    }

    webusb_device_connected(connection_event) {
        const device = connection_event.device;
        this.parent.log.log('USB device connected:', device);
        if (!this.webusb_device) {
            if (device && device.vendorId == VENDOR_ID) {
                this.connect(device);
            }
        }
    }

    webusb_device_disconnected(connection_event) {
        this.parent.log.log('USB device disconnected:', connection_event);
        const disconnected_device = connection_event.device;
        if (this.webusb_device &&  disconnected_device == this.webusb_device) {
            this.webusb_device = undefined;
        }
    }
}

class preprocessor {
    constructor() {
        this.log = console;
        // this.log = {
        //     log: () => {},
        //     info: () => {},
        //     warn: () => {},
        //     error: () => {},
        //     dir: () => {}
        // };

        this.ch3 = 0;
        this.startupMode = false;

        this.elCH3 = document.getElementById('ch3');
        this.elDiagnostics = document.getElementById('diagnostics');
        this.elDiagnosticsMessages = document.getElementById('diagnostics-messages');
        this.elMomentary = document.getElementById('momentary');
        this.elNotConnected = document.getElementById('not-connected');
        this.elStartupMode = document.getElementById('startup-mode');
        this.elSteering = document.getElementById('steering');
        this.elSteeringNeutral = document.getElementById('steering-neutral');
        this.elThrottle = document.getElementById('throttle');
        this.elThrottleNeutral = document.getElementById('throttle-neutral');

        this.webusb_device = undefined;
        this.uart = undefined;

        document.addEventListener('keydown', this.keydown_event_handler.bind(this));
        document.addEventListener('keyup', this.keyup_event_handler.bind(this));
        this.elSteeringNeutral.addEventListener('click', this.center_steering.bind(this));
        this.elThrottleNeutral.addEventListener('click', this.center_throttle.bind(this));
        this.elCH3.addEventListener('click', this.ch3_clicked.bind(this));
        this.elCH3.addEventListener('mousedown', this.ch3_down.bind(this));
        this.elCH3.addEventListener('mouseup', this.ch3_up.bind(this));
        this.elStartupMode.addEventListener('change', this.startup_mode_changed.bind(this));
    }

    center_steering(event) {
        event.preventDefault();
        this.elSteering.value = 0;
    }

    center_throttle(event) {
        event.preventDefault();
        this.elThrottle.value = 0;
    }

    ch3_clicked(event) {
        event.preventDefault();
        this.sendCh3('click');
    }

    ch3_down(event) {
        event.preventDefault();
        this.sendCh3('down');
    }

    ch3_up(event) {
        event.preventDefault();
        this.sendCh3('up');
    }

    startup_mode_changed() {
        this.sendStartup(this.elStartupMode.checked);
        if (this.startupTimer) {
            clearTimeout(this.startupTimer);
        }
    }

    clear_startup_mode() {
        this.startupMode = false;
        this.elStartupMode.checked = this.startupMode;
        this.sendStartup(this.startupMode);
    }

    sendCh3(action) {
        if (this.elMomentary.checked) {
            if (action == 'down') {
                this.ch3 = 1;
            }
            if (action == 'up') {
                this.ch3 = 0;
            }
        }
        else {
            if (action == 'click') {
                this.ch3 = this.ch3 ? 0 : 1;
            }
        }
        return false;
    }

    sendStartup(mode) {
        if (mode) {
            this.elSteeringNeutral.click();
            this.elThrottleNeutral.click();
            this.elSteering.disabled = true;
            this.elThrottle.disabled = true;
            this.elCH3.disabled = true;
        }
        else {
            this.elSteering.disabled = false;
            this.elThrottle.disabled = false;
            this.elCH3.disabled = false;
        }
        this.startupMode = mode ? 1 : 0;
    }

    // *************************************************************************
    reset_ui() {
        this.startupMode = true;
        this.ch3 = 0;
        this.elSteering.value = 0;
        this.elThrottle.value = 0;

        this.elStartupMode.checked = this.startupMode;
        this.sendStartup(this.startupMode);
        this.startupTimer = setTimeout(this.clear_startup_mode.bind(this), 2000);
    }

    // *************************************************************************
    keydown_event_handler(event) {
        const key = event.key.toLowerCase();
        if (CH3_KEYS.indexOf(key) >= 0) {
            this.sendCh3('click');
            this.sendCh3('down');
        }
        else if (key == 's') {
            this.elStartupMode.click();
        }
        return true;
    }

    // *************************************************************************
    keyup_event_handler(event) {
        const key = event.key.toLowerCase();
        if (CH3_KEYS.indexOf(key) >= 0) {
            this.sendCh3('up');
        }
        return true;
    }

    // *************************************************************************
    diagnostics(msg) {
        const lines = msg.trim().split('\n');
        for (msg of lines) {
            msg = new Date(Date.now()).toISOString() + ' ' + msg;

            this.log.log(msg);

            const el = document.createElement('div');
            el.textContent += msg;
            if (this.elDiagnosticsMessages.firstChild) {
                this.elDiagnosticsMessages.insertBefore(el, this.elDiagnosticsMessages.firstChild);
                while (this.elDiagnosticsMessages.childElementCount > MAX_UART_LOG_LINES) {
                    this.elDiagnosticsMessages.removeChild(this.elDiagnosticsMessages.lastChild);
                }
            }
            else {
                this.elDiagnosticsMessages.appendChild(el);
            }
        }
    }

    // *************************************************************************
    build_preprocessor_packet() {
        let st = parseInt(this.elSteering.value, 10);
        if (st < 0) {
            st = 256 + st;
        }

        let th = parseInt(this.elThrottle.value, 10);
        if (th < 0) {
            th = 256 + th;
        }

        let last_byte = 0;
        if (this.ch3) {
            last_byte += 0x01;
        }
        if (this.startupMode) {
            last_byte += 0x10;
        }

        let data = new Uint8ClampedArray(4);
        data[0] = SLAVE_MAGIC_BYTE;
        data[1] = st;
        data[2] = th;
        data[3] = last_byte;
        return data;
    }

    // *************************************************************************
    disconnect() {
        if (this.device) {
            this.device.disconnect();
            this.device = undefined;
        }
    }

    // *************************************************************************
    async init(port, baudrate) {
        while (this.elDiagnosticsMessages.childElementCount > 0) {
            this.elDiagnosticsMessages.removeChild(this.elDiagnosticsMessages.lastChild);
        }

        if (port == 'usb') {
            if (typeof navigator.usb !== 'undefined') {
                this.device = new _preprocessor_webusb(this);
            }
        }
        else {
            this.device = new _preprocessor_uart(this);
        }

        if (this.device) {
            this.device.init(port, baudrate);
        }

        this.reset_ui();
    }
}
