#ifndef _THING_COMMAND_
#define _THING_COMMAND_

#include <Arduino.h>
#include "Clock.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define STATUS_SIZE 4
typedef union _status {
  float asFloat[STATUS_SIZE];
  long  asLong[STATUS_SIZE];
  int   asInt[STATUS_SIZE*2];
  byte  asByte[STATUS_SIZE*4];
} Status;

#define MODE_LINEAR 0
#define MODE_SINUSOIDAL 1
#define MODE_RANDOM 2
#define MODE_SINGLE 3

#define VAR_0 1
#define VAR_1 2
#define VAR_2 4
#define VAR_3 8
#define VAR_4 16
#define VAR_5 32
#define VAR_6 64
#define VAR_7 128

#define VARS(a,b,c,d) ((1<<a)|(1<<b)|(1<<c)|(1<<d))
#define VAR(n) (1<<n)
#define HAS_VAR(v,n) (v&(1<<n))
#define PUT_VAR(v,n) (v|=(1<<n))


#define INACTIVE 0
#define ENTERING 1
#define ONGOING 2
#define EXITING 3


class Behaviour;
class Command{
  public:
  
  byte pre;
  byte activity;
  byte actuator;
  byte var; //how to interpret target
  byte mode;

  Status target;

  long time; //milliseconds to reach target
  long activated;

  long reg;
  Status mem;

  Status delta;
  byte processed;
  Behaviour *bh;
  
  void store(float f,int where){
    mem.asFloat[where]=f;
  }
  
  void store(int f,int where){
    mem.asInt[where]=f;
  }
  
  void store(long f,int where){
    mem.asLong[where]=f;
  }

  void storeAll(Status *st){
    for(int i=0;i<STATUS_SIZE;i++) mem.asFloat[i]=st->asFloat[i];
  }
  
  float getStore(int where){return mem.asFloat[where];}
  void reset_step(){reg=0;}
  void next_step(){reg++;}
  void store(long stp){reg=stp;}
  long step(long stp){reg=stp; return stp;}
  long step(){return reg;}
  

  long elapsed(){return now-reg;}
  

  void start(byte with_mode, long in_time, byte vars, ...){
    va_list vl;
    va_start(vl,vars);
    mode=with_mode;
    time=in_time;
    var=vars;
    for(int i=0;i<STATUS_SIZE;i++)
      if(HAS_VAR(var,i)) target.asFloat[i]=(float)va_arg(vl,double);
    activated=now;
    processed=0;
    va_end(vl);
  }
  
  void restore(byte with_mode,long in_time){
    mode=with_mode;
    time=in_time;
    var=VARS(0,1,2,3);
    for(int i=0;i<STATUS_SIZE;i++) target.asFloat[i]=mem.asFloat[i];
    activated=now;
    processed=0;
  }

};


#endif

