# Overview

query.py and post.py are required on the Yun. 
[setup instructions](setup.md)

## [query.py](query.py)

Called by the Arduino code using the [Bridge Process](http://www.arduino.cc/en/Reference/YunProcessConstructor)

Uses a local cache of tools and users to answer queries.

Runs post.py as a background process to do internet related tasks.

## [post.py](post.py)

Deals with all internet stuff:

* log tool usage
* post unrecognised rfid
* update local cache
* setup oauth2 tokens

# Debugging tools

## [log_serial.py](log_serial.py)

Helper script to log output of the Arduino and timestamp it for debugging

## [process_log.py](process_log.py)

Helper script to report number of cycles and cycle times of the debug logs
