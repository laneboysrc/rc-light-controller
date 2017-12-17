(function() {
  'use strict';

  document.addEventListener('DOMContentLoaded', event => {
    let connectButton = document.querySelector("#connect");
    let statusDisplay = document.querySelector('#status');

    function connect() {
      port.connect().then(() => {
        statusDisplay.textContent = '';
        connectButton.textContent = 'Disconnect';

        port.onReceive = data => {
          let textDecoder = new TextDecoder();
          console.log(textDecoder.decode(data));
        }
        port.onReceiveError = error => {
          console.error(error);
        };
      }, error => {
        statusDisplay.textContent = error;
      });
    }


    connectButton.addEventListener('click', function() {
      // if (port) {
      //   port.disconnect();
      //   connectButton.textContent = 'Connect';
      //   statusDisplay.textContent = '';
      //   port = null;
      // } else {
      //   serial.requestPort().then(selectedPort => {
      //     port = selectedPort;
      //     connect();
      //   }).catch(error => {
      //     statusDisplay.textContent = error;
      //   });
      // }

      const filters = [
        { 'vendorId': 0x6666, 'productId': 0xcab1 }
      ];


      navigator.usb.requestDevice({ 'filters': filters }).then(
        device => {
          device.open().then(() => {
            console.log(device.configuration);
            if (device.configuration === null) {
              return device.selectConfiguration(1);
            }
          })

          // .then(() => device.claimInterface(2))
          // .then(() => this.device_.selectAlternateInterface(2, 0))
          // .then(() => device.controlTransferOut({
          //     'requestType': 'class',
          //     'recipient': 'interface',
          //     'request': 0x22,
          //     'value': 0x01,
          //     'index': 0x02}))
          // .then(() => {
          //   readLoop();
          // })
          .catch(e => {
            console.error(e);
          });

        let readLoop = () => {
          device.transferIn(5, 64).then(result => {
            console.log(result.data);
            readLoop();
          }, error => {
            console.error(error);
          });
        };
      });


    });

    navigator.usb.getDevices().then(devices => {
      devices.map(device => console.log);
    })
    .catch(e => {
      console.error(e);
    });
    // serial.getPorts().then(ports => {
    //   if (ports.length == 0) {
    //     statusDisplay.textContent = 'No device found.';
    //   } else {
    //     statusDisplay.textContent = 'Connecting...';
    //     port = ports[0];
    //     connect();
    //   }
    // });
  });
})();
