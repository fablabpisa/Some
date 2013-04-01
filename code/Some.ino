#include <SPI.h> // needed in Arduino 0019 or later
#include <Ethernet.h>
#include "Clock.h"
#include "Thing.h"
#include "Synapse.h"
#include "Paraimpu.h"
#include <CapacitiveSensor.h>

CapacitiveSensor   cs_4_2 = CapacitiveSensor(pinCapOut,pinCapIn);

START_CLOCK();
START_THING();
START_CONNECTIONS();


void setup(){
  
  Serial.begin(9600);
  BEGIN_CONNECTIONS(); 
  
  senses[0].setup(&setup_light,&sense_light,20,50);
  senses[1].setup(&setup_touch,&sense_touch,10,100);
  senses[2].setup(&setup_sound,&sense_sound,20,50);
  senses[3].setup(&setup_temperature,&sense_temperature,2,1000);
  
  behaviours[0].setup(0,&ch_touch,&bh_touch,&commands[0]);  
  behaviours[1].setup(0,&ch_dance,&bh_dance,&commands[1]);  
  behaviours[2].setup(0,&ch_syn,&bh_syn,&commands[2]);  
  behaviours[3].setup(0,&ch_greet,&bh_greet,&commands[3]);  
  behaviours[4].setup(0,&ch_net,&bh_net,&commands[4]);  
  behaviours[5].setup(0,&ch_idle,&bh_idle,&commands[5]);  
  behaviours[6].setup(1,&ch_touch,&bh_touch,&commands[6]);  
  
  actuators[0].setup(0,&actuator_lamp);
  actuators[1].setup(1,&actuator_viber);

}

void loop(){
  getNow();
  for(int i=0;i<SENSES;i++) senses[i].update();
  for(int i=0;i<BEHAVIOURS;i++) behaviours[i].check(senses,actuators,commands);
  for(int i=0;i<ACTUATORS;i++) actuators[i].update(senses,actuators,commands);    
 }



#define DISABLE_CMD() cmd->activity=INACTIVE;return;

byte is_one_active(Command *cmds,int n,...){
  va_list vl;
  va_start(vl,n);

  byte ret=0;
  for(int i=0;i<n;i++){
    int cmd = va_arg(vl,int);
    if(cmds[cmd].activity) ret=1;
  }
  
  va_end(vl);
  return ret;  
}

//-------------------------TOUCH
void ch_touch(Sense* senses,Actuator *actuator, Command *cmds, Command *cmd){
  
 // DISABLE_CMD();
  if(senses[TOUCH].average>150){
    cmd->activity=(cmd->activity==INACTIVE) ? ENTERING:ONGOING;
  } else {
    cmd->activity=(cmd->activity==INACTIVE) ? INACTIVE:EXITING;
  }
}

void bh_touch(Sense* senses,Actuator *actuator, Command *cmds, Command *cmd){
  
  switch(cmd->activity){
    case ENTERING:
    break;
    case ONGOING:
    if(cmd->actuator==0) {
      //LAMP
      if(cmd->elapsed()>100){
        AROUSED(0.03);
        EMOTION(0.005);
        cmd->step(now);
        synDX.fire(MSG_TOUCH,(byte)(R()*255),(byte)(G()*255),(byte)(B()*255),(byte)(BRIGHTNESS()*255));
        synSX.fire(MSG_TOUCH,(byte)(R()*255),(byte)(G()*255),(byte)(B()*255),(byte)(BRIGHTNESS()*255));
        //Serial.println("SEND TOUCH");
      }
      cmd->start(MODE_LINEAR,500,VARS(0,1,2,3),COLOR(),BRIGHTNESS());
    } else cmd->start(MODE_SINGLE,500,VAR(0),1.0f); //VIBER
    break;
    case EXITING:
         cmd->start(MODE_SINGLE,500,VAR(0),0.0f);
    break;
  }
}



#ifndef AWE_LAMP
#define AUDIO_THRESHOLD 0.7
#else
#define AUDIO_THRESHOLD 0.5
#endif


//-------------------------DANCE
void ch_dance(Sense* senses,Actuator *actuator, Command *cmds, Command *cmd){ 
   
  //DISABLE_CMD();
  //sound "power" sensor calculations
  float mx = senses[SOUND].maxDelta();
  cmd->store(mx,2);
  if(cmd->step()>1000) {
    cmd->mem.asFloat[3]/=cmd->step();
    cmd->step(1);
  }else {
    cmd->mem.asFloat[3]+=mx;
    cmd->next_step();
  }
    
  if(cmd->mem.asFloat[3]/cmd->step()>AUDIO_THRESHOLD && now>5000 && mx>0.3 && !is_one_active(cmds,2,CMD_SYN,CMD_TOUCH)) {
    if(mx<1.0f)  cmd->mem.asLong[0]=now;
    cmd->activity=(cmd->activity==INACTIVE) ? ENTERING:ONGOING;
  } else{
    if(cmd->activity==ENTERING) cmd->activity=ONGOING;
    else cmd->activity=(cmd->activity!=INACTIVE && cmd->activity!=EXITING) ? EXITING:INACTIVE;
  }
  
}
void bh_dance(Sense* senses,Actuator *actuator, Command *cmds, Command *cmd){   

  switch(cmd->activity){
    case ONGOING:{
      float mx = cmd->getStore(2);
      float vf = (mx>0.4) ? ((mx>0.8) ? 1.0f:0.5f):0.2f;
      if(now-cmd->mem.asLong[0]>500) {
        cmd->start(MODE_LINEAR,100,VARS(0,1,2,3),0.0f,0.0f,0.0f,0.0f);
        synDX.fire(MSG_CANDLE,0.0f);
        synSX.fire(MSG_CANDLE,0.0f);
      } else cmd->start(MODE_LINEAR,100,VARS(0,1,2,3),random(0,2)*1.0f,random(10,21)/20.0f,random(0,2)*1.0f,vf);
      if(arousal<0.8) AROUSED(AROUSAL_UNIT);
    }
    break;
    case EXITING:
      //cmd->start(MODE_SINGLE,10,1.0,VARS(0,1,2,2),0.0f,0.0f,0,0.0f);
    break;
  }
}

//-------------------------IDLE
void ch_idle(Sense* senses,Actuator *actuator, Command *cmds, Command *cmd){
  cmd->activity=ONGOING;
}

#define basiccolor 0.31372549f
#define maxvariation 0.5f
#define variation 0.04f
void bh_idle(Sense* senses,Actuator *actuator, Command *cmds, Command *cmd){

    AROUSAL_DOWN(200);
    EMOTION_DOWN_WITH_TEMP(200,senses[TEMPERATURE].average);
    
    synDX.fire(MSG_HELLO,0.0f);
    synSX.fire(MSG_HELLO,0.0f);

    if(!is_one_active(cmds,5,CMD_TOUCH,CMD_SYN,CMD_GREET,CMD_DANCE,CMD_NET)){
      float rnd[3];
      float color;
      color = basiccolor + (random(-1, 1)*variation);
      color = constrain(color,(basiccolor*(1-maxvariation)),(basiccolor*(1+maxvariation)));
      if(senses[LIGHT].average<0.3){
        rnd[0]=rnd[1]=rnd[2]=color;//white
      } else if(senses[LIGHT].average>=0.3 && senses[LIGHT].average<0.5){
        rnd[0]=rnd[1]=color;//yellow
        rnd[2]=0.0f;
      }  else if(senses[LIGHT].average>=0.5){
        rnd[0]=R()*color;
        rnd[1]=G()*color;
        rnd[2]=B()*color;
      }
      cmd->start(MODE_LINEAR,100,VARS(0,1,2,3),rnd[0],rnd[1],rnd[2],constrain((1.0-senses[LIGHT].current)+arousal,0.2f,1.0f));    
    } else     cmd->start(MODE_SINGLE,200,0);//empty command during 200
   
  #ifndef AWE_LAMP
  #ifdef DEBUG
  DEBUG_COMMANDS();  
  DEBUG_SENSES();
  #endif
  #endif
   

}




//-------------------------GREET
//fired when someone passes by
void ch_greet(Sense* senses,Actuator *actuator, Command *cmds, Command *cmd){
  //DISABLE_CMD();
  if(senses[LIGHT].overDelta(senses[LIGHT].average>0.5 ?0.2:0.1) && !is_one_active(cmds,3,CMD_DANCE,CMD_TOUCH,CMD_SYN)){
    cmd->activity=(cmd->activity==INACTIVE) ? ENTERING:ONGOING;
  } else if(cmd->elapsed()>2000){
    cmd->activity=(cmd->activity==INACTIVE) ? INACTIVE:EXITING;
  }
}
void bh_greet(Sense* senses,Actuator *actuator, Command *cmds, Command *cmd){
  
  switch(cmd->activity){
    case ENTERING:
      //cmd->storeAll(&actuator[cmd->actuator].status);
      if(cmd->elapsed()>60000){
        AROUSED(0.1);
        EMOTION(0.1);
      }
      cmd->step(now);
    break;
    case ONGOING:{
      synDX.fire(MSG_GREET,0.0f); 
      synSX.fire(MSG_GREET,0.0f); 
      cmd->start(MODE_LINEAR,100,VARS(0,1,2,3),1.0f*random(0,2),random(10,21)/20.0f,1.0f*random(0,2),1.0f);
    }
    break;
  }
}


//-------------------------NET
void ch_net(Sense* senses,Actuator *actuator, Command *cmds, Command *cmd){
  //DISABLE_CMD();
  //timer for checking
  if(cmd->elapsed()>120000){
    cmd->step(now);
    paraimpu.check();
  }
  //timer for reacting
  if(now-cmd->mem.asLong[0]>240000){
    cmd->mem.asLong[0]=now;
    int tweets = paraimpu.consume();
    if(tweets){
      AROUSED(0.5+0.01*tweets);
      EMOTION(0.5+0.01*tweets);
      cmd->mem.asLong[2]=now;
      cmd->activity=ONGOING;
    }
  }
  //timer for tweeting
  if(now-cmd->mem.asLong[1]>600000){
    byte mm =random(0,2);
    //char buf[60];
    if(arousal<0.25 || emotion<0.25) {
      cmd->mem.asLong[1]=now;
      cmd->activity=ONGOING;
      cmd->mem.asLong[2]=now;
      paraimpu.say(random(SAD_0,SAD_2+1));
    } else if(arousal>0.8 || emotion>0.8){
      cmd->mem.asLong[1]=now;
      cmd->activity=ONGOING;
      cmd->mem.asLong[2]=now;
      paraimpu.say(random(HAPPY_0,HAPPY_2+1));
    }
  }
  
 //timer for fx
 if(now-cmd->mem.asLong[2]>5000){
   cmd->activity=INACTIVE;
 }
  
}

void bh_net(Sense* senses,Actuator *actuator, Command *cmds, Command *cmd){
      if(cmd->mem.asLong[3]%2) cmd->start(MODE_LINEAR,500,VARS(0,1,2,3),0.0f,0.0f,0.0f,0.0f);
      else cmd->start(MODE_SINGLE,1000,VARS(0,1,2,3),1.0f*random(0,2),random(10,21)/20.0f,1.0f*random(0,2),1.0f);
      cmd->mem.asLong[3]++;
}




//-------------------------SYN
//fired when happy
void ch_syn(Sense* senses,Actuator *actuator, Command *cmds, Command *cmd){
  
  //check serial
  if(cmd->elapsed()>100){
      cmd->step(now);
      Synapse *syn = cmd->mem.asByte[0] ? &synDX: &synSX;
      Synapse *synb = !cmd->mem.asByte[0] ? &synDX: &synSX;
      Message *msg = cmd->mem.asByte[0] ? &msg_dx:&msg_sx;
      byte timer = cmd->mem.asByte[0] ? 1:2;
      if(!syn->incoming(msg)){ //if no message
        cmd->mem.asByte[0]=!cmd->mem.asByte[0];
        msg->action=0;//empty message
        synb->restart();
      } else {
        if(msg->action>=MSG_LAST){
          syn->restart();
        }
        cmd->mem.asLong[timer]=now;
      }
  }

  //propagate msgs
  if(msg_dx.action || msg_sx.action) {
    cmd->activity=ONGOING;
    if(msg_dx.action==MSG_TOUCH) synSX.fire(MSG_TOUCH,msg_dx.value.asByte[0],msg_dx.value.asByte[1],msg_dx.value.asByte[2],(byte)((int)msg_dx.value.asByte[3]));
    if(msg_sx.action==MSG_TOUCH) synDX.fire(MSG_TOUCH,msg_sx.value.asByte[0],msg_sx.value.asByte[1],msg_sx.value.asByte[2],(byte)((int)msg_sx.value.asByte[3]));
  
    if(msg_dx.action==MSG_CANDLE) synSX.fire(MSG_CANDLE,0.0f);
    if(msg_sx.action==MSG_CANDLE) synDX.fire(MSG_CANDLE,0.0f);
  
    if(msg_dx.action==MSG_GREET) synSX.fire(MSG_GREET,0.0f);
    if(msg_sx.action==MSG_GREET) synDX.fire(MSG_GREET,0.0f);
  } else cmd->activity=INACTIVE;

}

void bh_syn(Sense* senses,Actuator *actuator, Command *cmds, Command *cmd){

  float rd=msg_dx.value.asByte[0]/255.0;
  float gd=msg_dx.value.asByte[1]/255.0;
  float bd=msg_dx.value.asByte[2]/255.0;
  float ld=msg_dx.value.asByte[3]/255.0;
  float rs=msg_sx.value.asByte[0]/255.0;
  float gs=msg_sx.value.asByte[1]/255.0;
  float bs=msg_sx.value.asByte[2]/255.0;
  float ls=msg_sx.value.asByte[3]/255.0;
  if(msg_dx.action==MSG_TOUCH && msg_sx.action==MSG_TOUCH){
      //touching from dx && sx!!
      cmd->start(MODE_LINEAR,500,VARS(0,1,2,3),(rd+rs)/2,(gd+gs)/2,(bd+bs)/2,(ld+ls)/2);
  } else if(msg_dx.action==MSG_TOUCH){
      //touching from dx only   
      cmd->start(MODE_LINEAR,500,VARS(0,1,2,3),rd,gd,bd,ld);
  } else if(msg_sx.action==MSG_TOUCH){
      //touching from sx only   
      cmd->start(MODE_LINEAR,500,VARS(0,1,2,3),rs,gs,bs,ls);
  } else if(msg_dx.action==MSG_CANDLE || msg_sx.action==MSG_CANDLE){
    cmd->start(MODE_LINEAR,100,VARS(0,1,2,3),0.0f,0.0f,0.0f,0.0f);
  } else if(msg_dx.action==MSG_GREET || msg_sx.action==MSG_GREET){
    cmd->start(MODE_LINEAR,100,VARS(0,1,2,3),1.0f*random(0,2),1.0f*random(0,2),1.0f*random(0,2),1.0f);
  }
}

//----------------SENSORS
float sense_sound(){
  return analogRead(pinSound)/1023.0f;
}

float sense_touch(){
  return (float)cs_4_2.capacitiveSensor(40);
}

float sense_light(){
  return (1.0f-analogRead(pinLight)/1023.0f);
}

float sense_temperature(){
  return (4.8 * analogRead(pinTemp) * 100.0)/1024.0;
}

void setup_light(){
}
void setup_sound(){
}
void setup_temperature(){
}
void setup_touch(){
  cs_4_2.reset_CS_AutoCal();
}


//-----------------ACTUATORS

void actuator_lamp(Status *status){
   analogWrite(pinRed, byte(status->asFloat[0]*status->asFloat[3]*255));
   analogWrite(pinGreen, byte(status->asFloat[1]*status->asFloat[3]*255));
   analogWrite(pinBlue, byte(status->asFloat[2]*status->asFloat[3]*255));
}

void actuator_viber(Status *status){
   digitalWrite(pinViber, status->asFloat[0]>=1.0f ? 1:0);
}

