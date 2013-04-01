#ifndef _THING_
#define _THING_


extern float arousal;
extern float emotion;

#define AROUSAL_UNIT 0.01
#define MIN_AROUSAL 0.1
#define AROUSED(a) arousal=(arousal+(a))<MIN_AROUSAL ? MIN_AROUSAL:((arousal+(a))>1 ? 1:(arousal+(a)))
#define AROUSAL_LIMIT(a) (((a)<MIN_AROUSAL) ? MIN_AROUSAL:(((a)>1) ? 1:(a)))
#define AROUSAL_DOWN(t) arousal=AROUSAL_LIMIT(arousal-arousal*t/120000)


#define EMOTION_UNIT 0.01
#define MIN_EMOTION 0.1
#define EMOTION(a) emotion=(emotion+(a))<MIN_EMOTION ? MIN_EMOTION:((emotion+(a))>1 ? 1:(emotion+(a)))
#define EMOTION_LIMIT(a) (((a)<MIN_EMOTION) ? MIN_EMOTION:(((a)>1) ? 1:(a)))
#define EMOTION_DOWN(t) emotion=EMOTION_LIMIT(emotion-emotion*t/600000)
#define EMOTION_DOWN_WITH_TEMP(t,T) emotion=EMOTION_LIMIT(emotion-emotion*t/(300000+300000*(1-(abs(T-20.0))/T)))


#define R() (1.0f*emotion)
#define G() (0.0f)
#define B() (1.0f*(1-emotion))
#define COLOR() R(),G(),B()
#define BRIGHTNESS() (constrain((1.0-senses[LIGHT].average)+arousal,0.2f,1.0f))


//-----------------------------------------------------------CHOOSE LAMP
//#define AWE_LAMP
#define SOME_LAMP
//#define THING_LAMP
//#define DEBUG
//-----------------------------------------------------------CHOOSE LAMP




#ifdef AWE_LAMP
  #define MY_ID 10
  #define AWE_ID 10
  #define SOME_ID 20
  #define THING_ID 30
  #define MAC 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
  #define IP 192, 168, 1, 20
  #define DNS 8,8,8,8
  #define GWAY 192,168,1,10
#endif

#ifdef SOME_LAMP
  #define MY_ID 20
  #define AWE_ID 10
  #define SOME_ID 20
  #define THING_ID 30
  #define MAC 0xDE, 0xAD, 0xCE, 0xEF, 0xFE, 0xED
  #define IP 192, 168, 1, 30
  #define DNS 8,8,8,8
  #define GWAY 192,168,1,10
#endif


#ifdef THING_LAMP
  #define MY_ID 30
  #define AWE_ID 10
  #define SOME_ID 20
  #define THING_ID 30
  #define MAC 0xDE, 0xAD, 0xDE, 0xEF, 0xFE, 0xED
  #define IP 192, 168, 1, 40
  #define DNS 8,8,8,8
  #define GWAY 192,168,1,10
#endif


//#define IP 131, 114, 180, 112

//---------------------THING
#define SENSES 4
#define BEHAVIOURS 7
#define ACTUATORS 2



#define MSG_HELLO 1
#define MSG_TOUCH 2
#define MSG_SYNCH 3
#define MSG_CANDLE 4
#define MSG_GREET 5
#define MSG_LAST 6



typedef struct _neurotransmitter {
  byte action;
  union {
    float asFloat;
    byte asByte[4];
    int asInt[2];
    long asLong;
  } value;
} Message;



//---------------------SENSORS DEFINITIONS


//Capacitive Sensor PINS
#define pinCapIn A4
#define pinCapOut A3

//Other PINS
#define pinTemp A1
#define pinSound A2
#define pinLight A0
#define pinRed 3
#define pinGreen 5
#define pinBlue 6
#define pinViber A5

#define LIGHT 0
#define TOUCH 1
#define SOUND 2
#define TEMPERATURE 3


#define START_THING() float arousal=0.5; \
float emotion=0.5; \
Sense senses[SENSES]; \
Behaviour behaviours[BEHAVIOURS]; \
Actuator actuators[ACTUATORS]; \
Command commands[BEHAVIOURS]; \
Message msg_dx; \
Message msg_sx


//Serials PINS
#define SYN_DX_RX 8
#define SYN_DX_TX 9
#define SYN_SX_RX 7
#define SYN_SX_TX 2

#define START_CONNECTIONS() Paraimpu paraimpu; \
Synapse synDX(SYN_DX_RX,SYN_DX_TX); \
Synapse synSX(SYN_SX_RX,SYN_SX_TX)


#define BEGIN_CONNECTIONS()   synDX.begin(9600); \
  synSX.begin(9600); \
  synDX.listen(); \
  paraimpu.setup(); \
  pinMode(pinViber,OUTPUT)




float sense_light();
float sense_sound();
float sense_temperature();
float sense_touch();


void setup_light();
void setup_sound();
void setup_temperature();
void setup_touch();



//--------------------BEHAVIOUR DEFINITIONS

#include "Clock.h"
#include "Sense.h"
#include "Behaviour.h"
#include "Actuator.h"
#include "Command.h"

//response to touch
#define CMD_TOUCH 0
void ch_touch(Sense* senses,Actuator *actuator, Command *command, Command *cmd);
void bh_touch(Sense* senses,Actuator *actuator, Command *command, Command *cmd);

#define CMD_DANCE 1
//response to audio
void ch_dance(Sense* senses,Actuator *actuator, Command *command, Command *cmd);
void bh_dance(Sense* senses,Actuator *actuator, Command *command, Command *cmd);

#define CMD_SYN 2
//fired when happy
void ch_syn(Sense* senses,Actuator *actuator, Command *command, Command *cmd);
void bh_syn(Sense* senses,Actuator *actuator, Command *command, Command *cmd);

#define CMD_GREET 3
//fired when someone pass by
void ch_greet(Sense* senses,Actuator *actuator, Command *command, Command *cmd);
void bh_greet(Sense* senses,Actuator *actuator, Command *command, Command *cmd);

#define CMD_NET 4
//fired when really sad
void ch_net(Sense* senses,Actuator *actuator, Command *command, Command *cmd);
void bh_net(Sense* senses,Actuator *actuator, Command *command, Command *cmd);

#define CMD_IDLE 5
//fake behaviour to update emotion/arousal with the passing of time
void ch_idle(Sense* senses,Actuator *actuator, Command *command, Command *cmd);
void bh_idle(Sense* senses,Actuator *actuator, Command *command, Command *cmd);


//--------------------ACTUATORS DEFINITIONS



#define LAMP_RED 0
#define LAMP_GREEN 1
#define LAMP_BLUE 2
#define LAMP_BRIGHT 3

void actuator_lamp(Status* status);

void actuator_viber(Status* status);



//--------------------------DEBUG

#define DEBUG_SENSES()     Serial.print("SENS "); \
    for(int i=0;i<SENSES;i++){ \
      Serial.print(" # "); \
      Serial.print(senses[i].current); \
      Serial.print("/"); \
      Serial.print(senses[i].average); \
    } \
      Serial.print(" # "); \
      Serial.print(cmds[CMD_DANCE].mem.asFloat[2]); \
      Serial.print("/"); \
      Serial.print(cmds[CMD_DANCE].mem.asFloat[3]/cmds[CMD_DANCE].step()); \
      Serial.println()

#define DEBUG_LAMP()         Serial.print(F("LAMP ")); \
    Serial.print(actuator[0].tmp.asFloat[0]); \
    Serial.print(" "); \
    Serial.print(actuator[0].tmp.asFloat[1]); \
    Serial.print(" "); \
    Serial.print(actuator[0].tmp.asFloat[2]); \
    Serial.print(" "); \
    Serial.println(actuator[0].tmp.asFloat[3])


#define DEBUG_EMOTION() Serial.print(F("AROUSAL/EMOTION ")); \
    Serial.print(arousal*100.0); \
    Serial.print(" # "); \
    Serial.println(emotion*100.0) 

#define DEBUG_COMMANDS()   Serial.print("CMDS "); \
    for(int i=0;i<BEHAVIOURS;i++){ \
      Serial.print(" # "); \
      Serial.print(cmds[i].activity); \
    } \
    Serial.println()




#endif
