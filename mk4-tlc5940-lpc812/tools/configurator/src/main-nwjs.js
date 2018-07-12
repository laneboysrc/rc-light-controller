'use strict';

var gui = require('nw.gui');
var https = require('https');

function getFromGithub(console) {
    https.get('https://laneboysrc.github.io/rc-light-controller/', (res) => {
        const { statusCode } = res;
        // const contentType = res.headers['content-type'];

        let error;
        if (statusCode !== 200) {
            error = new Error(`Request Failed Status Code: ${statusCode}`);
        }

        if (error) {
            console.log(error.message);
            // Consume response data to free up memory
            res.resume();
            return;
        }

        res.setEncoding('utf8');
        let rawData = '';
        res.on('data', (chunk) => { rawData += chunk; });
        res.on('end', () => {
            console.log(rawData);
        });
    }).on('error', (e) => {
        console.log(`Got error: ${e.message}`);
    });
}

function startConfigurator() {
    const windowOptions = {
        id: 'configurator',
        frame: 'chrome',
        innerBounds: {
            minWidth: 640,
            minHeight: 360
        }
    };

    chrome.app.window.create('build/configurator.html', windowOptions, function (appWindow) {

        // Example of how to access console.log
        // appWindow.contentWindow.console.log(appWindow);

        const nwWindow = appWindow.contentWindow.nw.Window.get();

        // Show the Chrome DevTools
        // FIXME: only do this in debug mode!
        nwWindow.showDevTools();

        // Listen to the new window event and open them in the default browser
        // configured for the operating system
        nwWindow.on('new-win-policy', function (frame, url, policy) {
            gui.Shell.openExternal(url);
            policy.ignore();
        });


        getFromGithub(appWindow.contentWindow.console);
    });


}

chrome.app.runtime.onLaunched.addListener(startConfigurator);
