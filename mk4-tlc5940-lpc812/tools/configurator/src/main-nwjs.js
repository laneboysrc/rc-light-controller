// nw.Window.open('build/configurator.html', {}, function (new_win) {
//     new_win.showDevTools();
//     new_win.on('focus', function() {
//         console.log('New window is focused');
//         new_win.eval(null, 'console.log("New window is focused EVAL");');
//     });

//     new_win.eval(null, 'console.log("Hello world");');
// });

// console.log('Hi there from boot.js');

'use strict';

function start_configurator() {
    const window_options = {
        id: 'configurator',
        frame: 'chrome',
        innerBounds: {
            minWidth: 640,
            minHeight: 360
        }
    };

    console.log('start_configurator');

    chrome.app.window.create('build/configurator.html', window_options, function (app_window) {

        console.log(app_window);
        app_window.contentWindow.console.log(app_window);

        const nw_window = app_window.contentWindow.nw.Window.get();
        app_window.contentWindow.console.log(nw_window);
        nw_window.showDevTools();
        nw_window.window.console.log('console.log from nw_window.window');
    });


}

chrome.app.runtime.onLaunched.addListener(start_configurator);
