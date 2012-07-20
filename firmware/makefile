dingo: master-dingo.hex

slave: rc-light-controller-slave.hex

master-dingo.hex: rc-light-controller.o light-table-dingo.o
	gplink -o master-dingo rc-light-controller.o light-table-dingo.o

light-table-dingo.o: light-table-dingo.asm
	gpasm -c -p pic16f628a light-table-dingo.asm

rc-light-controller.o: rc-light-controller.asm
	gpasm -c rc-light-controller.asm

rc-light-controller-slave.hex: rc-light-controller-slave.asm
	gpasm rc-light-controller-slave.asm



clean:
	rm -f master-dingo.hex
	rm -f master-dingo.lst
	rm -f master-dingo.cod
	rm -f rc-light-controller.lst
	rm -f rc-light-controller.o
	rm -f light-table-dingo.lst
	rm -f light-table-dingo.o
	rm -f rc-light-controller-slave.hex
	rm -f rc-light-controller-slave.lst
	rm -f rc-light-controller-slave.cod

