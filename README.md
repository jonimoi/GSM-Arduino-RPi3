# GSM-project-with-NRF24L01-ArduinoNano-RaspberryPi3

![](GifGSM.gif)

The GSM module & Arduino Nano receives a text-message sent from a phone. If the word is correct the Arduino Nano takes the message and sends it to a Raspberry Pi 3B+ through NRF24L01 2,4GHz radio modules.. 

The RPi runs a infinite Pythonloop that responds to a Radio.read and the script takes a picture, puts a time/datestamp on it and attaches the picture in a mail, and sends it to a Gmail-adress of the users choice!

Ai Thinker A7 needs to be set to a baudrate of 9600, follow this guide:
http://acoptex.com/project/286/basics-project-064a-ai-thinker-a7-gsm-gprs-gps-module-at-acoptexcom/

