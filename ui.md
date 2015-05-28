# LCD messages & Menu

Menu is driven by 2 illuminated buttons: next and select.

Display after inactive for 5 seconds

    --------------------
    scan RFID to start
    goto: ven.nz/1RFvu9s

Unrecognised user.

    --------------------
    unrecognised RFID
    goto: ven.nz/1RFvu9s

user Matt recognised

    --------------------
    user: matt
    press next for tools

next button pressed - shows a tool ready to start

    --------------------
    1: lathe    inducted  
    select to start

next button pressed  - showing Nic is already using machine for 1 hour 21 secs

    --------------------
    2: mill     inducted
    nic         00:01:21

next button pressed - showing router is currently unavailable, with webpage for details

    --------------------
    3: router   offline
    goto: ven.nz/1RFvu9s

next button pressed - showing laser cutter, not inducted and webpage for detils

    --------------------
    4: laser    noinduct
    goto: ven.nz/1RFvu9s

select button pressed - showing saw available for use

    --------------------
    5: saw      inducted
    select to start

select button pressed - showing saw and option to stop

    --------------------
    5: saw      00:00:00
    select to stop

select button pressed - showing option 1 again

    --------------------
    1: lathe    inducted  
    select to start

## Beeper

Beep when:

* RFID is read
* Button is pressed

## LEDs

Both buttons have green leds. 

* Next button illuminated when valid user present
* Select button illuminated when a choice is allowed

