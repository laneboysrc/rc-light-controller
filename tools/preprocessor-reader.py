import serial 

s = serial.Serial('/dev/ttyUSB0', 38400)

while True:
    sync = 0x00    
    while sync != 0x87:
        sync = ord(s.read(1))
        if sync != 0x87:
            print "Out of sync: %x" % sync
    
    st = ord(s.read(1))
    th = ord(s.read(1))
    ch3 = ord(s.read(1))
    
    if st > 127:
        st = -(256 - st)
    if th > 127:
        th = -(256 - th)
    

    print "%+04d %+04d %d" % (st, th, ch3)    
