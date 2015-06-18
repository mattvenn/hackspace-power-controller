# Todo

* More testing on Yun Mini Wifi

# 2015/06/16

The Yun Mini finally arrived! So these notes will be about the differences I
discover in getting the software to work on it.

* password/login screen has changed, and the password is doghunter
* the hostname has changed to linino.local
* uname -a now is Linux linino 3.3.8 #1 Sat Apr 18 02:45:58 CEST 2015 mips GNU/Linux
* no package python-ssl, tried python-openssl, works
* no python package atom, copied from my laptop (added to gdata.tar.gz)

Things that didn't work:

* Button [didn't work as expected](http://www.mattvenn.net/2015/06/18/57m-ss-hall-effect-button/)!
* PSU wrong! I've got no idea how I managed to get this wrong after spending so
 much time on it. It only goes up to ~120VAC. I've changed the BOM already.
* My Yun Mini PCB footprint slightly wrong sized, but still fits!
* My Yun Mini has poor wifi abilities, can't reliably create a hotspot or stay connected to another network.

# 2015/06/12

Nice day in the workshop building the [case](case/case.scad). Even made a vinyl
cut sticker to show where the RFID tag is swiped.
Laser cut the mounting panels for inside the box, and used the CNC router for
the front panel. Had to dremel the IEC inlet because the box is tool tall to fit
in the machines.

[Bubble etched](http://bristol.hackspace.org.uk/wiki/doku.php?id=bubble-etch) the PCB. I know it's not really necessary but I'm a sucker for
neatness inside the box as well as outside!

Had a chat with Nic about usage, and he reckons that requiring people to log
out will be problematic. Within the scope of this project I can't do much about
that. I do like the London and Nottingham hackspace systems that require an RFID
card left in a box by the machine in use.

I've modified the software so that a new user can stop another user's running
session in case they've left a machine running accidentally.

It's an interesting part of this project - normally I have a set idea I'm
working to and then if I make a mistake with a part I can change it. This time,
I'm working with a specific part, and then if later I realise the idea doesn't
quite fit I'd have to change the idea. Not going to do that after all this work
though!

# 2015/06/09 Lessons learnt (again)

* Just because I've seen a problem before doesn't mean I'll recognise it again
* Interprocess communication is harder than it seems
* When debugging problems with processes ending on Linux, keep a log of load avg
 and memory usage
* When making major changes to [the plan](#multiprocess), spend time thinking of
 the ramifications - pretty obvious what was going to happen in hindsight.

# 2015/06/06 Bridge.py problems

On another long burn test I got bridge.py to crash again. Once it's dead then
it's game over for the whole system. This time I was logging everything and I
found something interesting. Just before the system stopped I got a connection
timeout from post.py (logging to internet). In the meantime another 2 logging
processes were started (before the first post.py timed out). So 3 in total, plus the query program that calls it.

I [found a good way to spoof a
timeout](http://stackoverflow.com/questions/100841/artificially-create-a-connection-timeout-error)

Then I ran a simple Arduino program to call query.py which in turn starts a new
process (post.py) that will get timed out. After 4 loops the Yun became
unresponsive. I got a chance to run loadavg and look at /proc/meminfo:

    20:10:58 up  8:01,  load average: 2.31, 1.01, 0.47

    root@Arduino:~# cat /proc/meminfo 
    MemTotal:          61116 kB
    MemFree:            1356 kB

I'm guessing that as memory ran out, oom killer started killing big memory hogs,
which includes bridge.py (13% of total RAM, oom_score=45).

It's funny because I've come across this exact issue before with a script
executed with a cronjob - fixed with a lock. And I'd put adding a lock to
post.py on the todo list, but didn't get round to it. Instead I've spent hours researching a
problem that wouldn't have happened if I'd added the lock in the first place!

I tried to get the oom killer to do it's thing by running lots of processes but
it never seemed to come into life. After enough processes the Yun hung and I had
to reboot it. Fingers crossed this is the issue and it's now solved.

After a night of running the `Tool log - read rfid, fetch user, log tool to
internet` test I clocked up 5600 cycles and everything stayed running.

# Burn testing

Lots of cycles of the parts of the code that use the bridge code:

* fetching user info
* logging unknown rfid to internet
* fetching tools
* logging tool user to internet

## Unknown rfid - read rfid, fetch user, log to internet

3500 cycles in 8 hours, 11 cycles were longer than 5 seconds.
Mem checks:

    root@Arduino:~# cat /proc/meminfo | head
    MemTotal:          61116 kB
    MemFree:           29244 kB

Uptime checks

    20:02:51 up  3:16,  load average: 1.48, 1.65, 1.75

## Known rfid - read rfid, fetch user, return

1000 cycles in 1:10, all cycles completed in less than 4 secs

    21:22:26 up  4:36,  load average: 1.33, 0.72, 0.57

## Tool log - read rfid, fetch user, log tool to internet

Just when I thought it was safe to celebrate I started getting this:

    from: 06-05 07:48:42 about to run   
    to  : 06-05 07:49:18 finished        took 36.0 secs
    from: 06-05 07:49:44 about to run   
    to  : 06-05 07:50:21 finished    

Starting at 00:21:00 and continuing for every bridge process call after. Looking
at the logs for the query.py program and the post.py program I could see that
neither had been called since that time. So something happened to the bridge
program that made it unable to call programs.

I added some log lines to the Yun's bridge.py and processes.py programs to help
debug this. Now I'm getting lines like this in ~/bridge.log:

    06-05 09:17:46 INFO     starting process [['/root/query.py', '--check-user', '--rfid', '04184A4FC6']]
    06-05 09:17:46 INFO     process id is 0
    06-05 09:17:49 INFO     waiting for process 0
    06-05 09:17:49 INFO     finished 0

So at least I'll be able to see what the process call was before bridge gets
killed. Just setup another identical test and will leave all day.

# 2015-06-04 - Even More Testing

Still investigating the hangs. Got another program called
[process_log.py](software/python/process_log.py) that reads the log lines and
reports any pair of lines that are more than a set number of seconds apart.
That's been useful in helping to identify which parts of the code are hanging.

This test was with an invalid rfid card on top of the reader. It shows that the code is sometimes taking 30 seconds to read the rfid.

    from: 06-04 08:34:23 finished       
    to  : 06-04 08:34:53 rfid available  took 30.0 secs

And here's some logs showing what happens when the arduino code checks the rfid
with the cached users using query.py:

    from: 06-03 08:35:52 about to run   
    to  : 06-03 08:38:45 finished        took 173.0 secs
    from: 06-03 08:46:04 about to run   
    to  : 06-03 08:47:11 finished        took 67.0 secs

And here's the log from the python program at the same time:

    06-03 08:35:55 WARNING  logging unknown rfid [04184A51EB]
    06-03 08:35:55 INFO     ending
    06-03 08:38:53 WARNING  logging unknown rfid [04184A51EB]
    06-03 08:38:53 INFO     ending

I can see the program starting and ending straight away, but the code on the
arduino is hanging for around 3 minutes!
So I'm assuming the problem is with the Process stuff on the Arduino or the bridge.py program on the Yun.

I just updated the arduino ide from 1.5.8 to 1.6.4. Looking at the md5sums of
the source for the Bridge stuff, I can see that Bridge.cpp has changed but
Process.cpp hasn't. There is a change in put() which I don't think is being
used, but another in transfer to do with buffer lengths, so perhaps that's
hopeful.

Setting exit(1) in query.py for unknown users seems to have helped with the long
hang times related to looking up user. The update to arduino ide 1.6.4 made a
big difference to long timeouts occuring randomly to all bridge process
requests.


# 2015-06-03 - More Testing

As always, it's fairly quick to put together something that sort of works. But
then finding all the little bugs and issues that have to be ironed out to make a
good reliable product takes a lot longer.

To help me test it I put together a [useful little
script](software/python/log_serial.py) to capture and time
stamp the logging from the Arduino part of the Yun.

The system has been running over night and I've collected logs from the Linux
and Yun side of things (timestamps synced courtesy of ntp).

The only issue so far is occasional minute long hangs with the Process.run()
Arduino code. After all the hassle with getting things working in the first
place I'm going to leave this un-investigated for now. If people are interested
in the project then it can be fixed then.

# 2015-06-02 - Testing

During testing I discovered a nasty bug where the bridge.py process on the yun
(that deals with linux<->arduino communication) was failing. After a lot of
debugging (I thought it was to do with stderr buffers being overflowed) I
finally discovered that I wasn't null terminating the formatted time string for
tool logging.

## Multiprocess

There are 3 times when the system needs to connect to the internet:

* updating cache of users and tools
* logging unknown rfids
* logging tool usage when a tool is turned off

I don't want the UI to hang while these are happening. Although the Yun bridge
library has support for running shell commands asyncronously I decided to do it
on the Linux side.

I split the fetch program into [query](software/python/query.py) and
[post](software/python/post.py). 

query is what the Arduino uses for all user info/logging requests. It necessary
it then runs the post command as a subprocess that runs separately.

# 2015-06-01 - Testing

Nice day of tidying things up and testing. Decided to put all the config data in
one spreadsheet - tool owners/responsible people will have to trust each other.

Remember to doc some spreadsheet gotchas:

* tool ids have to be 1 indexed, as 0 is invalid (software bug with parseInt returning 0 for no parse)
* max tools is 10 (firmware)

Wanted to do async running of shell command but it doesn't seem to work. Not
investigating further for now.

# 2015-05-31 - Google doc access on the Yun with Python

Finally found a way of doing oauth2 with python without the google oauth2.client
library that has too many dependencies: gdata. This is another google library
for interacting with the google apis. It has built in oauth2 client and can be
installed on the yun:

    wget https://pypi.python.org/packages/source/g/gdata/gdata-2.0.18.tar.gz#md5=13b6e6dd8f9e3e9a8e005e05a8329408 --no-check-certificate

    tar -xzf gdata-2.0.18
    cd gdata-2.0.18
    python setup.py install

This package needs python expat installed:

    opkg install python-expat

## Oauth2 stuff:

* setup a new client id of type 'service account' in https://console.developers.google.com
* download json and save to secrets.json
* run ./fetch.py --auth-token
* follow the url, then tokens will be added and saved to secrets.json

# 2015-05-30 - Temboo

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

Trying an older version of oauth2client 1.4.3, still uses pyOpenSSL which has
the cffi dependency.

# 2015-05-29 - Peripheral hacking

Train hacking! Got LCD, RFID, encoder and button working on the train up to
Leeds.

# 2015-05-28 - AM switch control with RC-Switch

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

# 2015-05-27 - Components arrive

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

# 2015-05-22 - Research

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

# 2015-05-13 - Initial design

Initial design for a single enclosed RFID controlled switch. I chose a SSR for
simplicity.

Difficult to find a small panel mounted PSU for the power I require. Best I
could do was a 10W supply, which is much higher than needed.

Enjoyed using [visgraph](http://www.graphviz.org/) for the first time!

Modelled the components in openscad and moved things around to get the switched
mode PSU and Yun on opposite sides (hoping to reduce radio interference).
