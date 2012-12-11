.PHONY: dingo
dingo: dingo-master dingo-slave


.PHONY: dingo-master
dingo-master: dingo-master.hex

dingo-master.hex: dingo-master.o dingo-light-table.o
	gplink -m -o dingo-master dingo-master.o dingo-light-table.o

dingo-master.o: rc-light-controller.asm
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
	gpasm -c -o dingo-slave rc-light-controller-slave.asm
	rm -f io_slave.tmp



clean:
	rm -f *.hex
	rm -f *.lst
	rm -f *.map
	rm -f *.cod
	rm -f *.o
	rm -f *.tmp

