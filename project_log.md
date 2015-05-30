# Todo

* decide about power supply
* decide about rotary encoder/buttons

# 2015-05-30

Testing Yun cloud stuff. Temboo seems cool, but the only way to get columns of
data seems to be using a json or xml based fetch. Fetching 100 rows of user info
ended up being 200k of json! 

Made a help request and very quickly got told about adding output filters like
this:

    RetrieveSpecificRowsOrColumnsChoreo.addOutputFilter("users", "/entry[]/content","Response");

Which filters that 200k down to just the bits I need. So that's looking good.
The problem though is that my menu/rfid/lcd stuff is taking about 13k (60%) of
program space on the Yun. The Temboo demo for just fetching the user names comes
out at 13k (45%). So even with the output filtering I don't think the program
size is feasible.

So...
Now looking into a python script that does the spreadsheet stuff and can be
called via the bridge library. It could fetch all users and then cache them to
disk making subsequenct calls fast.

Got [something working](software/spreadsheet/fetch.py) so wanted to test on Yun. OpenWRT doesn't come with pip or
openssl, so with some [googling found
this](http://samjbrenner.com/notes/using-pip-to-install-python-packages-on-the-arduino-yun/).

Pip installed ok though I was amazed at the amount of disk it needs. The Yun
only has about 6.6MB for playing with, so it will be hard to get all these
packages installed.

I'm trying to avoid pip now and install things from downloaded packages, and
using opkg as much as possible. 

The stumbling block now is getting the python ssl stuff working. Most of it
seems to install ok, but it has a dependency on cffi. This seems to need to be
compiled and of course the Yun doesn't come with a compiler...


# 2015-05-29

Train hacking! Got LCD, RFID, encoder and button working on the train up to
Leeds.

# 2015-05-28

Had a look at 13.56MHz RFID readers and common (cheap) readers are all SPI and
3.3v. So I might change the PCB layout to accomodate either the serial or SPI
versions. 

[Cheap RFID/NFC reader for 13.56MHz based on
MFRC-522](http://www.ebay.co.uk/itm/261899995126)

[Image of RFID reader
pins](http://4.bp.blogspot.com/-CypWJHefOgY/U9AnzSpeEyI/AAAAAAAABao/fZKUC4YPNRk/s1600/RFID-RC522-pinout.png)

[Library](https://github.com/miguelbalboa/rfid), supports different SS & Reset
pins.

Not sure about availability of SPI for Yun though, is it used for the
bridge?

[Yun spec](http://www.arduino.cc/en/Main/ArduinoBoardYun) says:

* can handle SPI, though pins are connected to the wifi module
* pins 0&1 used for bridge serial comms so avoid

So will add header, using SS & Reset as the pins I'm using for the serial RFID's
enable & data.

# 2015-05-27

Received the components for testing. For all part numbers and datasheets see the
[bom.md](bom.md)

## Wireless plugs

The only remote plugs RS carry are the brennenstuhl 3600. These operate at
433MHz using AM. I was able to see the codes using a receiver module and a
scope. However RCSwitch doesn't seem to work easily. With some measurement and
copying down of 24bit codes from the scope I got the plugs switching with the
little AM transmitter I'll be using.

However the units seem to use some weird kind of unrepeating code that makes it
harder than I'd like to add more units. Found this [forum
thread](http://forum.pilight.org/Thread-Brennenstuhl-RC-3600) with more info,
but no solution. It works well enough for a demo but I wouldn't recommend these
units for home automation.

Found this [list of codes](http://pastebin.com/RgQ4VCyw), which I've yet to
compare against the 3 that I've decoded.

## LCD

20x2 line standard LCD. Worked as expected.

## RFID

I was expecting the RFID reader to read my hackspace access pass but it didn't.
I know there are 2 main bands: 125KHz and 13.56MHz. The reader is 125KHz type. I
suppose my card is the other, or that there are sub divisions of communication
types. I have ordered some 125KHz tags for now and am investigating the cards we
use for the hackspace.

## Rotary encoder

I was initially thinking of using just 2 illuminated buttons. But I wanted to the option to
use a rotary encoder in case that makes things more intuitive. I got a 12
position detented type to test.

Used an Adafruit library (see [requirements.txt](code/requirements.txt)) to read
the encoder. This works as expected.

# 2015-05-22

Got the Yun schematic so I could start thinking about pinouts. I'm thinking of
making a simple, single sided PCB to keep things neat. Necessary?

Worked out how much money the project cost and thought it was too high to be
practical for anyone else. Had the idea of using wireless mains plugs. This
means the control box can be safer & cheaper - plus have the option to control
many more machines.

Initial research:

* http://hackaday.com/2013/01/31/getting-an-arduino-to-control-a-wireless-outlet/
* https://code.google.com/p/rc-switch/
* https://code.google.com/p/rc-switch/wiki/List_KnownDevices
* http://www.maplin.co.uk/p/remote-controlled-mains-sockets-set-3-pack-n79ka
* http://www.instructables.com/id/Home-Automation-or-Robot-Butler-called-Geoffrey-/

RC-Switch library looks interesting. Have asked RS if they have anything
compatible.

# 2015-05-13 

Initial design for a single enclosed RFID controlled switch. I chose a SSR for
simplicity.

Difficult to find a small panel mounted PSU for the power I require. Best I
could do was a 10W supply, which is much higher than needed.

Enjoyed using [visgraph](http://www.graphviz.org/) for the first time!

Modelled the components in openscad and moved things around to get the switched
mode PSU and Yun on opposite sides (hoping to reduce radio interference).
