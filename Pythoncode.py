# This program listens for textmessages sent from a Arduino Nano paired with a NRF24L01 radio transmitter and a A7 GSM/GPRS/GPS module
# If it recieves a specific textmessage from arduino this program translates the textmessage, takes a snapshot with RPi Camera, puts a 
# time&date-stamp, packs the picture and sends it to a gmail with the picture attached.
# Add file to rc.local to make this script run at bootup
# The Arduino only sends specific messages through, so this program listens to every message.
# https://github.com/jonimoi/GSM-project-with-NRF24L01-ArduinoNano-RaspberryPi3
# Made by and last edited: JonMoi, 2019-01-10

import RPi.GPIO as GPIO
from lib_nrf24 import NRF24
import time
import datetime as dt
import os
import spidev
import smtplib
import picamera
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.base import MIMEBase
from email import encoders

GPIO.setmode(GPIO.BCM)
GPIO.setup(4, GPIO.IN, GPIO.PUD_DOWN) #ny
cam = picamera.PiCamera()
time.sleep(1)
# -NRF24L01 Setup- #
#One pipe is used, make sure the pipe is correctly typed in the Arduinocode
#The other pipe is for further development
pipes = [[0xE8, 0xE8, 0xF0, 0xF0, 0xE1], [0xF0, 0xF0, 0xF0, 0xF0, 0xE1]]
radio = NRF24(GPIO, spidev.SpiDev())
radio.begin(0, 17)
radio.setPayloadSize(32)
radio.setChannel(0x76)
radio.setDataRate(NRF24.BR_1MBPS)
radio.setPALevel(NRF24.PA_MIN)
radio.setAutoAck(True)
radio.enableDynamicPayloads()
radio.enableAckPayload()
radio.openReadingPipe(1, pipes[1])
radio.printDetails()
radio.startListening()

# When the radio recieves something, the loop starts
while True:
   while not radio.available(0):
       time.sleep(1)
   #Clear prompt
   os.system('clear')
   receivedMessage = []
   radio.read(receivedMessage, radio.getDynamicPayloadSize())
   print("Received: {}".format(receivedMessage))

   print("Translated: ")
   string = ""

   for n in receivedMessage:
       if (n >= 32 and n <=126):
           string += chr(n)
   print("Our received message coded: {}".format(string))
   # Find date&time
   cam.annotate_text = dt.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
   # Take picture, in the ('') put the location of where you want to save the picture
   cam.capture('')
   print("Picture taken")
   print("Making email & adding picture..")
   time.sleep(1)
   email_user = ''     #Your GMAIL adress here
   email_password = '' #Your GMAIL password here
   email_send = ''     #Your GMAIL adress here
   #Message in mail
   subject = 'Your requested picture'

   msg = MIMEMultipart()
   msg['From'] = email_user
   msg['To'] = email_send
   msg['Subject'] = subject

   body = 'Picture attached'
   msg.attach(MIMEText(body,'plain'))

   filename='' # Location of picture
   attachment  =open(filename,'rb')

   part = MIMEBase('application','octet-stream')
   part.set_payload((attachment).read())
   encoders.encode_base64(part)
   part.add_header('Content-Disposition',"attachment; filename= "+filename)

   msg.attach(part)
   text = msg.as_string()
   server = smtplib.SMTP('smtp.gmail.com',587)
   server.starttls()
   server.login(email_user,email_password)


   server.sendmail(email_user,email_send,text)
   server.quit()
