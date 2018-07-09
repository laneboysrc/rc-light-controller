nw.Window.open('build/configurator.html', {}, function (new_win) {
    new_win.showDevTools();
    new_win.on('focus', function() {
        console.log('New window is focused');
    });

    new_win.eval(null, 'console.log("Hello world");');
});

