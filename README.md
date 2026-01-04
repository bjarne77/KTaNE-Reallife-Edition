# KTaNE Reallife-Edition

This is a reallife edition from the game "Keep Talking and Nobody Explodes"

At the moment there are 8 modules planed (the main-control-module and 5 "normal" modules)

## Hardware

### Master (Arduino Nano)
The Master is equiped an Display for the timer, the serial number, the labels and the batterie count. To switch through all the modes you can press a button. The defplayer is also connected to play sounds. Furthermore there are two LEDs connected to display the strikes.

### Modules (Arduino Nano)

#### On the Subject of Wires:
There need to be two wires per color. The colors are: red, white, blue, yellow, black.

The ends of the cables are connected to 2,5mm audio plugs. To differentiate the wires not all three contacts will be connectet, so the arduino can messure which wire is connected.

#### On the Subject of Simon Says:
There need to be four square buttons in the following colors: blue, yellow, red, green.

#### On the Subject of Morse Code:
There needs to be an orange LED that is bilinking, as well as a display and three button to dial in correct frequenzy and submit it.

#### On the Subject of Complicated Wires
There need to be some wires in different colors and colors combinations. The needet colors are: red, blue, white, red-blue, red-white, blue-white.

To differentiate the wires the same princip is used as with the "normal" wires. The only difference is, that the complicated wires will have 3,5mm jacks so the can not get mixed up with the other wires.

The manual referes to a serialport. Because this is very complicated to change the style of the port between the games, a lit indicator is used instead.

#### On the Subject of Mazes
There need to be four button to move around in the maze, as well as an array of 6x6 single adressable rgb-leds. This will be made with an COB-LED-Stip to make wireing as simple as possible

#### On the Subject of Passwords
There need to be a green display as well as 13 button to display and change the charactars of the password.

#### On the subject of venting gas
There need to be an orange display and two button to display the questions an type in the answer. A two digit timer is also needed.

#### On the subject of keypads
There is an Touchdisplay to show the different symbols and test if they are pressed in the right order.

#### On the subject of whos on first
There is an Touchdisplay for the words, that can be pressed, a display on top for the main word. And a mini LED bar to show the correct stages.

#### On the subject of Memory
There is a LCD Display on top for the Number, a mini LED bar on the side. 4 seven segment displays on the bottom and Button corresponding to the 7 segment displays. 

#### On the subject of capacitive discharge
There needs to be a mini LED bar for the Chargeinglevel, a two digit 7 segment display to show the time. The lever will be 3D-Printed.

#### Furthermore
There needs to be one green LED for every modul to indicate if it was solved correctly (Except the Needy modules.

### DOG Pixel-Matrix
- Manual
https://cdn-reichelt.de/documents/datenblatt/A500/DOG-M(1).pdf

- Controller Manual
https://www.lcd-module.de/eng/pdf/zubehoer/st7036.pdf

- Controler example
https://docs.rs-online.com/02c9/A700000006657144.pdf

### DFPLAYER mini
https://wiki.dfrobot.com/DFPlayer_Mini_SKU_DFR0299

