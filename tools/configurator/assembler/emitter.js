var emitter = (function () {
    "use strict";

    var pc = 0;

    var hex = function (number) {
        var s = number.toString(16).toUpperCase();
        while (s.length < 8) {
            s = "0" + s;
        }

        return "0x" + s;
    }

    var emit_run_condition = function (priority_run_condition, run_condition) {
        console.log("emit_run_condition(): " + hex(priority_run_condition) + " " + hex(run_condition));
    }

    var emit_end_of_program = function () {
        console.log("emit_end_of_program()");
        //pc = 0;
    }

    var emit = function (code) {
        console.log("emit(): " + hex(code) +" pc=" + pc);
        ++pc;
    }


    var get_pc = function () {
        return pc;
    };

    return {
        emit: emit,
        emit_run_condition: emit_run_condition,
        emit_end_of_program: emit_end_of_program,
        pc: get_pc
    };

})();