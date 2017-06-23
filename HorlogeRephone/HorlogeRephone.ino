//Include the necessary libraries 
#include <LCheckSIM.h>
#include <LGSM.h>
#include <LGPS.h>
#include <stdio.h>
#include <string.h>
#include <wire.h>
#include <LSensorHub.h>
#include <string.h>


char receiveNumber[20]= {0};              // Number that you can get texts from
char num[20] = {0};                       // Number that calls you
char numTest[20] = "0031623707881";       // Number for testing purposes
char buf_contex[100];                     // Buffer for the texts that the rephone sends

    char buffer[100] = {0,};
    unsigned KeyValue = 0;
  
    //the date time
    unsigned char *utc_date_time = 0;


int fallSpeed = 100000;
int const history = 2;
//Accelero X, Y and Z values
long aXcurrent;
long aYcurrent;
long aZcurrent;

long aXlast;
long aYlast;
long aZlast;

//Set up
void setup()
{
    Serial.begin(115200);
}

//set a loop
void loop()
{

    //gets the data from the Accelero sensor
    LSensorHub.GetAccData(&aXcurrent, &aYcurrent, &aZcurrent);
    
   
   
   if(aXcurrent > (aXlast + fallSpeed) || aXcurrent < (aXlast - fallSpeed) ||
      aYcurrent > (aYlast + fallSpeed) || aYcurrent < (aYlast - fallSpeed) ||
      aZcurrent > (aZlast + fallSpeed) || aZcurrent < (aZlast - fallSpeed)) 
   {
        warning();
   }
   if(aXcurrent != aXlast)
   {
     aXlast = aXcurrent;
   }
   if(aYcurrent != aYlast)
   {
     aYlast = aYcurrent;
   }
   if(aZcurrent != aZlast)
   {
     aZlast = aZcurrent;
   }
    
  ReceiveCall();
  
    //checks if the sms available
    if(LSMS.available())
    {
    
      LSMS.remoteNumber(receiveNumber, 20);
      LSMS.remoteContent(buf_contex, 50);
   
      if(strcmp("Nieuwnummer", (char*)buf_contex) == 0)
      {
      
        if(LSMS.ready())
        {
           LSMS.beginSMS(receiveNumber);
           LSMS.print("Je nummer is nu toegevoegd");
           if(LSMS.endSMS()) Serial.println("SMS send");
           LSMS.flush();
         }
      }
    } 
}

void warning()
{

  if(LGPS.check_online())
  {
           utc_date_time = LGPS.get_utc_date_time();
           sprintf(buffer, "Help ik ben gevallen om dit tijdstip: UTC:%d:%d:%d, \nHier lig ik: https://www.google.nl/maps/place/%f,%f", 
                            utc_date_time[3], utc_date_time[4],utc_date_time[5], 
                            LGPS.get_latitude() ,LGPS.get_altitude());
                  
          if(LSMS.ready())
          {
            LSMS.beginSMS(numTest);
            LSMS.print(buffer);
            LSMS.endSMS();
            LSMS.flush();  

          }
       }        
       if(LSMS.endSMS()) Serial.println("SMS sent ok!");
       
}

void ReceiveCall()
{
  if(LVoiceCall.getVoiceCallStatus() == RECEIVINGCALL)
    {  
        LVoiceCall.retrieveCallingNumber(num,20);
        LVoiceCall.answerCall();
    }
}

