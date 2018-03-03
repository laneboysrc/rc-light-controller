(function() {
  'use strict';

  document.addEventListener('DOMContentLoaded', async event => {
    let connectButton = document.querySelector("#connect");
    let statusDisplay = document.querySelector('#status');

    connectButton.addEventListener('click', async function() {
      const filters = [
        { 'vendorId': 0x6666 }
      ];

      let device;
      try {
        device = await navigator.usb.requestDevice({ 'filters': filters });
      }
      catch (e) {
        console.log("requestDevice(): " + e);
      }

      if (device) {
        connect(device)
      }
    });

    let devices = await navigator.usb.getDevices();
    console.log(devices);
    let device = devices[0];
    if (device) {
      connect(device)
    }
  });

  async function connect(device) {
    console.log(device);
    await device.open();
    if (device.configuration === null) {
      await device.selectConfiguration(1);
    }
    // await device.claimInterface(0);
    console.log("Device ready!");

    let data = string2arraybuffer('Hello world!\n');

    const setup = {
      "requestType": "vendor",
      "recipient": "device",
      "request": 72,
      "value": 0,
      "index": 0
    };

    let result = await device.controlTransferOut(setup, data);
    if (result.status == "ok") {
      console.log("OK " + result.bytesWritten);
    }
    else {
      console.log("FAIL " + result.staus);
    }
    // await device.close();
  }

  function string2arraybuffer(str) {
    var buf = new ArrayBuffer(str.length);
    var bufView = new Uint8Array(buf);
    for (var i=0, strLen=str.length; i<strLen; i++) {
      bufView[i] = str.charCodeAt(i);
    }
    return buf;
  }

})();
