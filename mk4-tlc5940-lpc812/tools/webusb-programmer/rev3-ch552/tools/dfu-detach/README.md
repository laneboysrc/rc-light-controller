# chprog
Here is yet another CH55x programmer with v1 and v2 bootloader detection and support.

This one combines code from other projects: 
* vnproch551: https://github.com/NgoHungCuong/vnproch551
  
  used for v2.3 and v2.4 bootloader support
* libre551: https://github.com/rgwan/librech551
  
  used for v1.0 bootloader support
* added autodetection routine of V1 and V2 bootloaders

  No need to specify bootloader version or protocol - chprog should
  figure it out.

Supported are Write, Verify and Reset for all supported bootloader versions:
1.0, 2.3.1, 2.4.0

Tested on:
* CH552T, BL: 1.0, 2.3.1, 2.4.0
* CH551G, BL: 2.3.1

Usage is quite simple:
<pre>
sudo ./chprog firmware.bin
</pre>

Make sure your device is connected to USB port on your PC. The bootloader has to be
activated by pressing the booloader button (or by shorting the bootloader pins/pads)
on your dev board during power-on.
