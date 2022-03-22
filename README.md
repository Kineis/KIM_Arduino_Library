Kim2 Library :
# HW setup

* Arduino Uno : ATmega328 MCU
* KIM shield


\note 
This library is compatible with **Arduino Uno**, **Arduino Uno WiFi Rev2**, **Arduino NANO 33 BLE**, **Arduino MKR Wifi 1010**, **Wemos D1 Board**.
Check compatibilty if you want to use this library with other boards.

# SW

## Getting started

You can create the documentation for this software package, generated with Doxygen : 

* install Doxygen:
  ````
    sudo apt-get install doxygen
  ````
* run the Makefile :
  ````
    make doc
  ````
  
A good method is to start by compiling the example : 

* Open "Example_KIM2" with the Arduino IDE :
* Select the target board : Tools -> Boards -> Your Board
* Select the right port : Tools -> Port -> Your Board Port
* To upload it to your board, press the "Upload"
* Open the serial monitor (9600 baud)

If the HW configuration is right you should see the configuration of the Kim module.

## SW package

This software package contains:
* Arduino SW Library to control KIM Module
* Examples

The examples show how to use KIM_Arduino_Library.h library.
Below is the description of the core of "Example_KIM2" application : 
* Initialization : library, UART & GPIOs;
* Get factory data and transmission parameters(**KIM ID**, **KIM FW version**, ...) 
and print data on UART configured to show logs;
* Set transmission parameters and show logs;
* Start infinit periodic transmission : default payload value **0123456789**

\note 
To Communicate with KIM module, UART communication is configured as below 
* *BaudRate* : **9600**
* *Word length* : **8 bits**
* *Parity* : **None**
* *Hardware flow control* : **None**
* *Stop bits* : **1 bit**


## Source tree

The code is detailed as below:

````
.
├── doxyfile
├── README.md
├── Makefile
├── keywords.txt
├── library.properties
├── logo_kineis.png
````
 Folder containing this documentation once generated with Doxygen
````
├── doc
│   └── html
│       ├── index.html
│       └── ...
````
 Source code of Kim library
````
├── src
│   ├── KIM_Arduino_Library.h
│   └── KIM_Arduino_Library.cpp
````
 Source code of example application
````
└── examples
    ├── Example_KIM1_SW2
    ├── Example_KIM2
    └── Example_KIM2_Prepass
````