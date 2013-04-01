#ifndef _THING_SENSE_
#define _THING_SENSE_

#include <Arduino.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef float (*value_fn)();
typedef void (*setup_fn)();

class Sense {
 
  public:
  
  byte m;
  int time;
  unsigned long last_update;
  
  
  float temp;
  float current;
  float* previous;
  float average;

  byte last_sensed;

  value_fn sfn;
  
  Sense(){
  }
    
  
byte overThreshold(float lvl,float perc){
  int nt=0;  
  for(int i=0;i<m;i++){
    if(previous[i]>=lvl) nt++;
  }
  if(((float)nt/m)>perc) return 1;
  return 0;
}

byte overDelta(float dl){
  return (maxDelta()>dl);
}

float maxDelta(){
  float min=1;
  float max=0;
  
  for(int i=0;i<m;i++){
    if(previous[i]<min) min=previous[i];
    if(previous[i]>max) max=previous[i];
  }
  return (max-min);
}
  
  
  
void setup(setup_fn ssf,value_fn nvf,int ms,int ptime){
    m=ms;
    time=ptime;
    sfn=nvf;
    previous = (float*)calloc(m,sizeof(float));
    ssf();
  };
  
  
void update(){
    if(now-last_update<time) return;
    
    float val = sfn();
    last_sensed = (last_sensed+1)%m;

    average=(average*m-previous[last_sensed]+val)/m;
    previous[last_sensed]=current;
    current=val;
}
  
};


#endif

