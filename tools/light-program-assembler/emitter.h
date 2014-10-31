#pragma once

#include <stdint.h>

#define PARAMETER_TYPE_VARIABLE 0
#define PARAMETER_TYPE_LED 1
#define PARAMETER_TYPE_RANDOM 2
#define PARAMETER_TYPE_STEERING 3
#define PARAMETER_TYPE_THROTTLE 4

#define INSTRUCTION_MODIFIER_LED 0x02000000
#define INSTRUCTION_MODIFIER_IMMEDIATE 0x01000000

#define MAX_LIGHT_PROGRAMS 25
#define MAX_LIGHT_PROGRAM_VARIABLES 100

// "Program Counter"
unsigned int pc;

void initialize_emitter(void);

void add_led_to_list(int led_index);
void emit(uint32_t instruction);
void emit_led_instruction(uint32_t instruction);
void emit_run_condition(uint32_t priority, uint32_t run);
void emit_end_of_program(void);

void output_programs(void);

