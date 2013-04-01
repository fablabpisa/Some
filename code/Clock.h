#ifndef __CLOCK__
#define __CLOCK__


#define START_CLOCK() unsigned long now; void getNow(){now=millis();}

extern unsigned long now;
void getNow();

#endif

