# LCD messages & Menu

Menu is driven by 2 illuminated buttons: next and select.

Display after inactive for 5 seconds

    --------------------
    scan RFID to start
    ven.nz/hkspcpc

Unrecognised user.

    --------------------
    unrecognised RFID


user Matt recognised

    --------------------
    user: matt
    scroll for tools

next page - shows a tool ready to start

    --------------------
    1: lathe    inducted  
    select to start

next page - showing Nic is already using machine for 1 hour 21 secs

    --------------------
    2: mill     inducted
    nic         00:01:21

next page - showing router is currently unavailable

    --------------------
    3: router   noinduct
                offline 

next page - showing laser cutter in use, but not inducted

    --------------------
    4: laser    noinduct
    nic         00:01:21

next page - showing saw available for use

    --------------------
    5: saw      inducted
    start?

start/stop button pressed - showing saw and option to stop

    --------------------
    5: saw      inducted
    stop?       00:00:20

next page - showing option 1 again

    --------------------
    1: lathe    inducted  
    start?

## Beeper

Beep when:

* RFID is read
* Button is pressed

## LEDs

Both buttons have green leds. 

* Next button illuminated when valid user present
* Select button illuminated when a choice is allowed

