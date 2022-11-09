#include <Arduino.h>
#include <SPI.h>
//#include <Waveshare_ILI9486_GFX.h>
/*
 * Speeduino Display Dashboard
 *          BY
 *      DIOGO-SILVA - Programação
 *      PEDRO-SILVA - Eletrónica
 */
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv display;       // hard-wired for UNO / MEGA shields anyway.
#include  <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h> // Hardware-specific library
#include <Waveshare4InchTftShield.h>
//#include <Fonts/tftarduino12pt7b.h>
#include <EEPROM.h>


namespace{Waveshare4InchTftShield Waveshield;Adafruit_GFX &tft = Waveshield;}

/*
const int XP=6,XM=A2,YP=A1,YM=7; //ID=0x9341
const int TS_LEFT = 907, TS_RT = 136, TS_TOP = 942, TS_BOT = 139;
*/
/*
int16_t BOXSIZE;
int16_t PENRADIUS = 1;
uint16_t ID, oldcolor, currentcolor;
*/
uint8_t Orientation = 1;    //PORTRAIT
 
// Color definitions
#define BLACK 0x0000
#define NAVY 0x000F
#define DARKGREEN 0x03E0
#define DARKCYAN 0x03EF
#define MAROON 0x7800
#define PURPLE 0x780F
#define OLIVE 0x7BE0
#define LIGHTGREY 0xC618
#define DARKGREY 0x7BEF
#define DARKBLUE 0x001F
#define BLUE    0x045D
#define GREEN 0x07E0
#define CYAN 0x07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define DARKORANGE 0xFD20
#define ORANGE  0xFA60
#define GREENYELLOW 0xAFE5
#define PINK 0xF81F

//static uint32_t oldtime = millis();
uint8_t speedyResponse[100]; //The data buffer for the Serial data. This is longer than needed, just in case
uint8_t byteNumber[2];  // pointer to which uint8_t number we are reading currently
uint8_t readiat; // iat doesn't need to be updated very ofter so
int iat;   // to store coolant temp
uint8_t readclt; // clt doesn't need to be updated very ofter so
int clt;   // to store coolant temp
int tps;
int bat;
int adv;
 int valord2 =0;
 int valora5 = 0;
 int valorvoltagea5;
  bool toggle = false;
  bool toggleBackrpm = true;
  bool toggleTps = true;
  bool toggletemp = true;
  bool togglea5 = false;
  bool toggleUpText = false;
  bool toggleEcuMap = false;
  bool togglebat = true;
  bool toggleign = true;
  bool toggleicotemp = true;
  bool toggleEcraSel = false;
  //false -> Eco
  //true -> Power
  int rpmMax;
  double largRpmDash;
  int d, c,h;
  TSPoint tp;
  int ColorReg[] = {GREEN, GREEN, GREENYELLOW, YELLOW, DARKORANGE,RED, RED};
  int ColorTem[] = {YELLOW, DARKORANGE, RED, WHITE, BLUE,GREEN, PINK};


  int ColorReg2D[][9] = { {WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE},
                         {WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE},
                         {WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE},
                         {WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE},
                         {WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE},
                         {WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE},
                         {GREEN,GREEN,GREENYELLOW,YELLOW,DARKORANGE,RED,RED,RED,RED},
                         {GREEN, GREEN, GREENYELLOW, YELLOW, DARKORANGE,RED, RED,RED,RED},
                         {GREEN,GREEN,GREENYELLOW,YELLOW,DARKORANGE,RED,RED,RED,RED},
                         {GREEN,GREEN,GREENYELLOW,GREENYELLOW,YELLOW,DARKORANGE,RED,RED,RED},
  }
;

  double valorRpmCorr[] = {0 ,470, 235, 156.6667, 117.5, 94, 78.3333, 67.1429, 58.75, 52.2222, 47, 42.7273, 39.1667};
  //int ColorRegDark[] = {DARKGREEN,DARKGREEN, DARKGREEN, OLIVE,OLIVE,MAROON,MAROON};
  int ColorDot = GREEN;
  int ColorDotDark = GREEN;
  int rpmCorr;
  int lastdash;
  int ecraSel = 1;
  int pageINFO = 1;
  int pageBAR = 1;
  int brightness = 0;
  int corSel = 0;
  double corCorr;
  int rpmRedline;

unsigned int rpm;  //rpm and PW from speeduino
float afr;
float mapData;
int8_t psi;
float afrConv;
uint8_t cmdAdata[50] ; 







void setup () 
{
   
  display.reset();
//ID = display.readID();display.begin(ID);
  Serial.begin(115200);
  while (!Serial);
  SPI.begin();
  Waveshield.begin();

tft.setRotation(Orientation);
 
  
  
  
  tft.fillScreen(BLACK);
  delay(500);
  
  tft.setTextColor(WHITE);
  tft.setCursor(130,240);
  tft.setTextSize(10);


  tft.print("O");
  delay(200);



  tft.fillCircle(240, 120, 110,WHITE);
  tft.fillCircle(240, 120, 110-(8),BLACK);
  //}
  
    tft.print("P");
  

  for(int i = 0; i<165; i++)
  {
  tft.fillRect(100 + i,100,4,8+(i/20),WHITE);
  delay(6);
  }
    for(int i = 0; i<25; i++)
  {
  tft.fillRect(265 - (i*2.3),100 + (i*1.3),18,18,WHITE);
  delay(6);
  }
  
  tft.print("E");
  

    for(int i = 0; i<165; i++)
  {
  tft.fillRect(215 + i,135 + (i/20),4,(8+(8.25))-(i/20),WHITE);
  delay(6);
  }
  
  tft.print("L" );
  
  delay(1500);

  pageChange();
  
  
  pinMode(2, OUTPUT);

  pinMode(A5,INPUT);
 
    tft.setTextSize(4);
  
     drawDataInicial();

  }


 void pageChange()
 {
  tft.fillScreen(BLACK);
   
  
   EEPROM.get(1, brightness);
  EEPROM.get(4, corSel);
  EEPROM.get(6, rpmMax);
  EEPROM.get(8, rpmRedline);
  
  rpmMax = rpmMax * 100;
  brightness = brightness * 10;
    rpm = 6000;
  clt= 0;
  rpmRedline = rpmRedline*100;
  largRpmDash = (480- ((rpmMax/250)*5))/(rpmMax/250);
  Waveshield.setScreenBrightness(10+brightness*10);
  drawDataInicial();

 }

//uint32_t old_ts;

#define BYTES_TO_READ 74
#define SERIAL_TIMEOUT 300
float rps;
boolean sent = false;
boolean received = false;
uint32_t sendTimestamp;

void loop () {

//drawData();
  requestData();
  //if(received) {//displayData();
    drawData();
    received = false;//}

    // valord2 = digitalRead(2);
     //valora5= analogRead(A5);
   // valorvoltagea5 = valora5 * (5.0/1023.0);
    }


void requestData() {
  if(sent && Serial.available()) {
    if(Serial.read() == 'A') {
      uint8_t bytesRead = Serial.readBytes(speedyResponse, BYTES_TO_READ);
      if(bytesRead != BYTES_TO_READ) {
        processData();
       for(uint8_t i = 0; i < bytesRead; i++) {
        }
        received = true;
        clearRX();
      } else {
        processData();
        received = true;
        rps = 1000.0/(millis() - sendTimestamp);
      }
      sent = false;
    } else Serial.read();
  } else if(!sent) {
    Serial.write('A');
    sent = true;
    sendTimestamp = millis();
  } else if(sent && millis() - sendTimestamp > SERIAL_TIMEOUT) {
    sent = false;
  }
}

void clearRX() {
  while(Serial.available()) Serial.read();
}


 /*
//display the needed values in serial monitor for debugging
void displayData() {
  Serial.print("RPM-"); Serial.print(rpm); Serial.print("\t");
  Serial.print("CLT-"); Serial.print(clt); Serial.print("\t");
  Serial.print("MAP-"); Serial.print(psi); Serial.print("\t");
  Serial.print("AFR-"); Serial.print(afrConv); Serial.println("\t");
  Serial.print("IAT-"); Serial.print(iat); Serial.print("\t");
  //Serial.print("TPS-"); Serial.print(tps); Serial.print("\t");
  Serial.print("BATT.V-"); Serial.print(bat); Serial.print("\t");
  //Serial.print("FLEX %-"); Serial.print(flex); Serial.print("\t");
  Serial.print("ADVANCE°-"); Serial.print(adv); Serial.print("\t");
}
*/

 
void drawData() { 
          //rpm = 0000;
    tp = Waveshield.getPoint() ;

    Waveshield.normalizeTsPoint(tp);

clt= 0;

    if(ecraSel == 1)
    {

tft.setTextSize(3);




    //MAPA SEC.
/*
    if((tp.x >= 243 && tp.x <= 475 )&&(tp.y >= 0&&tp.y<= 55))
    {
      toggleEcuMap = true;
      toggle = false;
    }
    else if((tp.x >= 5 && tp.x <= 238)&&(tp.y >= 0 && tp.y<= 55))
    {
      toggleEcuMap = false;
      toggle = false;
    }
*/




    if((tp.x >= 120 && tp.x <=240) && (tp.y >= 225 && tp.y <320))
    {
      pageINFO++;

      if(pageINFO > 4)
      {
        pageINFO = 1;
      }

      tft.fillRect(121,201, 119,119, BLACK);
    }
    if((tp.x > 240 && tp.x <= 480) && (tp.y >= 255 && tp.y < 320))
    {
      pageBAR++;

      if(pageBAR > 2)
      {
        pageBAR = 1;
      }
      tft.fillRect(241, 256, 238, 64,BLACK);
    }
    if((tp.x > 5 && tp.x <= 100)&& (tp.y >= 5 && tp.y < 50))
    {
      ecraSel++;

      if(ecraSel > 2)
      {
        ecraSel = 1;
      }
      toggleEcraSel = true;
      
    }
      
/*
      
     if (toggleEcuMap == true && toggle == false)
     {
      toggle = true;
      tft.fillRect(242,5,233,45,RED);
      tft.setCursor(320,16);
      tft.setTextColor(BLACK);
      tft.print("SPORT");
      tft.fillRect(5,5,233,45, DARKGREY);
      tft.setCursor(60,16);
      tft.setTextColor(BLACK);
      tft.print("ECONOMY");
      digitalWrite(2, HIGH);
      }
     if(toggleEcuMap == false && toggle == false){
      toggle = true; 
      tft.fillRect(5,5,233,45,GREEN );
      tft.setCursor(60,16);
      tft.setTextColor(BLACK);
      tft.print("ECONOMY");
      tft.fillRect(242,5,233,45,DARKGREY);
      tft.setCursor(320,16);
      tft.setTextColor(BLACK);
      tft.print("SPORT");
      digitalWrite(2, LOW);
      }


*/



      

  //Rotação

 

        if ( rpmRedline <= rpm ) {  for(int i = 0; i < 6;i++){tft.fillScreen(RED);tft.setTextColor(BLACK);tft.setTextSize(12);tft.setCursor(30,115);tft.print("SHIFT!");delay(15);}tft.fillScreen(BLACK); toggleUpText = true;toggle = false;drawDataInicial();}







  d=0;
    for(int i = 0; i < rpm/250; i++)
  {
    
    if(i== 0){d = 5;}
    else{d = d + (largRpmDash + 5);}

    if(i < 10)
    {
      ColorDot = GREEN;
      ColorDotDark = DARKGREEN;
    }
    else if(i >= 10 && i <13)
    {
      ColorDot = GREENYELLOW;
      ColorDotDark = OLIVE;
    }
    else if(i >= 13 && i <19)
    {
      ColorDot = YELLOW; 
      ColorDotDark = MAROON;
    }
    else if(i >=19 && i<22)
    {
      ColorDot = DARKORANGE;
    }
    else if(i >=22 && i<28)
    {
      ColorDot = RED;
    }
     
    //tft.drawRect( d-1 ,(180 * (pow(1.14563, -i)))-1, 15.7857143,47, ColorDot);
   // tft.fillRect( d ,140, 13.7857143,((28-i)*(1.6071428)), ColorDotDark);
    //tft.fillRect( d ,75+(180 * (pow(1.14563, -i))), 13.7857143,45, ColorDot);
    tft.fillRoundRect( d ,75+(180 * (pow(1.14563, -i))), largRpmDash,45,5, ColorDot);
    lastdash = i;
    }
//-(i)*(1.6071428))
  //}

 
 // tft.fillRect(10 + rpmCorr ,lastdash, 465 - rpmCorr,45, BLACK);
  //tft.fillRect( 17+d,lastdash, 13.7857143,45,WHITE);

for(int i = lastdash; i<rpmMax/250; i++)
{
  if (rpm >= 250  )
  {  
  d = d +(largRpmDash+5);
  tft.fillRect(d,75+(180 * (pow(1.14563, -(i+1)))), largRpmDash,45,BLACK);
  }
  
  else
  {
    tft.fillRect(5, 255,  largRpmDash,45,BLACK);
  }
  
}

/*
if(rpm > 100)
{
  rpm -= 100;
}
else
{
  rpm = 6000;
}
*/

  
  if(toggleBackrpm && rpm < 1000){tft.fillRect(260,180, 215,65,BLACK);toggleBackrpm = false;}
  if(!toggleBackrpm && rpm>= 1000){toggleBackrpm = true;}
  
  tft.setTextSize(9);
  tft.setCursor(260,180);
  tft.setTextColor(RED,BLACK);
  tft.print(rpm);
  


 tft.setTextSize(2);
 c = 0;
for(int i = 0; i<(rpmMax/1000); i++)
{
  //c = c + (67.14);
 
 c= c + valorRpmCorr[rpmMax/1000];
  tft.fillRoundRect(5+c, 55 +(180 * (pow(1.14563, -((i+1)*((rpmMax/250)/(rpmMax/1000)))))) , 3, 15,5, ColorReg2D[rpmMax/1000][i]);
  //tft.fillRect(5+c, 140, 2, 45 , BLACK);
  tft.setTextColor(ColorReg2D[rpmMax/1000][i]);
  tft.setCursor((c - 15), 55 + (180 * (pow(1.14563, -((i+1)*((rpmMax/250)/(rpmMax/1000)))))));
  tft.print(i+1);
  //tft.print(round(i/corCorr));
}



    

/*
  if(pageBAR == 1)
 {*/
   //Posição da borboleta
  tft.setTextColor(MAGENTA, BLACK);
  tft.setCursor(100,15);
  tft.setTextSize(3);
  tft.print(" ");tft.print(tps);tft.print(" ");  
  tft.setCursor(165,15);tft.print("%");
 
  tft.fillRoundRect(195 + (tps*2.8), 5, 280 - (tps*2.8), 45,5, PURPLE);
  tft.fillRoundRect(195 , 5 , tps *(2.8), 45,5,MAGENTA);
  /*
    if(toggleTps && (tps <= 10 || tps < 100)){tft.fillRect(320,50, 210,65,BLACK);toggleTps = false;}
  if(!toggleTps && (tps> 10 || tps == 100)){toggleTps = true;}
  */ 
//}
 
  
  if(pageBAR == 1)
  {
  
  //Mistura
  tft.setTextSize(3);            // Draw 2X-scale text
  tft.setTextColor(BLUE,BLACK);
  tft.setCursor(250,262);
  tft.print("AFR-");
  tft.setTextSize(3);
  tft.setCursor(330,262);
  tft.print(afrConv, 1);
  h= 0;
  for(int i = 0; i < 28; i++)
  {

  if(i >= 0 && i<=3)
  {
    ColorDot= RED;
  }
  else if(i >= 4 & i <= 8)
  {
    ColorDot = DARKORANGE;
  }
  else if(i > 8 && i <=11)
  {
    ColorDot = YELLOW;
  }
  else if(i > 11 && i < 13)
  {
    ColorDot = GREENYELLOW;  
  }
  else if(i >= 13 && i <=16)  
  {
    ColorDot = GREEN;
  }
  else if(i > 15 && i <= 16)
  {
    ColorDot = GREENYELLOW;  
  }
  else if(i > 16 && i <=18)
  {
    ColorDot = YELLOW;
  }
  else if(i >= 18 & i <= 23)
  {
    ColorDot = DARKORANGE;
  }
  else  if(i >= 23 && i<=27)
  {
    ColorDot= RED;
  }
  
    h = h + 8.03571429;

    tft.fillRect(250 + (h-8.03571429) , 292 , 8.03571429, 22,ColorDot);
  }
  
  tft.fillRect(250 + (afrConv*8.03571429), 292,  4.03571429, 22, BLUE);
  
  }

  if(pageBAR == 2)
  {
  //Turbo
  tft.setTextSize(3);
  tft.setTextColor(YELLOW,BLACK);
  tft.setCursor(250,262);
  tft.print("MAP-");
  tft.setTextSize(3);
  tft.setCursor(330,262);
  tft.print(" ");tft.print(mapData);tft.print(" ");
  tft.fillRoundRect(250 + (mapData *1.125), 292, 225 - ((mapData)*1.125), 22,5, OLIVE);
  tft.fillRoundRect(250 , 292 , (mapData*1.125), 22,5,YELLOW);
  }

  if(pageINFO == 1)
  {
  //Temperatura do Motor
 
 clt = (clt-40);
 if( clt < 100 && toggletemp == true){toggletemp = false; tft.fillRect(121,265,119,54,BLACK);}
if( clt > 100 && toggletemp == false){toggletemp = true;}
  tft.setTextColor(RED,BLACK);
  tft.setTextSize(4);
  tft.setCursor(135,225);
  tft.print("TEMP");
  tft.setTextSize(4);
  tft.setCursor(145,265);
  tft.setTextColor(RED,BLACK);tft.print(clt);tft.setCursor(215,265);tft.setTextSize(1); tft.print("o");tft.setTextSize(2); tft.print("C");
  //tft.print((clt-40)/10);       // offset -40  C readout ( removed -40to get F readout)
  }
  /*
  if((((clt-40))>= 105) && toggleicotemp)
  {
    tft.setCursor(100, 215);
    tft.setTextSize(1);
    tft.setTextColor(RED,BLACK);
    tft.setFont(&tftarduino12pt7b);
    tft.print("A");
    tft.setFont();
    toggleicotemp =false;
  }
  else if((((clt-40))< 105) && toggleicotemp == false){
        tft.setCursor(100, 215);
    tft.setTextSize(1);
    tft.setTextColor(BLACK,BLACK);
    tft.setFont(&tftarduino12pt7b);
    tft.print("A");
    tft.setFont();
  }
  */



  if(pageINFO == 2)
  {
//Temperatura da Admissão
//iat = 540;
iat = (iat-40);
if( iat < 10 && toggletemp == true){toggletemp = false; tft.fillRect(121,265,119,54,BLACK);}
if( iat > 10 && toggletemp == false){toggletemp = true;}
   tft.setTextColor(YELLOW,BLACK);
  tft.setTextSize(4);
  tft.setCursor(145,225);
  tft.print("IAT");
  tft.setTextSize(4);
  tft.setCursor(145,265);
  tft.setTextColor(YELLOW,BLACK);tft.print(iat);tft.setCursor(215,265);tft.setTextSize(1); tft.print("o");tft.setTextSize(2); tft.print("C");
    // offset -40  C readout ( removed -40to get F readout)
  }


 if(pageINFO == 3)
{
//Voltagem da Bateria
if( bat < 10 && togglebat == true){togglebat = false; tft.fillRect(121,265,119,54,BLACK);}
if( bat >= 10 && togglebat == false){togglebat = true;}
  tft.setTextSize(4);
  tft.setTextColor(WHITE,BLACK);
  tft.setCursor(145,225);
  tft.print("BAT");
  tft.setTextSize(4);
  tft.setCursor(170,265);
  tft.print(bat/10);


/*
  tft.setTextSize(2);
  tft.setTextColor(GREEN,BLACK);
  tft.setCursor(300,90);
  tft.print("FLEX %-");
  tft.setTextSize(3);
  tft.setCursor(390,85);
  tft.print(flex);
*/                
}

if(pageINFO == 4)
{        
//Avanço da ignição
if( adv < 10 && toggleign == true){toggleign = false; tft.fillRect(121,265,119,54,BLACK);}
if( adv >= 10 && toggleign == false){toggleign = true;}
  tft.setTextSize(4);
  tft.setTextColor(GREEN,BLACK);
  tft.setCursor(145,225);
  tft.print("IGN"); 
  tft.setTextSize(4);
  tft.setCursor(170,265);
  tft.print(adv);
} 
  //delay(10);  
/*
       if(valorvoltagea5 > 4  )
     {
      tft.fillScreen(BLUE);
      tft.setTextSize(6);
      tft.setTextColor(BLACK);
      tft.setCursor(60,140);
      tft.print("L.CONTROL");
      tft.fillScreen(BLACK);
      togglea5 =false;
      toggle = false;
      }
     else if(valorvoltagea5 <= 4 && togglea5 == false)
     {
      togglea5 = true;
      toggle = false;
      //drawData();
      }
*/


//Do the outlines
      tft.drawRoundRect(120,200, 120,120,5,ColorTem[corSel]);//INFO
  tft.drawRoundRect(240,175, 240,80,5,ColorTem[corSel]);//RPM
 tft.drawRoundRect(0,0,480,320,5,ColorTem[corSel]);//OUTLINE
  tft.drawRoundRect(0,0,480,55,5,ColorTem[corSel]);//BOTTOM
    tft.drawRoundRect(240,255,240,65,5,ColorTem[corSel]);//TPS


    if(toggleEcraSel)
    {
      tft.fillScreen(BLACK); 
      toggleEcraSel = false;
      drawDataInicial();
    }
  
    }
    if(ecraSel == 2)
    {

     if((tp.x > 5 && tp.x <= 100)&& (tp.y >= 270 && tp.y < 315))
      {
      ecraSel++;

      if(ecraSel > 2)
      {
        ecraSel = 1;
      }
     toggleEcraSel = true;
     
      }
    

      if((tp.x > 430 && tp.x <= 475)&& (tp.y >= 5 && tp.y < 50))
      {
      brightness = brightness + 10;

      if(brightness > 100)
      {
        brightness = 10;
      }
      }
      if((tp.x > 270 && tp.x <= 285)&& (tp.y >= 5 && tp.y < 50))
      {
      brightness = brightness - 10;

      if(brightness < 10)
      {
        brightness = 100;
      }
      }
    
    tft.setCursor(320,14);
    tft.setTextColor(WHITE,BLACK);
    tft.print(brightness);tft.print(" ");


     if((tp.x > 430 && tp.x <= 475)&& (tp.y >= 60 && tp.y < 105))
      {
      rpmMax = rpmMax + 1000;

      if(rpmMax > 9000)
      {
        rpmMax = 6000;
      }
      }
      if((tp.x > 270 && tp.x <= 285)&& (tp.y >= 60 && tp.y < 105))
      {
      rpmMax = rpmMax - 1000;

      if(rpmMax < 6000)
      {
        rpmMax = 9000;
      }
      }

    tft.setCursor(320,69);
    tft.setTextColor(WHITE,BLACK);
    tft.print(rpmMax);


     if((tp.x > 430 && tp.x <= 475)&& (tp.y >= 115 && tp.y < 160))
      {
      rpmRedline = rpmRedline + 100;

      if(rpmRedline > 9000)
      {
        rpmRedline = 5000;
      }
      }
      if((tp.x > 270 && tp.x <= 285)&& (tp.y >= 115 && tp.y < 160))
      {
      rpmRedline = rpmRedline - 100;

      if(rpmRedline < 5000)
      {
        rpmRedline = 9000;
      }
      }

    tft.setCursor(320,126);
    tft.setTextColor(WHITE,BLACK);
    tft.print(rpmRedline);


    if((tp.x > 65 && tp.x <= 110)&& (tp.y >= 215 && tp.y < 260))
    {
    corSel = 0;
    }
    if((tp.x > 115 && tp.x <= 160)&& (tp.y >= 215 && tp.y < 260))
    {
    corSel = 1;
    }
    if((tp.x > 165 && tp.x <= 210)&& (tp.y >= 215 && tp.y < 260))
    {
    corSel = 2;
    }
    if((tp.x > 215 && tp.x <= 260)&& (tp.y >= 215 && tp.y < 260))
    {
    corSel = 3;
    }
    if((tp.x > 265 && tp.x <= 310)&& (tp.y >= 215 && tp.y < 260))
    {
    corSel = 4;
    }
    if((tp.x > 315 && tp.x <= 360)&& (tp.y >= 215 && tp.y < 260))
    {
    corSel = 5;
    }
    if((tp.x > 365 && tp.x <= 410)&& (tp.y >= 215 && tp.y < 260))
    {
    corSel = 6;
    }

    


    
      if((tp.x > 290 && tp.x <= 475)&& (tp.y >= 265 && tp.y < 320))
      {
      EEPROM.update(1, brightness/10);
      EEPROM.update(4, corSel);
      EEPROM.update(6, rpmMax/100);
      EEPROM.update(8, rpmRedline/100);
      tft.fillScreen(BLACK);
      pageChange();
      
    }

 tft.drawRoundRect(0,0,480,55,5, ColorTem[corSel]);
    tft.drawRoundRect(0,55,480,55,5, ColorTem[corSel]);
    tft.drawRoundRect(0,110,480,55,5, ColorTem[corSel]);
    tft.drawRoundRect(0,265,480,55,5, ColorTem[corSel]);
    tft.drawRoundRect(0,55,480,210,5, ColorTem[corSel]);

    if(toggleEcraSel)
    {
      tft.fillScreen(BLACK); 
      toggleEcraSel = false;
      pageChange();
    }

       
}
}

void drawDataInicial() {




  if(ecraSel == 1)
  {
  tft.setTextColor(WHITE,BLACK);
  tft.setTextSize(2);
  tft.setCursor(15,75);
  
  tft.print("RPMx1000");
    tft.setCursor(250,155);
  tft.print("RPM");
 rpmCorr = ((0.06714)*rpm);

   tft.setTextColor(WHITE);
  tft.setCursor(5, 215);
  tft.setTextSize(2);
  tft.print(0);
  tft.fillRoundRect(5, 235, 3, 15 ,5, WHITE);
 

tft.setTextColor(BLACK);
tft.setTextSize(3);
 tft.fillRoundRect(5,5, 95,45,5, DARKGREY);
 tft.setCursor(20,20); tft.print("OPT.");
    
  }
  if(ecraSel == 2)
  {
    tft.setTextColor(BLACK);
    tft.setTextSize(3);
    tft.fillRoundRect(5,270, 95,45,5, DARKGREY);
    tft.setCursor(15,283); tft.print("<---");

    
    tft.setTextSize(3);
    tft.setCursor(5,14);
    tft.setTextColor(WHITE,BLACK);
    tft.print("LUMINOSIDADE -");
    tft.fillRoundRect(270,5, 45,45,5, DARKGREY);
    tft.fillRoundRect(430,5, 45,45,5, DARKGREY);

    tft.setTextSize(4);
    tft.setCursor(282,12);
    tft.setTextColor(BLACK);
    tft.print("-");
    tft.setCursor(442,12);
    tft.setTextColor(BLACK);
    tft.print("+");


    
    tft.setTextSize(3);
    tft.setCursor(5,69);
    tft.setTextColor(WHITE,BLACK);
    tft.print("RPM MAXIMA -");
    tft.fillRoundRect(270,60, 45,45,5, DARKGREY);
    tft.fillRoundRect(430,60, 45,45,5, DARKGREY);

    tft.setTextSize(4);
    tft.setCursor(282,67);
    tft.setTextColor(BLACK);
    tft.print("-");
    tft.setCursor(442,67);
    tft.setTextColor(BLACK);
    tft.print("+");


    tft.setTextSize(3);
    tft.setCursor(5,126);
    tft.setTextColor(WHITE,BLACK);
    tft.print("SHIFT RPM -");
    tft.fillRoundRect(270,115, 45,45,5, DARKGREY);
    tft.fillRoundRect(430,115, 45,45,5, DARKGREY);

    tft.setTextSize(4);
    tft.setCursor(282,122);
    tft.setTextColor(BLACK);
    tft.print("-");
    tft.setCursor(442,122);
    tft.setTextColor(BLACK);
    tft.print("+");
    
    
    tft.setTextSize(4);
    tft.fillRoundRect(290,270, 185,45,5, DARKGREY);
    tft.setCursor(300,280);
    tft.setTextColor(BLACK);
    tft.print("APLICAR");


    tft.setCursor(165,180);
    tft.setTextSize(3);
    tft.setTextColor(WHITE);
    tft.print("COR TEMA:");
   
    tft.fillRoundRect(65,215, 45, 45,20, YELLOW);
    tft.fillRoundRect(115,215, 45, 45,20, DARKORANGE);
    tft.fillRoundRect(165,215, 45, 45,20, RED);
    tft.fillRoundRect(215,215, 45, 45,20, WHITE);
    tft.fillRoundRect(265,215, 45, 45,20, BLUE); 
    tft.fillRoundRect(315,215, 45, 45, 20, GREEN);
    tft.fillRoundRect(365,215, 45, 45,20, PINK);
    
    

  }


  
}




void processData() {  
 
  rpm = ((speedyResponse [15] << 8) | (speedyResponse [14])); 
  afr = speedyResponse[10];
  mapData = ((speedyResponse [5] << 8) | (speedyResponse [4]));
  psi = (mapData / 6.895);
  clt = speedyResponse[7];
  afrConv = afr/10;
  iat = speedyResponse[6];
  tps = speedyResponse[24];
  tps = tps/2;
  bat = speedyResponse[9];
  adv = speedyResponse[23];
  
}
