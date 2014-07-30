# Hardware overview

Over time the DIY RC Light Controller hardware went through several iterations.

The original version was based on the PIC16F628A and TLC5916 light controller.
This system allowed to drive up to 8 LEDs with a constant current. More than
8 LEDs were supported through cascading multiple units.

It is not recommended to use this design for new developments as it is 
very limited. One of the biggest issues was that the LEDs were dimmed using
PWM, which caused flicker in videos.


## TLC5940 based design

An updated version that is still recommended today is based on the TLC5940 chip.
Also a more modern micro-controller PIC16F1825 is employed, which is both cheaper
and more powerful than the PIC16F628A.

The TLC5940 can drive up to 16 LEDs, which proved sufficient for all vehicles
we equipped with lights so far. 

Schematics can be found [here](../electronics/rc-light-controller/schematics-rc-light-controller-tlc5940). 
The source files in Eagle format are available too.

A PCB design is available, using SMD components. 
You can have your own PCB made using the [supplied Gerber files](../electronics/rc-light-controller-gerber-oshpark-rev3.zip) from companies
such as [OSH Park](https://www.oshpark.com/) and [Smart Prototyping](http://www.smart-prototyping.com/). 

It is also possible to build the light controller using a prototyping board
and DIL-sized components, which may be easier for hobbyists than SMD. Be 
careful: the TLC5940 has a different pin assignment between the DIL and TSSOP
(SMD) package variants!

[![Light controller on prototyping board and PCB](http://farm6.staticflickr.com/5321/9769284031_7576b9dbe0.jpg)](http://www.flickr.com/photos/78037110@N03/9769284031/)

This variant of the light controller can operate from a wide voltage range
of 5 to 10V. The LEDs can be driven from an even higher voltage of up to
20V, although heat generated in the TLC5940 will be a limiting factor.

LEDs are driven with a constant current, which can be individually programmed 
per LED output. Since the LEDs are driven with DC there is no flicker 
appearing on videos.

If the supply voltage allows, multiple LEDs that need to light up at the 
same time can be wired in series. For example, the red tail light LEDs can
be wired in series, so only one light output is used up. Naturally wiring 
becomes more complex.


## WS2812 / PL9823 based design

In recent years LEDs are appearing on the market that contain a controller chip.
Currently the [WS2812B](https://www.adafruit.com/products/1655) is highly 
popular. It is a RGB LED that contains a simple chip inside. The LED is
controlled through a single pin using a [serial shift-register-like 
protocol](http://cpldcpu.wordpress.com/2014/01/14/light_ws2812-library-v2-0-part-i-understanding-the-ws2812/). 
Multiple LEDs can be chained together; like a Christmas light.

The WS2812B is extremely cheap too. When ordered in 100 pcs quantity from 
China it is only slightly more expensive than a regular 5 mm white LED from
the local hobby shop.

This makes wiring up a vehicle very easy: one only needs to run 3 wires from
one LED to the next. Also the control electronics becomes simpler as only
a simple low pin-count device is needed. We have chosen the PIC12F1840 that
we already employed in other applications.

Schematics can be found [here](../electronics/rc-light-controller-ws2812-pic12f1840/schematics-rc-light-controller-ws2812-12f1840.pdf).  
A PCB layout (Eagle format) and [Gerber files](../electronics/rc-light-controller-gerber-ws2812-pic12f1840.zip) are available too.

The WS2812B are SMD LEDs in a 5 x 5 mm square case. This makes mounting them
in a traditional RC car light bucket, which is usually designed for 5 mm round 
dome style LEDs, inconvenient.

However, the [PL9823](http://www.aliexpress.com/item/PL9823-F5-5mm-round-hat-RGB-LED-with-PD9823-chipset-inside-full-color-frosted/1707175958.html)
has the same functionality as the WS2812B and comes in a standard 5 mm dome
encasing with four leads. The PL9823 is timing compatible with the WS2812B, 
so both LEDs can be mixed in the same string of lights.

The PL9823 has few downsides over the WS2812B:
- High power consumption of ~7-8mA even if the LED is off

- When power is applied the LEDs usually light up blue until they receive 
  valid data. The WS2812B stay off until data is received.

- Data format is red-green-blue , while WS2812 is green-red-blue. This can be 
  easily dealt with in software though.

In order to use either WS2812B or PL9823, each LED must have its own
bypass capacitor of 100nF. When using the WS2812 it is advisable to not buy
the bare LED, but rather the ones that come on a tiny circuit board of ~10 x 10 mm.
This board already contains the bypass capacitor and convenient terminals
for soldering.

Unfortunately no such board seems to exist for the PL9823. It is possible to 
solder wires directly onto the leads of the LEDs, but due to the narrow pitch it
is not fun. We have made a small break-out board, including bypass capacitor, 
ourself. The board layout is available in Eagle format.

A downside of the WS2812B or PL9823 based light controller is the narrow 
operating range. The WS2812B can operate between 3.5V to 5.3V, the PL9823
requires 4.5V to 6V. The PIC12F1840 can operate between 2.5V and 5.5V.  
This means that the light controller is best powered from a 5V BEC, or if the 
BEC operates at 6V a diode must be used to drop the voltage to the safe range.

The LEDs also do not tolerate any reverse voltage. When + and - are
accidentally swapped they burn out immediately. Don't ask how we know...

A great feature of the WS2812B or PL9823 based light controller is that
the color of each LED can be programmed individually. In our Rally Legends
Lancia Fulvia body shelll the rear indicators and reversing lights are 
driven by a single LED, but by programming the LED to output either white or orange
light we were still able to simulate reversing lights and indicators.

Unfortunately there does not seem to be any variant of the WS2812B or PL9823
that comes in a 3 mm dome factor, which is sometimes used in light buckets
of Tamiya or HPI. Maybe with such a body the TLC5940 based light controller 
will be a better choice.


## Pre-processor for simpler wiring

To utilize its full potential, the DIY RC Light Controller must be able to read 
Steering, Throttle and AUX (CH3) signals from the receiver. Since such a
combined signal is unfortunately not available on modern surface receivers.
Therefore the easiest way is to use a Y-cable on steering and throttle 
channels and feed all signals individually to the light controller. 

However, since the light controller usually is mounted on the body shell
and the receiver on the chassis, the Y-cable solution requires one to disconnect
three cables every time the body shell needs to be separated from the chassis.

To solve this issue we added a small micro-controller into our receivers.
This micro-controller directly taps into the servo outputs and generates
a serial signal that contains information of all channels. This way we need
to run only a single servo extension wire between the chassis and the body, 
providing both power and data. We use this system in most of our RC cars.

More information about the pre-processor:

- [http://laneboysrc.blogspot.sg/2012/12/pre-processor-for-diy-rc-light.html](http://laneboysrc.blogspot.sg/2012/12/pre-processor-for-diy-rc-light.html)

- [http://laneboysrc.blogspot.sg/2013/01/pre-processor-miniaturization.html](http://laneboysrc.blogspot.sg/2013/01/pre-processor-miniaturization.html)

Note that with the WS2812 based light controller one could mount the light 
controller on the chassis and have a single 3-pole wire running to the first 
LED in the chain.
Since we run different bodies on many of our chassis, each having different light
configurations, we still use the pre-processor with the WS2812B based controller.
Since the cost of the PIC12F1840 is less than S$ 2.00 this is not an issue.

