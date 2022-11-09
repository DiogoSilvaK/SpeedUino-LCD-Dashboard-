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
#define BLACK   0x00
#define GRAY    0x8410
#define WHITE   0xFFFF
#define RED     0xF800
#define ORANGE  0xFA60
#define YELLOW  0xFFE0 
#define LIME    0x07FF
#define GREEN   0x07E0
#define CYAN    0x07FF
#define AQUA    0x04FF
#define BLUE    0x045D
#define MAGENTA 0xF81F
#define PINK    0xF8FF

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
  bool toggletemp = true;
  bool togglea5 = false;
  bool toggleUpText = false;
  bool toggleEcuMap = false;
  bool togglebat = true;
  bool toggleign = true;
  //false -> Eco
  //true -> Power
int rpmMax= 6600;



unsigned int rpm;  //rpm and PW from speeduino
float afr;
float mapData;
int8_t psi;
float afrConv;
uint8_t cmdAdata[50] ; 

void setup () 
{  display.reset();
//ID = display.readID();display.begin(ID);
  Serial.begin(115200);
  while (!Serial);
  SPI.begin();
  Waveshield.begin();
  
 
  tft.setRotation(Orientation);
  /*delay(500);
  tft.fillRect(42,28,376,118,BLACK);
  tft.setTextColor(WHITE);
  tft.setCursor(44,30);
  tft.setTextSize(16);
  tft.print("OPEL");
  tft.setTextColor(BLACK);
  tft.setCursor(120,220);
  tft.setTextSize(4);
  tft.println("POWERED BY");
   tft.setCursor(40,265);
  tft.println("BIELAS MOTORSPORT");
*/


  delay(2000);
  tft.fillScreen(BLACK);
  pinMode(2, OUTPUT);
  /*
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  */
  pinMode(A5,INPUT);
  //pinMode(A6,INPUT);
    tft.setTextSize(4);


 /* tft.fillRect(0,0, 480,30,WHITE);
  tft.setTextColor(BLACK);
  tft.setCursor(40,1);
  tft.print("BIELAS MOTORSPORT");
  */


  }
    

uint32_t old_ts;

#define BYTES_TO_READ 74
#define SERIAL_TIMEOUT 300
float rps;
boolean sent = false;
boolean received = false;
uint32_t sendTimestamp;

void loop () {
  drawData();
  requestData();
  if(received) {//displayData();
    drawData();
    received = false;}
    // valord2 = digitalRead(2);
     valora5= analogRead(A5);
    valorvoltagea5 = valora5 * (5.0/1023.0);
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


 
void drawData() { 

    TSPoint tp = Waveshield.getPoint() ;

    Waveshield.normalizeTsPoint(tp);

   // tft.setCursor(40,1);
 // tft.print(valorvoltagea5);
  
  //tft.setRotation(3);
  //tft.fillScreen(BLACK);
tft.setTextSize(3);
      //mapa secundário

    if((tp.x >= 243 && tp.x <= 475 )&&(tp.y >= 265&&tp.y<= 315))
    {
      toggleEcuMap = true;
      toggle = false;
    }
    else if((tp.x >= 5 && tp.x <= 238)&&(tp.y >= 265&&tp.y<= 315))
    {
      toggleEcuMap = false;
      toggle = false;
    }


      
     if (toggleEcuMap == true && toggle == false)
     {
      toggle = true;
      tft.fillRect(242,270,233,45,RED);
      tft.setCursor(320,281);
      tft.setTextColor(BLACK);
      tft.print("POWER");
      tft.fillRect(5,270,233,45, GRAY);
      tft.setCursor(100,281);
      tft.print("ECO");
      digitalWrite(2, HIGH);
      }
     if(toggleEcuMap == false && toggle == false){
      toggle = true; 
      tft.fillRect(5,270,233,45, GREEN);
      tft.setCursor(100,281);
      tft.setTextColor(BLACK);
      tft.print("ECO");
      tft.fillRect(242,270,233,45,GRAY);
      tft.setCursor(320,281);
      tft.setTextColor(BLACK);
      tft.print("POWER");
      digitalWrite(2, LOW);
      }






      

  //Rotação

 

        //if ( 6300 <= rpm ) {  for(int i = 0; i < 6;i++){tft.fillScreen(RED);tft.setTextColor(BLACK);tft.setTextSize(12);tft.setCursor(30,115);tft.print("SHIFT!");}tft.fillScreen(BLACK); toggleUpText = true;}
        
        tft.setTextColor(WHITE,BLACK);
       tft.setTextSize(3);
  tft.setCursor(5,40);
  //tft.print("RPM-");
  tft.setTextSize(3);
 int rpmCorr = ((0.06714)*rpm);
/*
  if (rpm < 3500) { tft.setTextColor(GREEN,BLACK);tft.fillRect(5,140, rpmCorr ,45, GREEN);}
  if ( rpm >= 3500 && rpm < 5300){tft.setTextColor(YELLOW,BLACK);tft.fillRect(5,140, rpmCorr ,45, YELLOW);}
  if ( rpm >= 5300){tft.setTextColor(RED,BLACK);tft.fillRect(5 ,140, rpmCorr ,45, RED);}

tft.fillRect(5+ rpmCorr ,140, 470 - rpmCorr,45, BLACK);
  
 */
int d = 0;

/*
  if(rpm < 5800)
  {
    rpm = rpm + 500;
  }
  else
  {
    rpm = 0;
  }
*/
  int ColorReg[] = {GREEN, GREEN, GREEN, YELLOW, YELLOW,RED, RED};

  int ColorDot = GREEN;




  
  if (rpm < 3500) { tft.setTextColor(GREEN,BLACK);tft.drawRect(4 ,139, 472 ,47, GREEN);
    for(int i = 0; i < rpm/(74.5); i++)
  {
     d = d +6;
    tft.fillRect( 5+d ,140, 2,45, GREEN);
    }

  }
  if ( rpm >= 3500 && rpm < 5300){tft.setTextColor(YELLOW,BLACK);tft.drawRect(4 ,139, 472 ,47, YELLOW);
    for(int i = 0; i < rpm/(74.5); i++)
  {
     d = d +6;
    tft.fillRect( 5+ d ,140, 2,45, YELLOW);
    }

  }
  if ( rpm >= 5300){tft.setTextColor(RED,BLACK);tft.drawRect(4 ,139, 472 ,47, RED);
    for(int i = 0; i < rpm/250; i++)
  {
    
    if(i== 0){d = 5;}
    else{d = d +17;}

    if(i < 12)
    {
      ColorDot = GREEN;
    }
    else if(i >= 12 && i <20)
    {
      ColorDot = YELLOW;
    }
    else if(i >= 20 && i <28)
    {
      ColorDot = RED; 
    }
     
    tft.fillRect( d ,140, 13.7857143,45, ColorDot);
    }

  }

  tft.fillRect(10+ rpmCorr ,140, 465 - rpmCorr,45, BLACK);
  




  

  
  if(toggleBackrpm && rpm <= 1000){tft.fillRect(15,55, 210,65,BLACK);toggleBackrpm = false;}
  if(!toggleBackrpm && rpm> 1000){toggleBackrpm = true;}
  rpm= 7000;
  tft.setTextSize(9);
  tft.setCursor(15,55);
  
  tft.print(rpm);

   tft.setTextColor(GREEN);
  tft.setCursor(15, 190);
  tft.setTextSize(2);
  tft.print(0);
  tft.fillRect(5, 190, 2, 15 , GREEN);


int c = 0;
for(int i = 0; i<7; i++)
{
  c = c + (67.14);
  tft.fillRect(5+c, 190, 2, 15, ColorReg[i]);
  //tft.fillRect(5+c, 140, 2, 45 , BLACK);
  tft.setTextColor(ColorReg[i]);
  tft.setCursor((c - 15), 190);
  tft.print(i+1);
  
}
/*
  tft.setTextColor(GREEN);
  tft.setCursor(43, 190);
  tft.print(10);
  tft.setTextColor(GREEN);
  tft.setCursor(114, 190);
  tft.print(20);
  tft.setTextColor(GREEN);
  tft.setCursor(186, 190);
  tft.print(30);
  tft.setTextColor(YELLOW);
  tft.setCursor(257, 190);
  tft.print(40);
  tft.setTextColor(YELLOW);
  tft.setCursor(328, 190);
  tft.print(50);
  tft.setTextColor(RED);
  tft.setCursor(399, 190);
  tft.print(60);
*/
 /* 
  tft.setTextColor(RED);
  tft.setCursor(442, 190);
  tft.print(66);
  //tft.fillRect(76, 140, 2, 45 , BLACK);
  //tft.fillRect(470, 190, 2, 15 , RED);
*/

  
    //  Now that we have a point in screen co-ordinates, draw something there.
   // Waveshield.fillCircle(tp.x, tp.y, 3, BLUE);

  
   //Posição da borboleta
  tft.setTextSize(3);
  tft.setCursor(250,50);
  tft.print("TPS-");
  tft.setTextSize(3);
  tft.setCursor(320,50);
  tft.print(tps);  tft.print("%");
 
  tft.fillRect(250 + (tps*2.25), 80, 225 - (tps*2.25), 50, BLACK);
  tft.fillRect(250 , 80 , tps *(2.25), 50,RED);
  
   
   
 
  
  
  //Mistura
  tft.setTextSize(2);            // Draw 2X-scale text
  tft.setTextColor(ORANGE,BLACK);
  tft.setCursor(5,220);
  tft.print("AFR-");
  tft.setTextSize(4);
  tft.setCursor(80,225);
  tft.print(afrConv, 1);

  //Turbo
  
  tft.setTextSize(3);
  tft.setTextColor(BLUE,BLACK);
  tft.setCursor(245,220);
  tft.print("MAP-");
  tft.setTextSize(4);
  tft.setCursor(330,225);
  tft.print(psi*0.0689);

  //Temperatura do Motor
 //clt=1140;
 clt = (clt-40)/10;
 if( clt < 100 && toggletemp == true){toggletemp = false; tft.fillRect(200,5,105,30,BLACK);}
if( clt > 100 && toggletemp == false){toggletemp = true;}
  tft.setTextColor(RED,BLACK);
  tft.setTextSize(3);
  tft.setCursor(130,5);
  tft.print("TMP-");
  tft.setTextSize(3);
  tft.setCursor(200,5);
  tft.setTextColor(RED);tft.print(clt); 
  //tft.print((clt-40)/10);       // offset -40  C readout ( removed -40to get F readout)

  
//Temperatura da Admissão
//iat = 540;
iat = (iat-40)/10;
if( iat < 10 && toggletemp == true){toggletemp = false; tft.fillRect(317,5,105,30,BLACK);}
if( iat > 10 && toggletemp == false){toggletemp = true;}
   tft.setTextColor(YELLOW,BLACK);
  tft.setTextSize(3);
  tft.setCursor(260,5);
  tft.print("IT-");
  tft.setTextSize(3);
  tft.setCursor(317,5);
  tft.setTextColor(YELLOW);tft.print(iat);
    // offset -40  C readout ( removed -40to get F readout)



/*
//Voltagem da Bateria
if( bat < 10 && togglebat == true){togglebat = false; tft.fillRect(85,5,55,30,BLACK);}
if( bat >= 10 && togglebat == false){togglebat = true;}
  tft.setTextSize(3);
  tft.setTextColor(WHITE,BLACK);
  tft.setCursor(5,5);
  tft.print("BAT-");
  tft.setTextSize(3);
  tft.setCursor(85,5);
  tft.print(bat/10);
*/
/*
  tft.setTextSize(2);
  tft.setTextColor(GREEN,BLACK);
  tft.setCursor(300,90);
  tft.print("FLEX %-");
  tft.setTextSize(3);
  tft.setCursor(390,85);
  tft.print(flex);
*/                         
//Avanço da ignição
if( adv < 10 && toggleign == true){toggleign = false; tft.fillRect(85,5,55,30,BLACK);}
if( adv >= 10 && toggleign == false){toggleign = true;}
  tft.setTextSize(3);
  tft.setTextColor(GREEN,BLACK);
  tft.setCursor(5,5);
  tft.print("IGN-"); 
  tft.setTextSize(3);
  tft.setCursor(85,5);
  tft.print(adv);
  
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
      tft.drawRect(0,0, 480,30,WHITE);//Right
  tft.drawRect(0,30, 240,105,WHITE);//RPM
  tft.drawRect(240,30, 240,105,WHITE);//RPM
  tft.drawRect(0,135,480,75,WHITE);//RPM OUTLINE
  tft.drawRect(0,265,480,55,WHITE);//BOTTOM
  tft.drawRect(0,210,240,55,WHITE);
    tft.drawRect(240,210,240,55,WHITE);
  
  
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
  bat = speedyResponse[9];
  adv = speedyResponse[23];
  
}
