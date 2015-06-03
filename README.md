# Hackspace RFID power switch & usage logger

A system for allowing inducted users to turn power on to various machines. Usage
time is logged to a google spreadsheet. Inducted users are managed using a
google spreadsheet.

Shortlink: [http://ven.nz/hkspcpc](http://ven.nz/hkspcpc)

# Project log

I've been making notes of [progress in the log](project_log.md)

# Software

## Design decisions

* Presenting an inducted user's RFID card will authenticate.
* Simple menu system to scroll through available tools and start/stop them.
* Never switch off/time out the connected machine in case it's a long job.
* Pull validated users once per day and store locally in case internet is unavailable.
* If internet is unavailable discard usage logging. Possible to add a queue later.

## Flowchart

![software](software/software.png)

## User Interface

See the [UI](ui.md) doc.

# Electrical

## Bill of Materials

See the [bom](bom.md)

## Wiring

![electrical components and wiring](electronics/electrical.png)

## Schematic

For interconnects, power and buzzer.

![schematic](electronics/schematic.png)

## Case design

[generated with openscad file case.scad](case/case.scad)

![case](case/case.png)

All measurements in mm and stated as L x W x D

* Yun + PCB = 70 x 85 x 10
* RFID = 62 x 82 x ?
* LCD = 37 x 116 x ?
* Buttons = 8 x 20

Modelling the layout led to a case size of 280 x 120 x 90mm.
But these dimensions didn't yield many cases! So I looked for some common sizes
and found some that were 240 x 120 x 100mm. Setting the openscad case dimensions
to these figures allowed me to move things about to check fit.

# Shortcomings

In many cases easy to bypass, but as used in a trusting environment, not seen as a big issue. 

To protect the plugs a locking box could be made out of wood that would contain an extension lead end, the radio power switch and the end of the machine cable.

To protect the spreadsheet, it could be split into different sheets and then
shared separately.

# Licenses

* Hardware is licensed under the [CERN open hardware license 1.2](http://www.ohwr.org/attachments/2388/cern_ohl_v_1_2.txt)

* Software is licensed under a Creative Commons Attribution 4.0 International License.
