# UI State machine

![FSM diagram](fsm.png)

# LCD Menu

[UI](ui.md)

# Arduino IDE

I had problems with an older version of the Arduino IDE compiler and Bridge
libraries. I had success with Arduino v1.6.4

# Requirements

Thanks to the developers of these libraries!

* [Rotary](https://github.com/brianlow/Rotary)
* [RFID](https://github.com/miguelbalboa/rfid) - only required for MFRC522
 reader
* [RCSwitch](https://github.com/sui77/rc-switch)

If you are using the Brennenstuhl sockets, then until my pull request is merged,
please use this [updated RCSwitch library](https://github.com/mattvenn/rc-switch).
