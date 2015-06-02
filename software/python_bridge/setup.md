# Yun

[update to latest version](http://www.arduino.cc/en/Tutorial/YunSysupgrade)

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


