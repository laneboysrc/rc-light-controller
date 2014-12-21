# Light programs

Light programs are simple scripts that allow end-users to build custom light sequences that go beyond the fixed car related functions built into the MK4 TLC5940 LPC812 light controller.

Light programs can be triggered when the light controller goes into certain states of operation. For example, a light program can be written to flash a 3rd brake light whenever the brakes are enganged.

Light programs are entered into the corresponding edit field in the light controller *configurator.html*.

A total number of 20 light programs can exist at a certain time.

All light programs comprise of the following structure

    run conditions

    variable and led configurations

    statements (= the actual code)

    end

These constructs are described below in further detail.

Here is an example of a light program:

    // ----------------------------------------------------------------------------
    // Light up the main beam lights while the light controller is initializing.
    //
    // Normally the main beam would be on only if the parking lights are already
    // on, so just the main beam lighting up is a unique identifiaction.
    // ----------------------------------------------------------------------------
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

``use all leds`` declares that while this light program is running no other light controller function shall control any of the LEDs. The lines ``led ... = led[y]`` assign human readable names to light outputs 2 and 3.

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


## Run conditions

*Run conditions* describe the state the light controller must be in for a light program to be executed. Each light program defines its own requirement for the state. More than one light program may run at the same time.

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

## Run conditions


        "light-switch-position-0": {"token": "RUN_CONDITION", "opcode": (1 << 0)},
        "light-switch-position-1": {"token": "RUN_CONDITION", "opcode": (1 << 1)},
        "light-switch-position-2": {"token": "RUN_CONDITION", "opcode": (1 << 2)},
        "light-switch-position-3": {"token": "RUN_CONDITION", "opcode": (1 << 3)},
        "light-switch-position-4": {"token": "RUN_CONDITION", "opcode": (1 << 4)},
        "light-switch-position-5": {"token": "RUN_CONDITION", "opcode": (1 << 5)},
        "light-switch-position-6": {"token": "RUN_CONDITION", "opcode": (1 << 6)},
        "light-switch-position-7": {"token": "RUN_CONDITION", "opcode": (1 << 7)},
        "light-switch-position-8": {"token": "RUN_CONDITION", "opcode": (1 << 8)},

        "neutral": {"token": "RUN_CONDITION", "opcode": (1 << 9)},
        "forward": {"token": "RUN_CONDITION", "opcode": (1 << 10)},
        "reversing": {"token": "RUN_CONDITION", "opcode": (1 << 11)},
        "braking": {"token": "RUN_CONDITION", "opcode": (1 << 12)},

        "indicator-left": {"token": "RUN_CONDITION", "opcode": (1 << 13)},
        "indicator-right": {"token": "RUN_CONDITION", "opcode": (1 << 14)},
        "hazard": {"token": "RUN_CONDITION", "opcode": (1 << 15)},
        "blink-flag": {"token": "RUN_CONDITION", "opcode": (1 << 16)},
        "blink-left": {"token": "RUN_CONDITION", "opcode": (1 << 17)},
        "blink-right": {"token": "RUN_CONDITION", "opcode": (1 << 18)},

        "winch-disabled": {"token": "RUN_CONDITION", "opcode": (1 << 19)},
        "winch-idle": {"token": "RUN_CONDITION", "opcode": (1 << 20)},
        "winch-in": {"token": "RUN_CONDITION", "opcode": (1 << 21)},
        "winch-out": {"token": "RUN_CONDITION", "opcode": (1 << 22)},








