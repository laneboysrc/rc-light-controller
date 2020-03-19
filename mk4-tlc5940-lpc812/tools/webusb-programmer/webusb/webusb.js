(function() {
  'use strict';


  function string2arraybuffer(str) {
    var buf = new ArrayBuffer(str.length);
    var bufView = new Uint8Array(buf);
    for (var i=0, strLen=str.length; i<strLen; i++) {
      bufView[i] = str.charCodeAt(i);
    }
    return buf;
  }


  async function connect(device) {
    console.log("connect()");
    console.log(device);
    try {
      await device.open();
    }
    catch (e) {
      console.log("device.open() failed: ", e);
      return;
    }

    if (device.configuration === null) {
      await device.selectConfiguration(1);
    }
    // await device.claimInterface(0);
    console.log("Device ready!");


    const setup = {
      "requestType": "vendor",
      "recipient": "device",
      "request": 72,
      "value": 0,
      "index": 0
    };


    console.log("Sending control transfer ...");
    // let data = string2arraybuffer('Hello world!\n');
    // let result = await device.controlTransferOut(setup, data);
    let result = await device.controlTransferOut(setup);
    if (result.status == "ok") {
      console.log("device.controlTransferOut() OK: " + result.bytesWritten + " Bytes written");
    }
    else {
      console.log("device.controlTransferOut() FAIL: " + result.staus);
    }

    console.log("controlTransferIn() ...");
    setup.value = 1;
    let controlIn = await device.controlTransferIn(setup, 1);
    console.log(controlIn);
    console.log(controlIn.data.getUint8(0));

    // await device.close();
  }

  async function init() {
    if (window.location.protocol != 'https:') {
      if (window.location.protocol != 'http:' || window.location.hostname != 'localhost') {
        document.querySelector('#protocol-error').classList.remove('hidden');
      }
    }

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
          console.log("requestDevice() failed: " + e);
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

  }

  init();
})();
