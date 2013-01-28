import time

### {{{ http://code.activestate.com/recipes/134892/ (r2)
#class _Getch:
#    """Gets a single character from standard input.  Does not echo to the
#screen."""
#    def __init__(self):
#        try:
#            self.impl = _GetchWindows()
#        except ImportError:
#            self.impl = _GetchUnix()

#    def __call__(self): return self.impl()


#class _GetchUnix:
#    def __init__(self):
#        import tty, sys

#    def __call__(self):
#        import sys, tty, termios
#        fd = sys.stdin.fileno()
#        old_settings = termios.tcgetattr(fd)
#        try:
#            tty.setraw(sys.stdin.fileno())
#            ch = sys.stdin.read(1)
#        finally:
#            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
#        return ch


#class _GetchWindows:
#    def __init__(self):
#        import msvcrt

#    def __call__(self):
#        import msvcrt
#        return msvcrt.getch()


#getch = _Getch()
### end of http://code.activestate.com/recipes/134892/ }}}

try:
    from msvcrt import kbhit, getch
except ImportError:
    import termios, fcntl, sys, os, tty
    def kbhit():
        fd = sys.stdin.fileno()
        oldterm = termios.tcgetattr(fd)
        newattr = termios.tcgetattr(fd)
        newattr[3] = newattr[3] & ~termios.ICANON & ~termios.ECHO
        termios.tcsetattr(fd, termios.TCSANOW, newattr)
        oldflags = fcntl.fcntl(fd, fcntl.F_GETFL)
        fcntl.fcntl(fd, fcntl.F_SETFL, oldflags | os.O_NONBLOCK)
        try:
            while True:
                try:
                    c = sys.stdin.read(1)
                    return c
                except IOError:
                    return False
        finally:
            termios.tcsetattr(fd, termios.TCSAFLUSH, oldterm)
            fcntl.fcntl(fd, fcntl.F_SETFL, oldflags)

    def getch():
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        try:
            tty.setraw(sys.stdin.fileno())
            ch = sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        return ch
            
count = 0
while True:
    c = kbhit()
    if c:
        if ord(c) == 113:
            break
        print "%d" % (	ord(c), ) 
    else:
        time.sleep(20/1000)
        print "\x08\x08%s" % ("-\|/"[count/10%4], ),
        count += 1
    
