.PHONY: all
all: phil-ford xr311 dingo


##############################################################################
# Phil's FORD trailing truck
##############################################################################
.PHONY: phil-ford
phil-ford: phil-ford-preprocessor phil-ford-master

phil-ford-master: io_phil-ford.inc
	$(eval CFLAGS = )
	$(eval files = rc-light-controller-intelligent-slave utils phil-ford-light-table)
	$(build)

.PHONY: phil-ford-preprocessor
phil-ford-preprocessor : io_preprocessor_12f1840.inc
	$(eval CFLAGS = -D PREPROCESSING_MASTER -D SEQUENTIAL_CHANNEL_READING )
	$(eval files = rc-light-controller utils)
	$(build)


##############################################################################
# AXIAL SCX10 Dingo
##############################################################################
.PHONY: dingo
dingo: dingo-master dingo-slave

.PHONY: dingo-master
dingo-master: io_original.inc
	$(eval CFLAGS = )
	$(eval files = rc-light-controller utils dingo-light-table)
	$(build)

.PHONY: dingo-slave
dingo-slave : io_original.inc
	$(eval CFLAGS = -D ENABLE_SERVO_OUTPUT)
	$(eval files = rc-light-controller-slave utils)
	$(build)


##############################################################################
# TAMIYA XR311
##############################################################################
.PHONY: xr311
xr311: xr311-preprocessor xr311-master

.PHONY: xr311-preprocessor
xr311-preprocessor : io_preprocessor_16f628a.inc
	$(eval CFLAGS = -D PREPROCESSING_MASTER -D SEQUENTIAL_CHANNEL_READING)
	$(eval files = rc-light-controller utils)
	$(build)

.PHONY: xr311-master
xr311-master : io_original.inc
	$(eval CFLAGS = -D DUAL_TLC5916 -D ENABLE_SERVO_OUTPUT)
	$(eval files = rc-light-controller-intelligent-slave utils xr311-light-table)
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
# It sets variables 'CFLAGS', 'prefix', 'io' and 'objects' for the new instance
# of make and asks to build the target $@.hex, which translates into 
# <component>.hex
#
# 'io' is set to the first dependency of the component, which must be the
#  io_*.inc file specifying the hardware of the component.
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
build = @$(MAKE) CFLAGS="$(CFLAGS)" prefix="$@" io="$<" objects="$(patsubst %,$@-%.o,$(files))" $@.hex 

$(prefix).hex : $(objects) 
	gplink -m -O 2 -o $@ $^

$(prefix)-%.o : %.asm $(io)
	@cp $(io) io.tmp
	gpasm -c $(CFLAGS) -o $@ $<
	@$(RM) io.tmp

clean :
	$(RM) *.hex
	$(RM) *.lst
	$(RM) *.map
	$(RM) *.cod
	$(RM) *.o
	$(RM) *.tmp
	
