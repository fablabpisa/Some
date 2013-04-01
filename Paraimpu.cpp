
#include "Paraimpu.h"

  //SET MAC & IPs in Thing.h
  byte mac[6] = { MAC };
  IPAddress ip( IP );
  IPAddress dnss( DNS );
  IPAddress gway( GWAY );
  IPAddress server( 156, 148, 18, 164 );  
  
void Paraimpu::setup(){
  Ethernet.begin(mac, ip,dnss,gway);
  delay(2000);
}



byte Paraimpu::say(byte msg){

  
  if (!client.connected() && client.connect(server, 80)) {

    int l=67+36+36;
    switch(msg){
      case SAD_0:l+=40; break;
      case SAD_1: l+=33;break;
      case SAD_2: l+=18;break;
      case HAPPY_0:l+=17;break;
      case HAPPY_1: l+=36;break;
      case HAPPY_2: l+=25;break;
    }
    client.print(F("POST /data/new HTTP/1.1\nHost: paraimpu.crs4.it\nContent-Type: application/json\nContent-Length: "));
    client.println(l, DEC);

    //YOU NEED A VALID PARAIMPU TOKEN!!
    client.print(F("\n{\"token\":\"779a7fd7-602d-4158-a1b2-e60e8b5fd63a\",\"content-type\":\"application/json\",\"data\":\""));

    switch(msg){
      case SAD_0:client.print(F("feel so lonely, looking for #SOME one :-("));break;
      case SAD_1: client.println(F("#SOMEbody around with #SOME love?"));break;
      case SAD_2: client.println(F("#SOME days are dim"));break;
      case HAPPY_0: client.print(F("feeling awe #SOME !!!"));break;
      case HAPPY_1: client.println(F("need #SOME love? Come and get it :-*"));break;
      case HAPPY_2: client.println(F("#SOME days are bright :-D"));break;
    }
    //YOU NEED A VALID PARAIMPU OWNER!!
    client.print(F("\",\"owner\":\"7fd0e61c-e7ce-4de6-8eec-7dbbb23d3af8\"}\n\n"));
  }    
  client.stop();
return 0;
}


byte Paraimpu::check(){
  
  byte ret=0;
  if(now-last_sensed<SENSE_DELAY) return ret;
  
  if(client.connected() && last_sensed_request!=0 && (now-last_sensed_request)>REQUEST_DELAY){
    
    #ifndef AWE_lAMP
    #ifdef DEBUG
    Serial.println("           ");
    Serial.println("reading ...");
    #endif
    #endif;
    char c;
    while (client.available()){
        
        c = client.read();
        if(c=='{') {
          valueof[SOME]++;
          //break;
        }
        #ifndef AWE_LAMP
        #ifdef DEBUG
        Serial.print(c);
        #endif
        #endif
     }
    client.stop();
    last_sensed = now;
    last_sensed_request = 0;
  } else if(last_sensed_request==0) {
    if(client.connect(server, 80)){
      #ifndef AWE_LAMP
      #ifdef DEBUG
      Serial.println("Connected");
      #endif
      #endif      
      //YOU NEED A VALID PARAIMPU URL
      client.println(F("GET /use?token=66b05614-81a7-41db-8202-8d7f57af9c08 HTTP/1.1\nHost: paraimpu.crs4.it\n"));
      last_sensed_request=now;
    }
  }
  return ret;
}


