#ifndef _THING_ACTUATOR_
#define _THING_ACTUATOR_

#include "Command.h"
#include "Clock.h"
#include "Sense.h"


typedef void (*act_fn)(Status*);

class Actuator {
  public:
  
   byte id;
   long last_update;
   act_fn act;
   Status status;
   Status tmp;

   
   void setup(int ac,act_fn aa);
   
   void update(Sense *senses, Actuator *actuators, Command* cmd);
   
   
   void single_step(Command *command);
   void single_update(Command* command, Status *temp);
   void linear_step(Command *command);
   void linear_update(Command* command, Status *temp,float dt);
   
};


#endif

