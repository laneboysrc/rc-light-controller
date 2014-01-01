.PHONY: all
all: subaru-impreza-2008 nismo-coppermix lancia-delta-integrale generic \
     datsun-240z-rally bmw-2002-turbo porsche-911-rsr wraith datsun-510-master \
     ben-landrover pajero subaru wyetuck-landrover phil-ford xr311 dingo


##############################################################################
# Generic light logic, not specific for any car
##############################################################################
.PHONY: generic
generic: generic-momentary-master generic-two-position-master generic-always-on-master

.PHONY: generic-momentary-master
generic-momentary-master: hw_tlc5940_16f1825.inc
	$(eval CFLAGS = -D LIGHT_MODE_MASK=0x0f -D CH3_MOMENTARY -D ENABLE_STEERING_WHEEL_SERVO -D ENABLE_SERVO_OUTPUT) 
	$(eval files = master servo-reader servo-output utils lights-generic)
	$(build)

.PHONY: generic-two-position-master
generic-two-position-master: hw_tlc5940_16f1825.inc
	$(eval CFLAGS = -D LIGHT_MODE_MASK=0x0f -D ENABLE_STEERING_WHEEL_SERVO -D ENABLE_SERVO_OUTPUT) 
	$(eval files = master servo-reader servo-output utils lights-generic)
	$(build)

.PHONY: generic-always-on-master
generic-always-on-master: hw_tlc5940_16f1825.inc
	$(eval CFLAGS = )
	$(eval files = master servo-reader utils lights-always-on)
	$(build)


##############################################################################
# Tamiya Subaru Impreza WRC 2008 (hatchback)
##############################################################################
.PHONY: subaru-impreza-2008
subaru-impreza-2008: subaru-impreza-2008-master

.PHONY: subaru-impreza-2008-master
subaru-impreza-2008-master: hw_tlc5940_16f1825.inc
	$(eval CFLAGS = -D LIGHT_MODE_MASK=0x03 -D ENABLE_UART) 
	$(eval files = master uart-reader utils lights-subaru-impreza-2008)
	$(build)


##############################################################################
# Tamiya Nismo Coppermix Silvia
##############################################################################
.PHONY: nismo-coppermix
nismo-coppermix: hkr3000-preprocessor nismo-coppermix-master

.PHONY: nismo-coppermix-master
nismo-coppermix-master: hw_tlc5940_16f1825.inc
	$(eval CFLAGS = -D LIGHT_MODE_MASK=0x03 -D ENABLE_UART) 
	$(eval files = master uart-reader utils lights-nismo-coppermix)
	$(build)


##############################################################################
# Tamiya Lancia Delta Integrale
##############################################################################
.PHONY: lancia-delta-integrale
lancia-delta-integrale: hkr3000-preprocessor lancia-delta-integrale-master

.PHONY: lancia-delta-integrale-master
lancia-delta-integrale-master: hw_tlc5940_16f1825.inc
	$(eval CFLAGS = -D LIGHT_MODE_MASK=0x01 -D ENABLE_UART)
	$(eval files = master uart-reader utils lights-lancia-delta-integrale)
	$(build)


##############################################################################
# Tamiya Datsun 240Z Rally
##############################################################################
.PHONY: datsun-240z-rally
datsun-240z-rally: hkr3000-preprocessor datsun-240z-rally-master

.PHONY: datsun-240z-rally-master
datsun-240z-rally-master: hw_tlc5940_16f1825.inc
	$(eval CFLAGS = -D LIGHT_MODE_MASK=0x03 -D ENABLE_UART)
	$(eval files = master uart-reader utils lights-datsun-240z-rally)
	$(build)


##############################################################################
# HPI BMW 2002 turbo
##############################################################################
.PHONY: bmw-2002-turbo
bmw-2002-turbo: hkr3000-preprocessor bmw-2002-turbo-master

.PHONY: bmw-2002-turbo-master
bmw-2002-turbo-master: hw_tlc5940_16f1825.inc
	$(eval CFLAGS = -D LIGHT_MODE_MASK=0x01 -D DISABLE_BRAKE_DISARM_TIMEOUT -D ENABLE_UART)
	$(eval files = master uart-reader utils lights-bmw-2002-turbo)
	$(build)


##############################################################################
# HPI Porsche 911 RSR
##############################################################################
.PHONY: porsche-911-rsr
porsche-911-rsr: hkr3000-preprocessor porsche-911-rsr-master

.PHONY: porsche-911-rsr-master
porsche-911-rsr-master: hw_tlc5940_16f1825.inc
	$(eval CFLAGS = -D LIGHT_MODE_MASK=0x03 -D DISABLE_BRAKE_DISARM_TIMEOUT -D ENABLE_UART)
	$(eval files = master uart-reader utils lights-porsche-911-rsr)
	$(build)


##############################################################################
# Patrick's Wraith (pre-processor. gearbox, winch control)
##############################################################################
.PHONY: wraith
wraith: hkr3000-preprocessor wraith-master

.PHONY: wraith-master
wraith-master: hw_tlc5940_16f1825.inc
	$(eval CFLAGS = -D LIGHT_MODE_MASK=0x03 -D ESC_FORWARD_REVERSE -D ENABLE_GEARBOX -D ENABLE_SERVO_OUTPUT -D SERVO_OUTPUT_ON_THROTTLE -D ENABLE_WINCH -D ENABLE_UART)
	$(eval files = master uart-reader servo-output utils lights-wraith)
	$(build)


##############################################################################
# HPI Datsun 510 (M-chassis using a pre-processor in the Rx)
##############################################################################
.PHONY: datsun-510-master
datsun-510-master: hw_tlc5940_16f1825.inc
	$(eval CFLAGS = -D LIGHT_MODE_MASK=0x07 -D DISABLE_BRAKE_DISARM_TIMEOUT -D ENABLE_UART)
	$(eval files = master uart-reader utils lights-datsun-510)
	$(build)

	
##############################################################################
# Ben's Land Rover D90
##############################################################################
.PHONY: ben-landrover
ben-landrover: ben-landrover-preprocessor ben-landrover-master

.PHONY: ben-landrover-preprocessor
ben-landrover-preprocessor : hw_preprocessor_12f1840.inc
	$(eval CFLAGS = )
	$(eval files = preprocessor servo-reader utils)
	$(build)

.PHONY: ben-landrover-master
ben-landrover-master: hw_tlc5940_16f1825.inc
	$(eval CFLAGS = -D LIGHT_MODE_MASK=0x07 -D CH3_MOMENTARY -D ENABLE_UART)
	$(eval files = master uart-reader utils lights-ben-landrover)
	$(build)


##############################################################################
# Pajero of a GMail user (request from 2013-02-04)
##############################################################################
.PHONY: pajero
pajero: pajero-master

.PHONY: pajero-master
pajero-master : hw_original_servo_reader.inc
	$(eval CFLAGS = -D RECEIVER_OUTPUT_RATE=20 -D LIGHT_MODE_MASK=0x0f)
	$(eval files = master servo-reader utils lights-pajero)
	$(build)


##############################################################################
# Subaru of a GMail user (request from 2013-01-30)
##############################################################################
.PHONY: subaru
subaru: subaru-master

.PHONY: subaru-master
subaru-master : hw_original_servo_reader.inc
	$(eval CFLAGS = -D RECEIVER_OUTPUT_RATE=20 -D LIGHT_MODE_MASK=0x07)
	$(eval files = master servo-reader utils lights-subaru)
	$(build)


##############################################################################
# Wyetuck's Land Rover Defender S2 SWB (all metal Landy)
##############################################################################
.PHONY: wyetuck-landrover
wyetuck-landrover: gt3b-preprocessor wyetuck-landrover-master

.PHONY: wyetuck-landrover-master
wyetuck-landrover-master: hw_tlc5940_16f1825.inc
	$(eval CFLAGS = -D LIGHT_MODE_MASK=0x03 -D ENABLE_UART)
	$(eval files = master uart-reader utils lights-wyetuck-landrover)
	$(build)


##############################################################################
# Phil's FORD trailing truck
##############################################################################
.PHONY: phil-ford
phil-ford: gt3b-preprocessor phil-ford-master

.PHONY: phil-ford-master
phil-ford-master: hw_tlc5940_16f1825.inc
	$(eval CFLAGS = -D LIGHT_MODE_MASK=0x07 -D ESC_FORWARD_REVERSE -D ENABLE_UART)
	$(eval files = master uart-reader utils lights-phil-ford)
	$(build)


##############################################################################
# AXIAL SCX10 Dingo
##############################################################################
.PHONY: dingo
dingo: dingo-master dingo-slave

.PHONY: dingo-master
dingo-master: hw_original_servo_reader.inc
	$(eval CFLAGS = -D ENABLE_STEERING_WHEEL_SERVO)
	$(eval files = master servo-reader utils lights-dingo)
	$(build)

.PHONY: dingo-slave
dingo-slave : hw_original_uart_reader.inc
	$(eval CFLAGS = -D ENABLE_SERVO_OUTPUT)
	$(eval files = slave servo-output utils)
	$(build)


##############################################################################
# TAMIYA XR311
##############################################################################
.PHONY: xr311
xr311: xr311-preprocessor xr311-master

.PHONY: xr311-preprocessor
xr311-preprocessor : hw_preprocessor_16f628a.inc
	$(eval CFLAGS = -D SEQUENTIAL_CHANNEL_READING)
	$(eval files = preprocessor servo-reader utils)
	$(build)

.PHONY: xr311-master
xr311-master : hw_original_uart_reader.inc
	$(eval CFLAGS = -D DUAL_TLC5916 -D ENABLE_STEERING_WHEEL_SERVO -D ENABLE_SERVO_OUTPUT -D LIGHT_MODE_MASK=0x03 -D ENABLE_UART)
	$(eval files = master uart-reader servo-output utils lights-xr311)
	$(build)
	

##############################################################################
# Pre-processor for the HobbyKing HKR3000 receiver
##############################################################################
.PHONY: hkr3000-preprocessor
hkr3000-preprocessor : hw_preprocessor_12f1840.inc
	$(eval CFLAGS = -D SEQUENTIAL_CHANNEL_READING )
	$(eval files = preprocessor servo-reader utils)
	$(build)


##############################################################################
# Pre-processor for the GT3B receiver
##############################################################################
.PHONY: gt3b-preprocessor
gt3b-preprocessor : hw_preprocessor_12f1840.inc
	$(eval CFLAGS = -D SEQUENTIAL_CHANNEL_READING -D CHANNEL_SEQUENCE_TH_ST_CH3)
	$(eval files = preprocessor servo-reader utils)
	$(build)


##############################################################################
# Test code for hardware validation and debugging
##############################################################################
.PHONY: test
test: test-tlc5940-16f1825

.PHONY: test-tlc5940-16f1825
test-tlc5940-16f1825: hw_tlc5940_16f1825.inc
	$(eval CFLAGS = -D ENABLE_UART)
	$(eval files = master uart-reader utils lights-test-tlc5940-16f1825)
	$(build)


	
##############################################################################
##############################################################################
##############################################################################
# Internal functions, do not modify!
#
#
# Architecture of this makefile
# -----------------------------
# The RC light controller is based on various modules that allow different
# configurations. The original configuration was a master-slave concept,
# but it has been intended with the pre-processor concept that puts a 
# small micro-controller into the receiver, connecting with a single wire
# to the light controller master. Also different functions are configurable,
# such as whether a steering wheel output is available, support for different
# LED drivers, and possibly more in the future.
# 
# This quickly lead to the problem of maintaining the makefile. It became
# unwieldly especially during refactoring the source code.
# 
# The solution was that each hex file is built from just three configuration
# options:
# 
#   - The name of the io_*.inc file, specifying the hardware and chip type
#   - The defines that switch features through conditional compilation
#   - The list of "modules" that are needed
# 
# To make this work two important features of GNU Make were needed:
# 
#   - The $(eval x) function is used to set variables from within recipes
# 
#     Example: $(eval CFLAGS = -D DUAL_TLC5916 -D ENABLE_SERVO_OUTPUT)  
# 
# 
#   - Calling the make file recursively to execute the build of an individual
#     component given the variables set in the recipe for the component.
#
#     $(MAKE) var1=value1 var2=... target
#
#
#
##############################################################################
##############################################################################
##############################################################################



# The following code is a "macro" that recursivly involes this makefile for
# building a  single component (= a master, a slave, or a pre-processor).
#
# It sets variables 'CFLAGS', 'prefix', 'hw' and 'objects' for the new instance
# of make and asks to build the target $@.hex, which translates into 
# <component>.hex
#
# 'hw' is set to the first dependency of the component, which must be the
#  hw_*.inc file specifying the hardware of the component.
#
# 'objects' is transformed from the list of source files (without extension)
# required for the module by prepending the component name, a dash, and 
# appending the .o file extension. 
#
# So if you build component 'xr311-slave' then a list of files 
#
#    rc-light-controller utils
#
# will translate into 'objects' being 
#
#    xr311-slave-rc-light-controller.o xr311-slave-utils.o
build = @$(MAKE) CFLAGS="$(CFLAGS)" prefix="$@" hw="$<" objects="$(patsubst %,$@-%.o,$(files))" $@.hex 

$(prefix).hex : $(objects) 
	gplink -m -O 2 -o $@ $^

$(prefix)-%.o : %.asm $(hw)
	@cp $(hw) hw.tmp
	gpasm -c $(CFLAGS) -o $@ $<
	@$(RM) hw.tmp

clean :
	$(RM) *.hex
	$(RM) *.lst
	$(RM) *.map
	$(RM) *.cod
	$(RM) *.o
	$(RM) *.tmp
	
