.PHONY: all
all: preprocessor-v2


#############################################################################
.PHONY: preprocessor-v2
preprocessor-v2: preprocessor-v2.hex

preprocessor-v2.hex: preprocessor-v2.o servo-reader.o uart-output.o
	sdcc --use-non-free -mpic14 -p12f1840 -o $@ $^


#############################################################################
.c.o:
	sdcc --use-non-free -mpic14 -p12f1840 -c $<


#############################################################################
clean :
	$(RM) *.hex
	$(RM) *.lst
	$(RM) *.map
	$(RM) *.cod
	$(RM) *.o
	$(RM) *.asm
	
