# KTaNE Reallife-Edition

This is a reallife edition from the game "Keep Talking and Nobody Explodes"

At the moment there are 8 modules planed (the main-control-module and 5 "normal" modules)

## Hardware

### Master (ESP32)

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
There need to be an orange display and two button to display the questions an type in the answer. A two digit timer is also needet

#### Furthermore
There need to be a display for the serial number, a display and a white LED for the lit indicator and a batery holder on the side of the briefcase.

Also there is one green LED for every modul to indicate if it was solved correctly.

### DOG Pixel-Matrix
- Manual
https://cdn-reichelt.de/documents/datenblatt/A500/DOG-M(1).pdf

- Controller Manual
https://www.lcd-module.de/eng/pdf/zubehoer/st7036.pdf

- Controler example
https://docs.rs-online.com/02c9/A700000006657144.pdf

### DFPAYER mini
https://wiki.dfrobot.com/DFPlayer_Mini_SKU_DFR0299
