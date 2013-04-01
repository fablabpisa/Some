#include "Actuator.h"
#include <Arduino.h>
#include "Thing.h"



void Actuator::setup(int ac,act_fn aa){
act=aa;
id=ac;
}

void Actuator::single_step(Command* cmd){
  for(int i=0;i<8;i++){
     if(HAS_VAR(cmd->var,i)){
        cmd->delta.asFloat[i]=(cmd->target.asFloat[i]-status.asFloat[i]);
     }          
  }
}

void Actuator::single_update(Command *cmd,Status *tmp){
  for(int i=0;i<8;i++){
    if(HAS_VAR(cmd->var,i)){
       tmp->asFloat[i]+=cmd->delta.asFloat[i];
    }
  }
}
void Actuator::linear_step(Command* cmd){
  for(int i=0;i<8;i++){
     if(HAS_VAR(cmd->var,i)){
        cmd->delta.asFloat[i]=(cmd->target.asFloat[i]-status.asFloat[i])/cmd->time;
     }          
  }
}

void Actuator::linear_update(Command *cmd,Status *tmp,float dt){
  for(int i=0;i<8;i++){
    if(HAS_VAR(cmd->var,i)){
       tmp->asFloat[i]+=cmd->delta.asFloat[i]*dt;
    }
  }
}


void Actuator::update(Sense *senses, Actuator *actuators, Command* cmd){

  memcpy(&tmp,&status,sizeof(Status));
  
  for(int i=0;i<BEHAVIOURS;i++){
    if(cmd[i].activity && cmd[i].actuator==id){
      
      //I have a command which is active and it's mine: oh joy!
      if(!cmd[i].processed || cmd[i].pre!=cmd[i].activity) {
        //ok, first, let's call the behaviour
        cmd[i].pre=cmd[i].activity;
        cmd[i].bh->update(senses, actuators, cmd);
        
        if(cmd[i].activated) {//something to do
        //go for the step calculation
          switch(cmd[i].mode){
            case MODE_LINEAR:
            linear_step(&cmd[i]);
            break;
            case MODE_SINGLE:
            single_step(&cmd[i]);
            break;
          }
          cmd[i].processed=1;
        }
      }
      
      if(cmd[i].activated) {
      //let's execute it!
      float dt = now-last_update;
       if(dt>cmd[i].time) dt=cmd[i].time; //clamp time
       switch(cmd[i].mode){
          case MODE_LINEAR:
           linear_update(&cmd[i],&tmp,dt);
          break;
          case MODE_SINGLE:
           single_update(&cmd[i],&tmp);
          break;
        }
      }
      //is this command ended?
      if(now-cmd[i].activated>=cmd[i].time){
          cmd[i].activated=0;
          cmd[i].processed=0;
          if(cmd[i].activity==EXITING) cmd[i].activity=INACTIVE;
       }       
    }
  }
    
  memcpy(&status,&tmp,sizeof(Status));

  //clamp
  for(int i=0;i<STATUS_SIZE;i++){
    if(status.asFloat[i]>1) status.asFloat[i]=1;
    else if(status.asFloat[i]<0) status.asFloat[i]=0;
  }
  
  
  
  last_update=now;
  act(&status);
}

