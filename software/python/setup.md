# Yun

[update to latest version](http://www.arduino.cc/en/Tutorial/YunSysupgrade)

timezone yun - fixed with the yun setup page

## packages

    opkg update
    opkg install python-expat
    opkg install python-ssl

## install python dependencies

This won't work without having an expanded disk. How to do it for the mini?

    wget https://pypi.python.org/packages/source/g/gdata/gdata-2.0.18.tar.gz#md5=13b6e6dd8f9e3e9a8e005e05a8329408 --no-check-certificate

    tar -xzf gdata-2.0.18
    cd gdata-2.0.18
    python setup.py install

## copy over programs

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
* json will be downloaded. Save this file to secrets.json on the yun
* run ./post.py --auth-token --spread-key [your spreadsheet key here]
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
