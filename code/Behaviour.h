#ifndef _THING_BEHAVE_
#define _THING_BEHAVE_

#include "Actuator.h"
#include "Sense.h"
#include "Command.h"

typedef void (*cond_fn)(Sense*,Actuator*, Command *, Command*);
typedef void (*behave_fn)(Sense*,Actuator *, Command *, Command*);

class Behaviour {
  public:
  
  Command *cmd;
  behave_fn bfn;
  cond_fn cfn;
  
  
  void setup(int act,cond_fn fnc,behave_fn fnb, Command *command){
  bfn=fnb;
  cfn=fnc;
  cmd=command;
  cmd->actuator=act;
  cmd->bh = this;
}
 
void update(Sense *senses,Actuator *actuator, Command * command){
  if(cmd->activity) bfn(senses,actuator,command,cmd);
}

void check(Sense *senses,Actuator *actuator, Command * command){
  cfn(senses,actuator,command,cmd);
}
  
};







#endif

