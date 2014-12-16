/*jslint vars: true */

var logger = (function () {
    "use strict";

    var log_levels = {
        DEBUG: 0,
        INFO: 1,
        WARNING: 2,
        ERROR: 3,
        FATAL: 4
    };

    var log_level = log_levels.DEBUG;

    var log_level_strings = ["DEBUG  ", "INFO   ", "WARNING", "ERROR  ", "FATAL  "];

    var log_function;


    // *************************************************************************
    var console_output = function (msg) {
        console.log(msg);
    };


    // *************************************************************************
    var stderr_output = function (msg) {
        process.stderr.write(msg + "\n");
    };


    // *************************************************************************
    var log = function (module, level, message) {
        level = isNaN(level) ? log_levels[level] || 0 : level;

        if (level < log_level) {
            return;
        }

        while (module.length < 8) {
            module += " ";
        }

        log_function(module + " [" + log_level_strings[level] + "] " + message);
    };


    // *************************************************************************
    var set_log_level = function (level) {
        level = isNaN(level) ? log_levels[level] || 0 : level;
        log_level = level;
    };


    // *************************************************************************

    if (typeof process !== "undefined" &&
            process.stderr !== undefined &&
                process.stderr.write !== undefined) {
        log_function = stderr_output;
    } else {
        log_function = console_output;
    }

    return {
        log: log,
        set_log_level: set_log_level
    };

}());


// node.js exports; hide from browser where exports is undefined and use strict
// would trigger.
if (typeof exports !== "undefined") {
    exports.logger = logger;
}
