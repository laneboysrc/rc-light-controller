'use strict';

var gui = require('nw.gui');

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
    });


}

chrome.app.runtime.onLaunched.addListener(startConfigurator);
