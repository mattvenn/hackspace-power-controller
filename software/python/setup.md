# Yun

[update to latest version](http://www.arduino.cc/en/Tutorial/YunSysupgrade)

timezone yun - fixed with the yun setup page

## packages

    opkg update
    opkg install python-expat
    opkg install python-ssl

## install gdata

[gdata](https://pypi.python.org/pypi/gdata) is too big without having an
expanded disk - which requires an extra breakout board for the Yun mini.

Instead, use this [minimal version](gdata.tar.gz) that includes only support for
what we need.

Copy it to the Yun, then when logged in:

    cd /usr/lib/python2.7/site-packages/
    tar -xzf ~/gdata.tar.gz

## copy over programs

    cd software/python/
    scp post.py query.py root@arduino.local:~/

## create a spreadsheet

make a copy of this
[spreadsheet](https://docs.google.com/spreadsheets/d/1C3qdswIIFLBH4fQWuWFZRTpnnRjpzyk5xHRKnjuHExU/edit?usp=sharing).

Make a note of your spreadsheet key; it's the text directly after "d/" in the URL up to "/edit"

## do oauth2 for post.py

* create a new project at https://console.developers.google.com
* on left menu choose APIs & auth -> APIs.
* Search for drive API and then enable
* on left menu choose APIs & auth -> credentials
* 'create new client id' of type 'installed application', 'other'
* a json file will be downloaded. Save this file to /root/secrets.json on the yun
* on the yun, run ./post.py --auth-token --spread-key [your spreadsheet key here]
* follow the url, and copy and paste the validation code into the terminal; tokens will be added to secrets.json

Test by running:

    ./post.py --update-cache

Which should fetch your users and tools

Then try:

    ./post.py --log-unknown-rfid --rfid xxxx

And check the bad rfid has been logged in the 'unknown' tab of the spreadsheet

Both ./post.py and ./query.py log lots of debugging information to post.log and
query.log

## crontab

refresh the local cache every hour

    0 * * * * ./post.py --update-cache
