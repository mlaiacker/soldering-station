# Soldering Station for Weller Soldering Tips
This project is based on the work of Martin Kumm.

http://www.martin-kumm.de/wiki/doku.php?id=Projects:SMD_Solderstation

I made it simpler and cheaper. Instead of using an Arduino I use a ATmega8. This design doesn't use SMD instead a single sided PCB for easy assembly and cheap PCB production. The PCB is only 70x50mm. Instead of using two buttons to adjust the temperature I use a poti and optionally you can use a standard HD44780 compatible LCD to display temperatures. It runs on plain c (no arduino) and also includes supports a bootloader for easy firmware upgrade. The Bootloader has to be programmed befor soldering the mega8 into the PCB. The bootloader is activaded using the Boot pins on the PCB (short PD2 to ground)

http://mlaiacker.no-ip.org/Elektronik/Soldering%20Station/

http://fpv-community.de/showthread.php?41335-DIY-SMD-L%F6tstation-Ersatz-f%FCr-%28WELLER%29

## Images
PCB<br>
![max solder pcb]( https://raw.githubusercontent.com/mlaiacker/soldering-station/master/doc/images/IMG_4370.JPG) <br>
With nice enclosure and LCD<br>
![maxsolder anclosure](https://raw.githubusercontent.com/mlaiacker/soldering-station/master/doc/images/img_20140730_225220.jpg) <br>
You can also just use this scale to set the temperature instead of the LCD
![maxsolder anclosure](https://raw.githubusercontent.com/mlaiacker/soldering-station/master/doc/images/scale_solder.PNG) <br>
Ask me for PCBs !! I had 10 made and some are available<br>
![PCBs]( https://raw.githubusercontent.com/mlaiacker/soldering-station/master/doc/images/IMG_4367.JPG) <br>

## FAQ

### Hast du noch Platinen? 

ja noch 
10 (2.4.2016)
9 (5.4.2016)
Ich kann auch gleich programmierten atmega8 und OPA und LCD dazu legen

### Was Kostet? 
10

### hex bauen?
Das erstellen der main.hex geht über das Makefile mit "make all" dazu musst du den gcc avr Compiler installiert haben siehe dazu :http://www.mikrocontroller.net/articles/AVR-GCC

### flashen?
Mit "make program" wird dann avrdude aufgerufen um über die serielle Schnittstelle ein neues Programm zu laden.
Dazu musst du wahrscheinlich den Port (com1) ändern im Makefile

AVRDUDE_PORT = com1 -b 38400 # programmer connected to serial device

Computer per TTL Seriell mit der Platine verbinden einen jumper auf die "Boot" pins stecken und einschalten dann wird der Bootloader aktiviert.

https://github.com/mlaiacker/soldering-station/blob/master/doc/images/board_v12.png

### bootloader ?

kopliziert. mit extra Adapter per ISP und dann mit "make programm_all" im bootloader Ordner. (avrdude und DIAMEX USB ISP bei mir)

### Wo bekomme ich IC3 her?

TLC 272 DIP bei reichelt
