#ifndef __PARAIMPU__
#define __PARAIMPU__

#include <SPI.h>
#include <Ethernet.h>
#include <Arduino.h>

#include "Thing.h"



#define SOME 0
#define VALUES 1

#define SAY_DELAY 36000000
#define SENSE_DELAY 10000
#define REQUEST_DELAY 5000


#define SAD_0 0
#define SAD_1 1
#define SAD_2 2
#define HAPPY_0 3
#define HAPPY_1 4
#define HAPPY_2 5
#define LINK_0 6
#define LINK_1 7
#define LINK_2 8




class Paraimpu {
  public:

  long last_sensed;
  long last_sensed_request;
  
  int valueof[VALUES];

  
  EthernetClient client;  

  void setup();

  byte check();
  byte say(byte msg);  
  int consume(){
    int t = valueof[SOME];
    valueof[SOME]=0;
    return t;
  }
  
  byte connected(){
    return client && client.connected();
  }
};

#endif

