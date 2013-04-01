#SOME - the Object's root
================================

version 1.0

Intro
-------------------------
#SOME version 1.0 is C++ code for Arduino-compatible microcontrollers developed to empower physical objects with custom-made behaviours. 
It is based on an object oriented architecture that links sensor inputs to actuator's status changes via behavioural functions.
Due to Arduino's space and computational constraints, the current version is not as general-purpose as possible. We are working
on version 2.0 that will feature a virtual machine for an ad hoc programming language targeted to more performant architectures (Arduino Due, Maple, RasperryPI,...) 


Requirements
-------------------------
* #SOME requires an Arduino IDE or an equivalent toolchain. It has been tested with Arduino UNO and Arduino 2009. 
* Due to the size of the code, you need an Arduino with the ATMEL328, or at least 29k of Flash.


How it works
-------------------------

#SOME provides the following C++ Objects:

* SENSE: it abstracts a perception unit by acquiring data from a sensor and storing some features of the stream (average signal, last n samples,...)
* ACTUATOR: it abstracts a device linked to the Arduino that can be "actuated". In our case study we have a RGB Led actuator and a Viber stolen from a dead cellphone.
* BEHAVIOUR: it abstracts the behaviour an actuator should manifest, given the sensory inputs and the current state of #SOME
* COMMAND: it abstracts the status associated with a behaviour.
* SYNAPSE: it abstracts the communication between #SOME's by proximity. Two #SOME can be linked by a serial port an start exchanging "neurotransmitters". Each #SOME has two synapses, so if you are into it, you can try a three #SOME :).


The main setup-logic goes like this:

* call every setup() method in the setup() function of Arduino.
* First, setup() the Senses providing a sensor setup/acquisition function an some parameters (polling time, buffer size).
* Second, setup the Behaviours specifying the actuator they control, and the condition/execution functions.
* Third, setup the Actuators with a function controlling the device.

The main loop logic goes like this:

* update the senses
* call the condition function for each behaviour. The condition function (starting with ch_) modifies the associated Command, changing its status to ENTERING/ONGOING/EXITING/INACTIVE. If a behaviour is in INACTIVE state, its execution (starting with bh_) will not be called.
* call the actuator update method. It will check all the  related behaviours and if they are not INACTIVE, it calls the execution function.
* the actuator will then bring its status to a linear interpolation of the status requested by each behaviour.
* check the ch_touch & bh_touch for better understanding of the interaction between behaviours & actuators.


Disclaimer
-------------------------
1. We are not responsible for any damage this code can cause to your Arduino, PC, smartphone or kittens.
2. We know the code is ugly and maybe buggy. We are working to port it to platforms with more memory (Arduino DUE, RaspberryPI,...), feel free to contribute!


License - MIT
-------------------------
Copyright (c) 2013 FabLabPisa

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWA


