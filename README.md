# Arduino Yun


# Hackspace RFID power switch & usage logger


## Hardware plan

![hardware](hardware.svg)

## Hardware layout


## Software plan

![software](software.svg)

## Parts list

* Arduino Yun
* case
* PSU
* RFID reader
* SSR
* LCD 2x20
* fused power inlet
* power outlet
* green & red LEDs

## Case

All measurements in mm and stated as L x W x D

* Yun = 73 x 53 x ?
* SSR + heatsink = 100 x 55 x 67
* PSU = 105 x 50 x 18
* LED = 12mm diameter
* RFID = 62 x 82 x ?
* IEC inlet/outlet/fuse = 32.5 x 79.2 (hole size for 1.5mm panel thickness) x 32.3 depth
* LCD = 37 x 116 x ?

Modelling the layout led to the following dimensions for the case:

    280 x 120 x 90

But these dimensions didn't yield many cases! So I looked for some common sizes
and found some that were 240 x 120 x 100mm. Setting the openscad case dimensions
to these figures allowed me to move things about to check fit.

One decision I made was to keep the PSU and the RFID reader on opposite sides of
the case, hopefully reducing interferance from the switched mode PSU.

![case](case.png)

