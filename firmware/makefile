.PHONY: dingo
dingo: dingo-master dingo-slave


.PHONY: dingo-master
dingo-master: dingo-master.hex

dingo-master.hex: dingo-master.o dingo-light-table.o
	gplink -m -o dingo-master dingo-master.o dingo-light-table.o

dingo-master.o: rc-light-controller.asm io_original.inc
	cp io_original.inc io_master.tmp
	gpasm -c -o dingo-master rc-light-controller.asm
	rm -f io_master.tmp

dingo-light-table.o: dingo-light-table.asm
	gpasm -c -p pic16f628a dingo-light-table.asm


.PHONY: dingo-slave
dingo-slave: dingo-slave.hex

dingo-slave.hex: dingo-slave.o
	gplink -m -o dingo-slave dingo-slave.o

dingo-slave.o: rc-light-controller-slave.asm io_original.inc
	cp io_original.inc io_slave.tmp
	gpasm -c -D DUAL_TLC5916 -o dingo-slave rc-light-controller-slave.asm
	rm -f io_slave.tmp



.PHONY: xr311
xr311: xr311-master xr311-slave


.PHONY: xr311-master
xr311-master: xr311-master.hex

xr311-master.hex: xr311-master.o
	gplink -m -o xr311-master xr311-master.o
	
xr311-master.o: rc-light-controller.asm io_preprocessor.inc
	cp io_preprocessor.inc io_master.tmp
	gpasm -c -D PREPROCESSING_MASTER -D SEQUENTIAL_CHANNEL_READING -o xr311-master rc-light-controller.asm
	rm -f io_master.tmp


.PHONY: xr311-slave
xr311-slave: xr311-slave.hex

xr311-slave.hex: xr311-slave.o xr311-light-table.o
	gplink -m -o xr311-slave xr311-slave.o xr311-light-table.o
	
xr311-slave.o: rc-light-controller-intelligent-slave.asm io_original.inc
	cp io_original.inc io_master.tmp
	gpasm -c -o xr311-slave rc-light-controller-intelligent-slave.asm
	rm -f io_master.tmp
	
xr311-light-table.o: xr311-light-table.asm
	gpasm -c -p pic16f628a xr311-light-table.asm


clean:
	rm -f *.hex
	rm -f *.lst
	rm -f *.map
	rm -f *.cod
	rm -f *.o
	rm -f *.tmp

