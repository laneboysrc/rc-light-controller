var disassembler = (function() {
	"use strict";

	var MAX_NUMBER_OF_INSTRUCTIONS = 16 * 1024 / 4;

	var asm = new Array(MAX_NUMBER_OF_INSTRUCTIONS);
	var labels = {};
	var next_label_index = 1;

	var offset = 0;
	var var_offset = 0;
	var pc = 0;

	var STATE_PRIORITY = 0;
	var STATE_RUN = 1;
	var STATE_LEDS_USED = 2;
	var STATE_PROGRAM = 3;
	var STATE_END_OF_PROGRAM = 4;
	var STATE_IGNORE = 5;

	var state  = STATE_PRIORITY;

	var opcodes = {
		"GOTO": 0x01,
		"SET": 0x02,
		"SET_I": 0x03,
		"FADE": 0x04,
		"FADE_I": 0x05,
		"WAIT": 0x06,
		"WAIT_I": 0x07,
		"ASSIGN": 0x10,
		"ASSIGN_I": 0x11,
		"ADD": 0x12,
		"ADD_I": 0x13,
		"SUBTRACT": 0x14,
		"SUBTRACT_I": 0x15,
		"MULTIPLY": 0x16,
		"MULTIPLY_I": 0x17,
		"DIVIDE": 0x18,
		"DIVIDE_I": 0x19,
		"AND": 0x1a,
		"AND_I": 0x1b,
		"OR": 0x1c,
		"OR_I": 0x1d,
		"XOR": 0x1e,
		"XOR_I": 0x1f,
		"END_OF_PROGRAM": 0xfe,
		"END_OF_PROGRAMS": 0xff
	};

	var INSTRUCTION_MODIFIER_LED = 0x02000000;
	var INSTRUCTION_MODIFIER_IMMEDIATE = 0x01000000;

	var PARAMETER_TYPE_VARIABLE = 0;
	var PARAMETER_TYPE_LED = 1;
	var PARAMETER_TYPE_RANDOM = 2;
	var PARAMETER_TYPE_STEERING = 3;
	var PARAMETER_TYPE_THROTTLE = 4;

    var RUN_WHEN_NORMAL_OPERATION 			= 0;
    var RUN_WHEN_NO_SIGNAL 					= (1 << 0);
    var RUN_WHEN_INITIALIZING               = (1 << 1);
    var RUN_WHEN_SERVO_OUTPUT_SETUP_CENTRE  = (1 << 2);
    var RUN_WHEN_SERVO_OUTPUT_SETUP_LEFT    = (1 << 3);
    var RUN_WHEN_SERVO_OUTPUT_SETUP_RIGHT   = (1 << 4);
    var RUN_WHEN_REVERSING_SETUP_STEERING   = (1 << 5);
    var RUN_WHEN_REVERSING_SETUP_THROTTLE   = (1 << 6);
    var RUN_WHEN_GEAR_CHANGED               = (1 << 7);

    var RUN_WHEN_LIGHT_SWITCH_POSITION_0  	= (1 << 0);
    var RUN_WHEN_LIGHT_SWITCH_POSITION_1 	= (1 << 1);
    var RUN_WHEN_LIGHT_SWITCH_POSITION_2 	= (1 << 2);
    var RUN_WHEN_LIGHT_SWITCH_POSITION_3 	= (1 << 3);
    var RUN_WHEN_LIGHT_SWITCH_POSITION_4 	= (1 << 4);
    var RUN_WHEN_LIGHT_SWITCH_POSITION_5 	= (1 << 5);
    var RUN_WHEN_LIGHT_SWITCH_POSITION_6 	= (1 << 6);
    var RUN_WHEN_LIGHT_SWITCH_POSITION_7 	= (1 << 7);
    var RUN_WHEN_LIGHT_SWITCH_POSITION_8 	= (1 << 8);
    var RUN_WHEN_NEUTRAL                 	= (1 << 9);
    var RUN_WHEN_FORWARD                 	= (1 << 10);
    var RUN_WHEN_REVERSING               	= (1 << 11);
    var RUN_WHEN_BRAKING                 	= (1 << 12);
    var RUN_WHEN_INDICATOR_LEFT          	= (1 << 13);
    var RUN_WHEN_INDICATOR_RIGHT         	= (1 << 14);
    var RUN_WHEN_HAZARD                  	= (1 << 15);
    var RUN_WHEN_BLINK_FLAG              	= (1 << 16);
    var RUN_WHEN_BLINK_LEFT              	= (1 << 17);
    var RUN_WHEN_BLINK_RIGHT             	= (1 << 18);
    var RUN_WHEN_WINCH_DISABLERD         	= (1 << 19);
    var RUN_WHEN_WINCH_IDLE              	= (1 << 20);
    var RUN_WHEN_WINCH_IN                	= (1 << 21);
    var RUN_WHEN_WINCH_OUT               	= (1 << 22);
    var RUN_WHEN_GEAR_1                  	= (1 << 23);
    var RUN_WHEN_GEAR_2                  	= (1 << 24);

    var RUN_ALWAYS                       	= (1 << 31);


	// *************************************************************************
	var decode_priority_run_condition = function (instruction) {
		if (instruction == RUN_WHEN_NORMAL_OPERATION) {
			return;
		}

		if (instruction & RUN_WHEN_NO_SIGNAL) {
			asm[offset++]['decleration'] = "run when no-signal";
		}
		if (instruction & RUN_WHEN_INITIALIZING) {
			asm[offset++]['decleration'] = "run when initializing";
		}
		if (instruction & RUN_WHEN_SERVO_OUTPUT_SETUP_CENTRE) {
			asm[offset++]['decleration'] = "run when servo-ouput-setup-centre";
		}
		if (instruction & RUN_WHEN_SERVO_OUTPUT_SETUP_LEFT) {
			asm[offset++]['decleration'] = "run when servo-ouput-setup-left";
		}
		if (instruction & RUN_WHEN_SERVO_OUTPUT_SETUP_RIGHT) {
			asm[offset++]['decleration'] = "run when servo-ouput-setup-right";
		}
		if (instruction & RUN_WHEN_REVERSING_SETUP_STEERING) {
			asm[offset++]['decleration'] = "run when reversing_setup_steering";
		}
		if (instruction & RUN_WHEN_REVERSING_SETUP_THROTTLE) {
			asm[offset++]['decleration'] = "run when reversing_setup_throttle";
		}
		if (instruction & RUN_WHEN_GEAR_CHANGED) {
			asm[offset++]['decleration'] = "run when gear-changed";
		}
	};


	// *************************************************************************
	var decode_run_condition = function (instruction) {
		if (instruction == 0) {
			return;
		}

		if (instruction & RUN_ALWAYS) {
			asm[offset++]['decleration'] = "run always";
			return;
		}

		if (instruction & RUN_WHEN_LIGHT_SWITCH_POSITION_0) {
			asm[offset++]['decleration'] = "run when light-switch-position0";
		}
		if (instruction & RUN_WHEN_LIGHT_SWITCH_POSITION_1) {
			asm[offset++]['decleration'] = "run when light-switch-position1";
		}
		if (instruction & RUN_WHEN_LIGHT_SWITCH_POSITION_2) {
			asm[offset++]['decleration'] = "run when light-switch-position2";
		}
		if (instruction & RUN_WHEN_LIGHT_SWITCH_POSITION_3) {
			asm[offset++]['decleration'] = "run when light-switch-position3";
		}
		if (instruction & RUN_WHEN_LIGHT_SWITCH_POSITION_4) {
			asm[offset++]['decleration'] = "run when light-switch-position4";
		}
		if (instruction & RUN_WHEN_LIGHT_SWITCH_POSITION_5) {
			asm[offset++]['decleration'] = "run when light-switch-position5";
		}
		if (instruction & RUN_WHEN_LIGHT_SWITCH_POSITION_6) {
			asm[offset++]['decleration'] = "run when light-switch-position6";
		}
		if (instruction & RUN_WHEN_LIGHT_SWITCH_POSITION_7) {
			asm[offset++]['decleration'] = "run when light-switch-position7";
		}
		if (instruction & RUN_WHEN_LIGHT_SWITCH_POSITION_8) {
			asm[offset++]['decleration'] = "run when light-switch-position8";
		}
		if (instruction & RUN_WHEN_NEUTRAL) {
			asm[offset++]['decleration'] = "run when neutral";
		}
		if (instruction & RUN_WHEN_FORWARD) {
			asm[offset++]['decleration'] = "run when forward";
		}
		if (instruction & RUN_WHEN_REVERSING) {
			asm[offset++]['decleration'] = "run when reversing";
		}
		if (instruction & RUN_WHEN_BRAKING) {
			asm[offset++]['decleration'] = "run when braking";
		}
		if (instruction & RUN_WHEN_INDICATOR_LEFT) {
			asm[offset++]['decleration'] = "run when indicator-left";
		}
		if (instruction & RUN_WHEN_INDICATOR_RIGHT) {
			asm[offset++]['decleration'] = "run when indicator-right";
		}
		if (instruction & RUN_WHEN_HAZARD) {
			asm[offset++]['decleration'] = "run when hazard";
		}
		if (instruction & RUN_WHEN_BLINK_FLAG) {
			asm[offset++]['decleration'] = "run when blink-flag";
		}
		if (instruction & RUN_WHEN_BLINK_LEFT) {
			asm[offset++]['decleration'] = "run when blink-left";
		}
		if (instruction & RUN_WHEN_BLINK_RIGHT) {
			asm[offset++]['decleration'] = "run when blink-right";
		}
		if (instruction & RUN_WHEN_WINCH_DISABLERD) {
			asm[offset++]['decleration'] = "run when winch-disabled";
		}
		if (instruction & RUN_WHEN_WINCH_IDLE) {
			asm[offset++]['decleration'] = "run when winch-idle";
		}
		if (instruction & RUN_WHEN_WINCH_IN) {
			asm[offset++]['decleration'] = "run when winch-in";
		}
		if (instruction & RUN_WHEN_WINCH_OUT) {
			asm[offset++]['decleration'] = "run when winch-out";
		}
		if (instruction & RUN_WHEN_GEAR_1) {
			asm[offset++]['decleration'] = "run when gear-1";
		}
		if (instruction & RUN_WHEN_GEAR_2) {
			asm[offset++]['decleration'] = "run when gear-2";
		}
	};


	// *************************************************************************
	var decode_leds_used = function (instruction) {
		var i;
		var any_led = false;

		for (i = 0; i < 32; i++) {
			if (instruction & (1 << i)) {
				var type = (i < 16) ? 'master' : 'slave';

				asm[offset++]['decleration'] =
					"led led" + i + " = " + type + "[" + (i % 16) + "]";
				any_led = true;
			}
		}

		if (any_led){
			asm[offset++]['decleration'] = '';	// Empty line
		}
	};


	// *************************************************************************
	var decode_leds = function (instruction) {
		var stop = (instruction & 0x00ff0000) >> 16;
		var start = (instruction & 0x0000ff00) >> 8;
		var result = '';

		while (start <= stop) {
			if (result != '') {
				result += ', ';
			}
			result += 'led' + start++;
		}

		return result;
	};


	// *************************************************************************
	var decode_variable_assignment = function (instruction) {
		var parameter_type;
		var index;

		if (instruction & INSTRUCTION_MODIFIER_IMMEDIATE) {
			// FIXME: handle negative numbers properly
			// FIXME: add hexadecimal as comment?!
			var number = instruction & 0xffff;
			var decimal = number;
			if (decimal >= 0x8000) {
				decimal = -0x10000 + decimal;
			}
			return '' + decimal + " // 0x" + number.toString(16);
		}

		parameter_type = (instruction >> 8) & 0xff;
		index = instruction & 0xff;

		switch (parameter_type) {
			case PARAMETER_TYPE_VARIABLE:
				if (index == 0) {
					return "clicks";
				}
				return "var" + index;

			case PARAMETER_TYPE_LED:
				return "led" + index;

			case PARAMETER_TYPE_RANDOM:
				return "random";

			case PARAMETER_TYPE_STEERING:
				return "steering";

			case PARAMETER_TYPE_THROTTLE:
				return "throttle";

			default:
				return "ERROR: unknown parameter type " + parameter_type
		}
	};


	// *************************************************************************
	var process_opcode = function (opcode, instruction) {
		switch (opcode) {
			case opcodes['END_OF_PROGRAM']:
				return STATE_END_OF_PROGRAM;

			case opcodes['GOTO']:
				var address = (instruction & 0xffffff);

				if (asm[offset + address]['label'] == null) {
					asm[offset + address]['label'] = 'label' + address;
				}

				asm[offset + pc++]['code'] = 'goto ' + asm[offset + address]['label'];
				break;

			case opcodes['SET']:
				asm[offset + pc++]['code'] = 'set ' +
					decode_leds(instruction) + ' var' + (instruction & 0xff);
				break;

			case opcodes['SET_I']:
				asm[offset + pc++]['code'] = 'set ' +
					decode_leds(instruction) + ' ' + (instruction & 0xff);
				break;

			case opcodes['WAIT']:
				asm[offset + pc++]['code'] = 'wait var' + (instruction & 0xff);
				break;

			case opcodes['WAIT_I']:
				asm[offset + pc++]['code'] = 'wait ' + (instruction & 0xffff);
				break;

			case opcodes['FADE']:
				asm[offset + pc++]['code'] = 'fade ' +
					decode_leds(instruction) + ' var' + (instruction & 0xff);
				break;

			case opcodes['FADE_I']:
				asm[offset + pc++]['code'] = 'fade ' +
					decode_leds(instruction) + ' ' + (instruction & 0xff);
				break;

			case opcodes['ASSIGN']:
			case opcodes['ASSIGN_I']:
				asm[offset + pc++]['code'] =
					'var' + ((instruction & 0x00ff0000) >> 16) + ' = ' +
						decode_variable_assignment(instruction);
				break;

			case opcodes['ADD']:
			case opcodes['ADD_I']:
				asm[offset + pc++]['code'] =
					'var' + ((instruction & 0x00ff0000) >> 16) + ' += ' +
						decode_variable_assignment(instruction);
				break;

			case opcodes['SUBTRACT']:
			case opcodes['SUBTRACT_I']:
				asm[offset + pc++]['code'] =
					'var' + ((instruction & 0x00ff0000) >> 16) + ' -= ' +
						decode_variable_assignment(instruction);
				break;

			case opcodes['MULTIPLY']:
			case opcodes['MULTIPLY_I']:
				asm[offset + pc++]['code'] =
					'var' + ((instruction & 0x00ff0000) >> 16) + ' *= ' +
						decode_variable_assignment(instruction);
				break;

			case opcodes['DIVIDE']:
			case opcodes['DIVIDE_I']:
				asm[offset + pc++]['code'] =
					'var' + ((instruction & 0x00ff0000) >> 16) + ' /= ' +
						decode_variable_assignment(instruction);
				break;

			case opcodes['AND']:
			case opcodes['AND_I']:
				asm[offset + pc++]['code'] =
					'var' + ((instruction & 0x00ff0000) >> 16) + ' &= ' +
						decode_variable_assignment(instruction);
				break;

			case opcodes['OR']:
			case opcodes['OR_I']:
				asm[offset + pc++]['code'] =
					'var' + ((instruction & 0x00ff0000) >> 16) + ' |= ' +
						decode_variable_assignment(instruction);
				break;

			case opcodes['XOR']:
			case opcodes['XOR_I']:
				asm[offset + pc++]['code'] =
					'var' + ((instruction & 0x00ff0000) >> 16) + ' ^= ' +
						decode_variable_assignment(instruction);
				break;

			default:
				asm[offset + pc++]['code'] =
					'TODO 0x' + (instruction & 0xffffffff).toString(16);
				break

		}

		return STATE_PROGRAM;
	};


	// *************************************************************************
	var process_instruction = function (instruction) {
		var opcode = (instruction >> 24) & 0xff;
		// console.log(opcode.toString(16));

		switch (state) {
			case STATE_PRIORITY:
				decode_priority_run_condition(instruction);
				state = STATE_RUN;
				break;

			case STATE_RUN:
				decode_run_condition(instruction);
				asm[offset++]['decleration'] = '';	// Empty line
				state = STATE_LEDS_USED;
				break;

			case STATE_LEDS_USED:
				decode_leds_used(instruction);
				var_offset = offset;
				state = STATE_PROGRAM;
				break;

			case STATE_PROGRAM:
				state = process_opcode(opcode, instruction);
				break;

			case STATE_END_OF_PROGRAM:
				if (opcode == opcodes['END_OF_PROGRAMS']) {
					state = STATE_IGNORE;
				}
				else {
					// Add variables

					offset += pc;
					pc = 0;


					asm[offset++]['decleration'] = '';	// Empty line
					asm[offset++]['decleration'] = '__NEXT_PROGRAM__';
					asm[offset++]['decleration'] = '';	// Empty line

					decode_priority_run_condition(instruction);
					state = STATE_RUN;
				}
				break;

			case STATE_IGNORE:
				break;
		}
	};


	// *************************************************************************
	var disassemble = function (instructions) {
		var i;

		for (i = 0; i < asm.length; i++) {
			asm[i] = {'decleration' : null, 'label' : null, 'code' : null};
		}

		state = STATE_PRIORITY;

		instructions.forEach(function (instruction) {
			process_instruction(instruction);
		});


		var source_code = ""

		for (i = 0; i < (offset + pc); i++) {
			if (asm[i]['decleration'] != null) {
				source_code += asm[i]['decleration'] + "\n";
			}
			if (asm[i]['label'] != null) {
				source_code += asm[i]['label'] + ":\n";
			}
			if (asm[i]['code'] != null) {
				source_code += "    " + asm[i]['code'] + "\n";
			}
		}

		return source_code;
	};

	return {
		disassemble: disassemble
	}
})();

