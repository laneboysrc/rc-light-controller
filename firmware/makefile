.PHONY: phil-ford
phil-ford: phil-ford-master phil-ford-slave



.PHONY: phil-ford-master
phil-ford-master: phil-ford-master.hex

phil-ford-master.hex: phil-ford-master.o phil-ford-master-utils.a
	gplink -m -O 2 -o phil-ford-master phil-ford-master.o phil-ford-master-utils.a
	
phil-ford-master.o: rc-light-controller.asm io_preprocessor_12f1840.inc
	cp io_preprocessor_12f1840.inc io.tmp
	gpasm -c -D INCLUDE_CONFIG -D PREPROCESSING_MASTER -D SEQUENTIAL_CHANNEL_READING -o phil-ford-master rc-light-controller.asm
	rm -f io.tmp

phil-ford-master-utils.o: utils.asm io_preprocessor_12f1840.inc
	cp io_preprocessor_12f1840.inc io.tmp
	gpasm -c -o phil-ford-master-utils utils.asm
	rm -f io.tmp
	

.PHONY: phil-ford-slave
phil-ford-slave: phil-ford-slave.hex

phil-ford-slave.hex: phil-ford-slave.o phil-ford-light-table.o phil-ford-slave-utils.a
	gplink -m -O 2 -o phil-ford-slave phil-ford-slave.o phil-ford-light-table.o phil-ford-slave-utils.a
	
phil-ford-slave.o: rc-light-controller-intelligent-slave.asm io_phil-ford.inc
	cp io_phil-ford.inc io.tmp
	gpasm -c -D INCLUDE_CONFIG -o phil-ford-slave rc-light-controller-intelligent-slave.asm
	rm -f io.tmp
	
phil-ford-light-table.o: phil-ford-light-table.asm io_phil-ford.inc
	cp io_phil-ford.inc io.tmp
	gpasm -c phil-ford-light-table.asm
	rm -f io.tmp

phil-ford-slave-utils.o: utils.asm io_preprocessor_12f1840.inc
	cp io_original.inc io.tmp
	gpasm -c -o phil-ford-slave-utils utils.asm
	rm -f io.tmp


.PHONY: dingo
dingo: dingo-master dingo-slave


.PHONY: dingo-master
dingo-master: dingo-master.hex

dingo-master.hex: dingo-master.o dingo-light-table.o dingo-master-utils.a
	gplink -m -O 2 -o dingo-master dingo-master.o dingo-light-table.o dingo-master-utils.a

dingo-master.o: rc-light-controller.asm io_original.inc
	cp io_original.inc io.tmp
	gpasm -c -D INCLUDE_CONFIG -o dingo-master rc-light-controller.asm
	rm -f io.tmp

dingo-light-table.o: dingo-light-table.asm io_original.inc
	cp io_original.inc io.tmp
	gpasm -c dingo-light-table.asm
	rm -f io.tmp

dingo-master-utils.o: utils.asm io_original.inc
	cp io_original.inc io.tmp
	gpasm -c -o dingo-master-utils utils.asm
	rm -f io.tmp
	

.PHONY: dingo-slave
dingo-slave: dingo-slave.hex

dingo-slave.hex: dingo-slave.o dingo-slave-utils.a
	gplink -m -O 2 -o dingo-slave dingo-slave.o dingo-slave-utils.a

dingo-slave.o: rc-light-controller-slave.asm io_original.inc
	cp io_original.inc io.tmp
	gpasm -c -D INCLUDE_CONFIG -D ENABLE_SERVO_OUTPUT -o dingo-slave rc-light-controller-slave.asm
	rm -f io.tmp

dingo-slave-utils.o: utils.asm io_preprocessor_12f1840.inc
	cp io_original.inc io.tmp
	gpasm -c -o dingo-slave-utils utils.asm
	rm -f io.tmp


.PHONY: xr311
xr311: xr311-master xr311-slave


.PHONY: xr311-master
xr311-master: xr311-master.hex

xr311-master.hex: xr311-master.o xr311-master-utils.a
	gplink -m -O 2 -o xr311-master xr311-master.o xr311-master-utils.a
	
xr311-master.o: rc-light-controller.asm io_preprocessor_16f628a.inc
	cp io_preprocessor_16f628a.inc io.tmp
	gpasm -c -D INCLUDE_CONFIG -D PREPROCESSING_MASTER -D SEQUENTIAL_CHANNEL_READING -o xr311-master rc-light-controller.asm
	rm -f io.tmp

xr311-master-utils.o: utils.asm io_preprocessor_16f628a.inc
	cp io_preprocessor_16f628a.inc io.tmp
	gpasm -c  -o xr311-master-utils utils.asm
	rm -f io.tmp


#io_slave = io_original.inc

.PHONY: xr311-slave
xr311-slave : io_slave = io_original.inc
xr311-slave : xr311-slave.hex 

xr311-slave.hex : xr311-slave.o xr311-light-table.o xr311-slave-utils.a
	gplink -m -O 2 -o xr311-slave xr311-slave.o xr311-light-table.o xr311-slave-utils.a
	rm -f io.tmp
	
xr311-slave.o : rc-light-controller-intelligent-slave.asm io.tmp 
	gpasm -c -D INCLUDE_CONFIG -D DUAL_TLC5916 -D ENABLE_SERVO_OUTPUT -o xr311-slave rc-light-controller-intelligent-slave.asm
	
xr311-light-table.o : xr311-light-table.asm io.tmp 
	gpasm -c -o xr311-light-table.o xr311-light-table.asm

xr311-slave-utils.o : utils.asm io.tmp
	gpasm -c -o xr311-slave-utils utils.asm

$(io_slave) :
	io_slave = io_original.inc

io.tmp : $(io_slave)
	cp $(io_slave) io.tmp


clean :
	rm -f *.hex
	rm -f *.lst
	rm -f *.map
	rm -f *.cod
	rm -f *.o
	rm -f *.a
	rm -f *.tmp


%.a : %.o
	gplib -c $@ $<

