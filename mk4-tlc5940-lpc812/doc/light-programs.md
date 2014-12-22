# Light programs

Light programs are simple scripts that allow end-users to build custom light sequences that go beyond the fixed car related functions built into the MK4 TLC5940 LPC812 light controller.

Light programs can be triggered when the light controller goes into certain states of operation. For example, a light program can be written to flash a 3rd brake light whenever the brakes are enganged.

Light programs are entered into the corresponding edit field in the light controller *configurator.html*.

A total number of 20 light programs can exist at a certain time.

All light programs comprise of the following structure

    run conditions

    variable and led declerations

    statements (= the actual code)

    end

These constructs are described below in further detail.

> **Important**
>
> Every light program **must** end with an ``end`` statement. A new-line
> must be present after the ``end`` statement.


Here is an example of a light program:

    // ----------------------------------------------------------------------
    // Light up the main beam lights while the light controller is
    // initializing.
    //
    // Normally the main beam would be on only if the parking lights are
    // already on, so just the main beam lighting up is a unique
    // identifiaction.
    // ----------------------------------------------------------------------
    run when initializing

    use all leds
    led main-beam-l = led[2]
    led main-beam-r = led[3]

    loop:
        fade all leds stepsize 0
        all leds = 0%
        main-beam-l, main-beam-r = 100%
        sleep 0
        goto loop

    end

Light programs are line-based, meaning a single statement or decleration must be on the same source code line.

Everything from ``//`` until the end of the line is considered as comment

The ``run when`` line describes the condition that the light controller must be in for the program to run. In this case the light program runs when the light controller is starting up (i.e. waiting a short time until power up until it reads the center points for steering and throttle channels)

``use all leds`` declares that no other light controller function shall control any of the LED swhile this light program is running. The lines ``led ... = led[y]`` assign human readable names to light outputs 2 and 3.

This ends the decleration section.

The ``loop:`` statement defines a label that can be used to jump to from a ``goto`` instruction.

``fade all leds stepsize 0`` turns slowly fading LEDs in and out off for all LEDs, i.e. all LEDs turn on and off in an instant.

Then all LEDs are switched off, after which led[2] and led[3] are specifically set to fully on (100% brightness).

The light program pauses then for one 20 ms period (``sleep 0``), which causes the LED values we assigned to be actually executed and other light controller functions to run.
Without the sleep statement the light program would run for 50 internal instructions before being forcefully paused, which is unnecessary. After the 20 ms are over the ``goto`` statement is executed and the light program continues from the begin.

The following sections all elements of the light program language in detail.


## Comments and line continuation

Everything behind ``//`` or ``;`` until the end of the line is considered a *comment*.

Comments are useful for describing what the light program does. The comments are not stored into the light controller itself, but can be saved in the configurator.html through saving the current configuration.

Here is a valid light program showing various forms of comments:

    // This is a test to ensure the comment system works
    //

    // Another comment, with an empty line in front
    run always  // no-signal

        sleep 1     // Sleep for 1 ms. Actually will sleep for about 20ms...
    pos1:
        sleep 2     // Sleep for 2ms // Comment in comment
        goto forward_decleration
        ; Also semicolons can be used for comments
    pos2:               ; This is a comment at a label
        sleep 3
        goto pos1

    forward_decleration:
        sleep 4
    //pos2:
    ;  pos2:
        goto pos2

        end

While light programs are line-based, if the last character on a line is ``\`` the statement is assumed to be continued on the following line. This can be useful to prevent excessively long lines.

The following light program shows possible usage. It is not advisable to write light programs like this though:

    run \
       when\
        indicator-left or braking \
        or \        // Comment here, should not matter ...
        indicator-right blink-flag \

    // ^ That linefeed should have triggered!

        sleep 0
    pos1 :
    pos2 \
    :                  // Not very readable, but correct...
        sleep \
            2
            goto \
                pos1
        goto \
    pos2

        end


## Identifiers

Light programs can assign human readable names to variables, LEDs and labels. These are called identifiers.

Identifiers must start with an a character ``a..z`` or ``A..Z`` and continue with a number of alphanumeric characters, ``-`` and ``_``. Identifiers are case sensitive.

### Examples of valid identifiers

    testvariable
    var_with_underscore
    name-with-dash
    ThisOneHas1Number

### Examples of invalid identifiers

    3test       // does not start with a..zA..Z
    name&value  // & not allowed in identifiers
    _min        // does not start with a..zA..Z


## Run conditions

*Run conditions* define the state the light controller must be in for a light program to be executed. Each light program defines its own run condition. More than one light program may run at the same time.

There are several types of *run conditions*:

* Events

    Events are single-shot that can trigger execution of a light program. At the moment the only event is "gear changed". Events have the highest priority.

* Priority run conditions

    These run conditions take precendence over other run coditions.

* Run conditions

    Ordinary conditions like forward, braking, indicators, etc.

A light program can define multiple run conditions of the same type. For example, a light program can be run when the light controller is either *forward*  or *braking*.

However, a light program can not mix events, priority run conditions and run conditions.

Run conditions must be the first no-comment or empty lines in the light program.
The syntax is as follows:

    run always
    run when cond1
    run when cond1 cond2
    run when cond1 or cond2

``run always`` is a special condition that runs the light program all times.

``run when cond1`` describes a single condition that must be met for the light program to run.

``run when cond1 cond2`` and ``run when cond1 or cond2`` are identical and define that the light program shall be executed when *cond1* or *cond2* are met.


### Priority run conditions and events

- no-signal

    The light program runs when the light controller does not receive a valid servo input signal.

- initializing

    The light program runs after startup while the light controller waits before reading center points of steering and throttle channels.

- servo-output-setup-centre, servo-output-setup-left, servo-output-setup-right

    The light program runs when the respective setup function for the steering wheel servo or gearbox servo is triggered. Performing eight CH3-clicks starts servo setup. These run conditions can be used to drive the lights in a unique manner to guide the user through the servo setup.

- reversing-setup-steering, reversing-setup-throttle

    The light program runs when the servo reversing for the steering/throttle channel is engaged. Performing seven CH3-clicks starts steering and throttle reversing. These run conditions can be used to guide the user through the setup process.

- gear-changed

    This event fires whenever the gear is changed. It only applies when the light controller is configured to drive a 2-speed or 3-speed gearbox using a servo connected to the OUT/ISP ouptut. The run condition can be used to perform a short light animation, indicating to the user that the gear change occured.


### Run conditions

- light-switch-position-0 .. light-switch-position-8

    The light program runs when the virtual light switch, which is incremented by one CH3-click and decremented by two CH3-clicks, is in the given postion.

- neutral, forward, reversing, braking

    The light program runs when the thtottle is in neutral, the car is driving forward, reversing, or is braking. Neutral, forward and reversing are mutually exclusive. Braking may be active when in parallel with the other states.

- indicator-left, indicator-right

    The light program runs when the left/right indicator (turn signal) is active. The indicators are engaged by having throttle and steering in neutral for one second, and then moving the steering either left or right.

- hazard

    The light program runs when the hazard lights are active. The hazard lights can be toggled on/off with four CH3-clicks.

- blink-flag

    The light program runs during the bright period of the blink timer used for indicators and hazard lights. By default the blink frequency is 1.5 Hz (320 ms half-period during which the blink-flag is set).

- blink-left, blink-right

    The light program runs during the bright period of the respective left or right indicator light. Note that the indicator light may also be blinking due to the hazard light function, in which case both blink-left and blink-right will be active during the same time.

- winch-disabled, winch-idle, winch-in, winch-out

    The light program runs during the respective winch state. This applies if the light controller is configured to drive the [LANE Boys RC winch controller](https://github.com/laneboysrc/rc-winch-controller).
    The winch states are mutually exclusive.


## Declerations

The decleration section defines the LEDs and variables used by the light program and assigns human readable names to them.

Variable and LED declerations can appear in any order in the decleration section.

### Variable declerations

Variables are storage locations that hold numeric values. In total all light programs can utilize up to 100 variables.

Variables have a data type of *signed 16-bit integer*. This means that the range of numbers that can be stored is *-32768* to *32767*.

A variable is created by declaring its identifier:

    var ThisIsAVariable
    var another-one

Variables can also be used to exchange information between different light programs. These are called *global variables*. All programs that want to access a global variable must use the same identifier when declaring the variable.

Light program 1:

    global var i_am_global
    var but_i_am_local
    global var VARIABLE3

Light program 2:

    global var i_am_global
    var VARIABLE3

Light program 3:

    global var VARIABLE3

Light programs 1 and 2 share the global variable ``i_am_global``.
Light program 1 and 3 also declares a global variable ``VARIABLE3``. Light program 2 also declares ``VARIABLE3``, but as local variable, so in this example ``VARIABLE3`` of light program 2 is a separate, private storage location from the global ``VARIABLE3`` shared by light programs 1 and 3.


**FIXME** special variables clicks and light-switch-position


### LED declerations

LED declerations serve two purpose:

- They define which LEDs are used by the light program, preventing lower priority light programs and car functions to overwrite the LED brightness while the light program is running.

- They give human readable names to LEDs, which also makes it easy to swap LED outputs at a later time.

LEDs are declared as follows:

    use all leds
    led identifer = led[0]
    led slave_led_15 = led[31]
    led yet-another-led = led[1]

``use all leds`` gives the light program control of all LEDs. This is useful for light programs that intend to take over all LEDs during special run conditions such as ``initializing`` or ``no-signal``.

Assigning identifiers to individual LEDs follows the form ``led x = led[y]`` where ``x`` is the identifier and ``y`` is the number of the LED output of the light controller to use. For a single light controller the output number range is ``0..15``. The LEDs on a slave light controller range from ``16..31``.


## Statements

The actual light program function is described in a sequence of statements, which are described in this section.

### Statement arguments

Most statements support a variety of different arguments:

    var x
    led LED1a = led[8]

    x = 42          // Immedite decimal number
    x = -1          // Immedite negative decimal number
    sleep 0x14      // Immediate hexadecimal number
    sleep x         // Use value of a variable as sleep time in milliseconds
    x = LED1a       // Current value of led[8], range 0..100
    x = random      // A pseudo-random value between -32768 and 32767
    x = clicks      // Pre-defined global variable "clicks"
    x = light-switch-position  // Pre-defined global variable
    x = steering    // Steering channel (range: -100..100), read-only
    x = throttle    // Throttle channel (range: -100..100), read-only
    x = gear        // Current gear, read only,
                    //   only useful if gearbox servo support is enabled


### Assignments

Variables and LEDs can be assigned a value:

    var x
    var y
    led LED1a = led[8]

    x = 42          // Assign decimal number 42
    x = -1          // Assign decimal number -1 (variables are signed 16-bit)
    x = 0xbeef      // Assign hexadecimal number BEEF
    x = y           // Copy value of variable "y"
    x = LED1a       // Copy value of led[8] (range: 0..100)
    x = random      // Retrieve a random value
    x = clicks      // Copy value of global variable "clicks"
    x = steering    // Copy value of steering channel (range: -100..100)
    x = throttle    // Copy value of throttle channel (range: -100..100)

Assignments can also perform mathematical functions:

    x += 42         // x = x + 42
    x -= 0xcafe     // x = x - 0xcafe
    x *= 0xbabe
    x /= LED1a
    x &= y          // bit-wise AND
    x |= 0x80       // bit-wise OR
    x ^= 15         // bit-wise XOR

> **Division by zero**
>
> If a ``x /= y`` assignment is made where the divisor is 0 then the result
> is set to 32767 (largest possible integer value).


**FIXME** LEDs


### Labels

Labels are identifiers that mark locations in the light program that can be jumped to with the ``goto`` statement.

Labels must appear on their own line. They do not perform any activity nor do they consume memory. Labels comprise of an identifier followed by a ``:``.

Example:

    run always

        sleep 1
    pos1:
        sleep 2
        goto forward_decleration

    pos2:
        sleep 3
        goto pos1

    forward_decleration:
        sleep 4
        goto pos2

        end


### Goto

The ``goto`` statement redirects program execution to a different part of the light program. The parameter of the ``goto`` statement must be a label identifier that resides in the same light program.

> **Important**
>
> It is not possible to branch to a label in another light program.


### Sleep

The ``sleep`` statment suspends the execution of the light program for the given number of milliseconds. The resolution of timing is 20 milliseconds, which means that ``sleep 1`` causes a suspension for 20 ms rather than 1 ms.

It is only when a light program is suspended that the LED values assigned by the program are becoming into effect. It is therefore good practice to add ``sleep`` statements in loops of light programs.

Since the light contoller firmware is implemented using a *mainloop* and co-operative multi-tasking, light programs are forcefully suspended for one mainloop every 50 statements if no ``sleep`` statement is encountered during execution of the light program.


### Fade


### Conditional statements



## The ``end`` statement

Every light program **must** end with an ``end`` statement. A new-line must be added after the ``end`` statement, otherwise an error will be reported when the light program is processed by the configurator.









