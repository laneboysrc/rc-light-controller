'use strict';

var preprocessor = (function () {

    var throttle = 0;
    var steering = 0;
    var ch3 = 0;
    var sendingThrottle = false;
    var sendingSteering = false;
    var startupMode = false;

    var elWebusbConnect;
    var elNotConnected;
    var elStartupMode;
    var elSteering;
    var elThrottle;
    var elCH3;
    var elMomentary;
    var elSteeringNeutral;
    var elThrottleNeutral;
    var elResponse;
    var elDiagnostics;
    var elUART;

    var send_function;

    var webusb_device;
    var websocket;

    const SLAVE_MAGIC_BYTE = 0x87;

    // Pressing any of those keys triggers CH3
    const CH3_KEYS = '3acux';

    const VENDOR_ID = 0x6666;
    const TEST_INTERFACE = 1;
    const TEST_EP_IN = 1;
    const TEST_EP_OUT = 2;
    const EP_SIZE = 64;

    const MAX_UART_LOG_LINES = 20;

    var center_steering = function (event) {
        event.preventDefault();
        elSteering.value = 0;
        sendSteering(0);
    }

    var center_throttle = function (event) {
        event.preventDefault();
        elThrottle.value = 0;
        sendThrottle(0);
    }

    var steering_changed = function (event) {
        event.preventDefault();
        sendSteering(parseInt(elSteering.value));
    }

    var throttle_changed = function (event) {
        event.preventDefault();
        sendThrottle(parseInt(elThrottle.value));
    }

    var ch3_changed = function (event) {
        event.preventDefault();
        sendCh3(this);
    }

    var startup_mode_changed = function () {
        sendStartup(elStartupMode.checked);
    }

    // Moving the throttle slider causes a lot of update events. Sending
    // all those requests to the rc-sound-module is quite rude to both
    // the software, the web browser and the network.
    //
    // So instead we build a function that only sends the value if it has
    // changed, and it waits for the last transfer to have finished before
    // sending the next.
    // We achieve this with flags (global variable sendingThrottle) and a
    // local callback function that gets called after the XHR request has
    // completed (successful or not).
    //
    // Of course we have to ensure that the last throttle value correctly
    // gets sent to the rc-sound-module.
    var sendThrottle = function (value) {
        var sentThrottle;       // Holds the last throttle value we sent

        // Local callback function, called when the XHR has completed
        function cb() {
            sendingThrottle = false;
            // If the value we sent has changed in the meantime we trigger
            // another transmission to ensure the most recent value is
            // sent to the rc-sound-module
            if (sentThrottle != throttle) {
                sendThrottleCommand();
            }
        }

        // This local function sends the actual throttle command. It sets
        // the "transmission in progress flag sendingThrottle, and stores
        // the sent value for later comparison in the callback function.
        function sendThrottleCommand() {
            sendingThrottle = true;
            sentThrottle = throttle;
            send_function('TH', throttle, cb);
        }

        // Ignore multiple events of the same throttle value
        if (value == throttle) {
            return;
        }
        throttle = value;

        // Trigger sending only if no throttle transmission is already ongoing.
        // If a transmission is in progress the callback function will
        // ensure that the latest throttle value is sent to the
        // rc-sound-module.
        if (!sendingThrottle) {
            sendThrottleCommand();
        }
    }

    var sendSteering = function (value) {
        var sentSteering;       // Holds the last steering value we sent

        // Local callback function, called when the XHR has completed
        function cb() {
            sendingSteering = false;
            // If the value we sent has changed in the meantime we trigger
            // another transmission to ensure the most recent value is
            // sent to the rc-sound-module
            if (sentSteering != steering) {
                sendSteeringCommand();
            }
        }

        // This local function sends the actual throttle command. It sets
        // the "transmission in progress flag sendingThrottle, and stores
        // the sent value for later comparison in the callback function.
        function sendSteeringCommand() {
            sendingSteering = true;
            sentSteering = steering;
            send_function('ST', steering, cb);
        }


        // Ignore multiple events of the same throttle value
        if (value == steering) {
            return;
        }
        steering = value;

        // Trigger sending only if no throttle transmission is already ongoing.
        // If a transmission is in progress the callback function will
        // ensure that the latest throttle value is sent to the
        // rc-sound-module.
        if (!sendingSteering) {
            sendSteeringCommand();
        }
    }

    var sendCh3 = function (action) {
        if (elMomentary.checked) {
            if (action == 'down') {
                ch3 = 1;
                send_function('CH3', ch3);
            }
            if (action == 'up') {
                ch3 = 0;
                send_function('CH3', ch3);
            }
        }
        else {
            if (action == 'click') {
                ch3 = ch3 ? 0 : 1;
                send_function('CH3', ch3);
            }
        }
        return false;
    }

    var sendStartup = function (mode) {
        if (mode) {
            elSteeringNeutral.click();
            elThrottleNeutral.click();
        }
        startupMode = mode ? 1 : 0;
        send_function('STARTUP_MODE', startupMode);
    }

    var reset_ui = function () {
        startupMode = true;
        steering = 0;
        throttle = 0;
        ch3 = 0;

        elSteering.value = steering;
        elThrottle.value = throttle;
        elStartupMode.checked = startupMode;

        send_function('ST', steering);
        send_function('TH', throttle);
        send_function('CH3', ch3);
        sendStartup(startupMode);
    }

    var send_ping = function () {
        let pingTimer;

        function ping_callback(msg) {
            if (msg != 'OK') {
                elNotConnected.style.visibility = 'visible';
            }
            else {
                if (elNotConnected.style.visibility == 'visible') {
                    reset_ui();
                }
                elNotConnected.style.visibility = 'hidden';
            }

            if (pingTimer) {
                clearTimeout(pingTimer);
            }
            pingTimer = setTimeout(send_ping, 3000);
        }

        send_function('PING', 1, ping_callback);
    }

    let update_ui = send_ping;

    var keydown_event_handler = function (event) {
        const key = event.key.toLowerCase();
        if (CH3_KEYS.indexOf(key) >= 0) {
            sendCh3('click');
            sendCh3('down');
        }
        else if (key == 's') {
            elStartupMode.click();
        }
        return true;
    }

    var keyup_event_handler = function (event) {
        const key = event.key.toLowerCase();
        if (CH3_KEYS.indexOf(key) >= 0) {
            sendCh3('up');
        }
        return true;
    }

    var diagnostics = function (msg) {
        const lines = msg.trim().split('\n');
        for (msg of lines) {
            msg = new Date(Date.now()).toISOString() + ' ' + msg;

            console.log(msg);

            const el = document.createElement('div');
            el.textContent += msg;
            if (elUART.firstChild) {
                elUART.insertBefore(el, elUART.firstChild);
                while (elUART.childElementCount > MAX_UART_LOG_LINES) {
                    elUART.removeChild(elUART.lastChild);
                }
            }
            else {
                elUART.appendChild(el);
            }
        }
    }


    var send_webusb = function (cmd, value, callback) {
        if (callback) {
            callback(webusb_device ? 'OK' : 'NOT CONNECTED');
            // callback('OK');
        }
    }

    var webusb_connect = async function (device) {
        try {
            await device.open();
            if (device.configuration === null) {
                await device.selectConfiguration(1);
            }

            await device.claimInterface(TEST_INTERFACE);
        }
        catch (e) {
            console.error('Failed to open the device', e);
            return;
        }

        elResponse.textContent =
            'Connected to Light Controller with serial number ' + device.serialNumber;
        webusb_device = device;
        webusb_send_data();
        webusb_receive_data();
        update_ui();
    }

    var webusb_disconnect = async function () {
        if (webusb_device) {
            try {
                await webusb_device.close();
            }
            finally {
                webusb_device = undefined;
                elResponse.textContent = ' ';
            }
        }
        update_ui();
    }

    var webusb_send_data = async function () {
        let st = steering;
        if (st < 0) {
            st = 256 + st;
        }

        let th = throttle;
        if (th < 0) {
            th = 256 + th;
        }

        let last_byte = 0;
        if (ch3) {
            last_byte += 0x01;
        }
        if (startupMode) {
            last_byte += 0x10;
        }

        let data = new Uint8ClampedArray(4);
        data[0] = SLAVE_MAGIC_BYTE;
        data[1] = st;
        data[2] = th;
        data[3] = last_byte;

        try {
            let result = await webusb_device.transferOut(TEST_EP_OUT, data);
            if (result.status == 'ok') {
                setTimeout(webusb_send_data, 20);
            }
            else {
                console.error('transferOut() failed:', result.status);
            }
        }
        catch (e) {
            console.error('transferOut() exception:', e);
            return;
        }
    }

    var webusb_receive_data = async function () {
        while (true) {
            try {
                let result = await webusb_device.transferIn(TEST_EP_IN, EP_SIZE);
                if (result.status == 'ok') {
                    const decoder = new TextDecoder('utf-8');
                    const value = decoder.decode(result.data);
                    diagnostics(value);
                }
                else {
                    console.error('transferIn() failed:', result.status);
                }
            }
            catch (e) {
                console.error('transferIn() exception:', e);
                return;
            }
        }
    }

    var webusb_device_connected = function (connection_event) {
        const device = connection_event.device;
        console.log('USB device connected:', device);
        if (!webusb_device) {
            if (device && device.vendorId == VENDOR_ID) {
                webusb_connect(device);
            }
        }
    }

    var webusb_device_disconnected = function (connection_event) {
        console.log('USB device disconnected:', connection_event);
        const disconnected_device = connection_event.device;
        if (webusb_device &&  disconnected_device == webusb_device) {
            webusb_disconnect();
        }
    }

    var webusb_init = async function () {
        // Check if the browser supports WebUSB. If not, show a message
        // to the user.
        if (typeof navigator.usb === 'undefined') {
            document.getElementById('webusb-not-available').classList.remove('hidden');
        }
        else {
            elDiagnostics.classList.remove('hidden');

            navigator.usb.addEventListener('connect', webusb_device_connected);
            navigator.usb.addEventListener('disconnect', webusb_device_disconnected);

            let devices = await navigator.usb.getDevices();
            if (devices.length) {
                webusb_connect(devices[0]);
            }
            else {
                // Show the connect button
                elWebusbConnect.style.visibility = 'visible';
                elWebusbConnect.addEventListener('click', async (event) => {
                    event.preventDefault();
                    const options = {filters:[{vendorId: VENDOR_ID}]};
                    let device;
                    try {
                        device = await navigator.usb.requestDevice(options);
                        console.log(device);
                        if (device) {
                            await webusb_connect(device);
                            if (webusb_device) {
                                elWebusbConnect.style.visibility = 'hidden';
                            }
                        }
                    }
                    catch (e) {
                        console.log('requestDevice failed:' + e);
                    }
                });
            }
        }
    }

    var init = async function (port, baudrate) {
        elMomentary = document.getElementById('momentary');
        elWebusbConnect = document.getElementById('webusb-connect');
        elNotConnected = document.getElementById('not-connected');
        elStartupMode = document.getElementById('startup-mode');
        elSteering = document.getElementById('steering');
        elThrottle = document.getElementById('throttle');
        elCH3 = document.getElementById('ch3');
        elSteeringNeutral = document.getElementById('steering-neutral');
        elThrottleNeutral = document.getElementById('throttle-neutral');
        elResponse = document.getElementById('response');
        elUART = document.getElementById('uart');
        elDiagnostics = document.getElementById('diagnostics');

        if (port == 'usb') {
            send_function = send_webusb;
            webusb_init();
        }
        else {
            // send_function = send_xmlhttp;
        }

        document.addEventListener('keydown', keydown_event_handler);
        document.addEventListener('keyup', keyup_event_handler);
        elSteeringNeutral.addEventListener('click', center_steering);
        elThrottleNeutral.addEventListener('click', center_throttle);
        elSteering.addEventListener('input', steering_changed);
        elSteering.addEventListener('change', steering_changed);
        elThrottle.addEventListener('input', throttle_changed);
        elThrottle.addEventListener('change', throttle_changed);
        elCH3.addEventListener('click', ch3_changed.bind('click'));
        elCH3.addEventListener('mousedown', ch3_changed.bind('down'));
        elCH3.addEventListener('mouseup', ch3_changed.bind('up'));
        elStartupMode.addEventListener('change', startup_mode_changed);

        reset_ui();
        update_ui();

        if (typeof(chrome) !== 'undefined'  &&  chrome.serial) {
            chrome.serial.getDevices(ports => {
                ports.forEach(port => {
                    elResponse.textContent += port.path + ' ' + port.displayName;
                });
            });
        }
        else {
            elResponse.textContent = 'Serial ports are not supported';
        }
    };

    // *************************************************************************
    return {
        init: init
    };
}());
