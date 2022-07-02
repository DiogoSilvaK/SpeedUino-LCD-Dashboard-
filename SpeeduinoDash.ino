#include <Arduino.h>
#include <SPI.h>
//#include <Waveshare_ILI9486_GFX.h>
/*
 * Speeduino Display Dashboard
 *          BY
 *      DIOGO-SILVA
 *      PEDRO-SILVA
 */
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv display;       // hard-wired for UNO / MEGA shields anyway.
#include  <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h> // Hardware-specific library
#include <Waveshare4InchTftShield.h>
#include <Fonts/FreeMonoBold18pt7b.h>

namespace{Waveshare4InchTftShield Waveshield;Adafruit_GFX &tft = Waveshield;}

const int XP=6,XM=A2,YP=A1,YM=7; //ID=0x9341
const int TS_LEFT = 907, TS_RT = 136, TS_TOP = 942, TS_BOT = 139;
TSPoint tp;

int16_t BOXSIZE;
int16_t PENRADIUS = 1;
uint16_t ID, oldcolor, currentcolor;
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
#define BLUE    0x001F
#define MAGENTA 0xF81F
#define PINK    0xF8FF

static uint32_t oldtime = millis();
uint8_t speedyResponse[100]; //The data buffer for the Serial data. This is longer than needed, just in case
uint8_t byteNumber[2];  // pointer to which uint8_t number we are reading currently
uint8_t readiat; // iat doesn't need to be updated very ofter so
int iat;   // to store coolant temp
uint8_t readclt; // clt doesn't need to be updated very ofter so
int clt;   // to store coolant temp
//int tps;
int bat;
int adv;
 int valord2 =0;
 int valora5 = 0;
 int valorvoltagea5;
  bool toggle = false;
  bool toggleBackrpm = true;
  bool toggleinforight = true;
  bool togglea5 = false;
  
unsigned int rpm;  //rpm and PW from speeduino
float afr;
float mapData;
int8_t psi;
float afrConv;
uint8_t cmdAdata[50] ; 

void setup () 
{  display.reset();ID = display.readID();display.begin(ID);tft.setRotation(4);
  Serial.begin(115200);
  while (!Serial);
  SPI.begin();
  Waveshield.begin();
  
  tft.fillScreen(WHITE);
  tft.setRotation(3);
  delay(500);
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

  delay(2000);
  tft.fillScreen(BLACK);
  pinMode(2, INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  pinMode(A5,INPUT);
  pinMode(A6,INPUT);
    tft.setTextSize(4);
  
  tft.fillRect(0,0, 480,30,WHITE);
  tft.setTextColor(BLACK);
  tft.setCursor(40,1);
  tft.print("BIELAS MOTORSPORT");
  //rpm= 2000;

 
  }
    

uint32_t old_ts;

#define BYTES_TO_READ 105
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
     valord2 = digitalRead(2);
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
   // tft.setCursor(40,1);
 // tft.print(valorvoltagea5);
  
  tft.setRotation(3);
  //tft.fillScreen(BLACK);
tft.setTextSize(3);
      //mapa secundário
     if ((valord2== 1) && (toggle == true)){toggle = false; tft.fillRect(180,285,146,30,RED); tft.setCursor(210,291);tft.setTextColor(BLACK); tft.print("POWER"); tft.fillRect(330,285,146,30, GRAY);tft.setCursor(375,291);tft.print("ECO");}
     if((valord2==0) && (toggle == false)){toggle = true; tft.fillRect(330,285,146,30, GREEN);tft.setCursor(375,291);tft.setTextColor(BLACK); tft.print("ECO"); tft.fillRect(180,285,146,30,GRAY); tft.setCursor(210,291);tft.setTextColor(BLACK); tft.print("POWER");}
     if(valorvoltagea5 > 4 && togglea5 == true ){togglea5 =false;tft.fillRect(6,285,170,30,GREEN);}
     else if(valorvoltagea5 <= 4 && togglea5 == false){tft.fillRect(6,285,170,30,GRAY); tft.setTextColor(BLACK); tft.setCursor(10,291);tft.print("L.CONTROL");togglea5 = true;}
  
  //Rotação

        if ( 6300 <= rpm ) {for(int i = 0; i < 6;i++){tft.fillScreen(RED);tft.setTextColor(BLACK);tft.setTextSize(12);tft.setCursor(30,115);tft.print("SHIFT!");}tft.fillScreen(BLACK);}
        tft.setTextColor(WHITE,BLACK);
       tft.setTextSize(3);
  tft.setCursor(5,40);
  tft.print("RPM-");
  tft.setTextSize(3);
  if (rpm < 3500) { tft.setTextColor(GREEN,BLACK);tft.fillRect(5,160, (rpm*0.047),45, GREEN);}
  if ( rpm >= 3500 && rpm < 5300){tft.setTextColor(YELLOW,BLACK);tft.fillRect(5,160, (rpm*0.047),45, YELLOW);}
  if ( rpm >= 5300){tft.setTextColor(RED,BLACK);tft.fillRect(5,160, (rpm*0.047),45, RED);}

  if(toggleBackrpm && rpm <= 1000){tft.fillRect(100,90, 200,65,BLACK);toggleBackrpm = false;}
  if(!toggleBackrpm && rpm> 1000){toggleBackrpm = true;}
  
  tft.setTextSize(8);
  tft.setCursor(100,90);
  tft.print(rpm);



  /*
  tft.setTextSize(1);
  tft.setTextColor(ORANGE,BLACK);
  tft.setCursor(440,310);
  tft.print("TPS");
  tft.fillRect(430, 260, 40, 40, BLACK);
  tft.fillRect(430, 260 + (40 - tps), 40, tps,RED);
  */
  
  
  
  
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
 if( clt < 100 && toggleinforight == true){toggleinforight = false; tft.fillRect(425,45,55,120,BLACK);}
if( clt >= 100 && toggleinforight == false){toggleinforight = true;}
  tft.setTextColor(WHITE,BLACK);
  tft.setTextSize(3);
  tft.setCursor(325,125);
  tft.print("TEMP-");
  tft.setTextSize(3);
  tft.setCursor(420,125);
  if((clt-40)/10 <=45){tft.setTextColor(GREEN);tft.print((clt-40)/10); }
  if((clt-40)/10 >45 && (clt-40)/10 <=75){tft.setTextColor(LIME,BLACK);tft.print((clt-40)/10);}
  if((clt-40)/10 >75 && (clt-40)/10 <=95){tft.setTextColor(YELLOW,BLACK);tft.print((clt-40)/10);}
  if((clt-40)/10 >95 && (clt-40)/10 <=110){tft.setTextColor(ORANGE,BLACK);tft.print((clt-40)/10);}
  if((clt-40)/10 >110 && (clt-40)/10 <=130){tft.setTextColor(RED,BLACK);tft.print((clt-40)/10);}
  //tft.print((clt-40)/10);       // offset -40  C readout ( removed -40to get F readout)

  
//Temperatura da Admissão
if( iat < 10 && toggleinforight == true){toggleinforight = false; tft.fillRect(425,45,55,120,BLACK);}
if( iat >= 10 && toggleinforight == false){toggleinforight = true;}
   tft.setTextColor(WHITE,BLACK);
  tft.setTextSize(3);
  tft.setCursor(325,165);
  tft.print("IAT-");
  tft.setTextSize(3);
  tft.setCursor(415,165);
  if((iat-40)/10 <=45){tft.setTextColor(GREEN);tft.print((iat-40)/10); }
  if((iat-40)/10 >=45 && (iat-40)/10 <=75){tft.setTextColor(LIME,BLACK);tft.print((iat-40)/10);}
  if((iat-40)/10 >=75 && (iat-40)/10 <=95){tft.setTextColor(YELLOW,BLACK);tft.print((iat-40)/10);}
  if((iat-40)/10 >=95 && (iat-40)/10 <=110){tft.setTextColor(ORANGE,BLACK);tft.print((iat-40)/10);}
  if((iat-40)/10 >=110 && (iat-40)/10 <=130){tft.setTextColor(RED,BLACK);tft.print((iat-40)/10);}        // offset -40  C readout ( removed -40to get F readout)
/*
  //Posição da borboleta
  tft.setTextSize(2);
  tft.setCursor(160,0);
  tft.print("TPS-");
  tft.setTextSize(2);
  tft.setCursor(200,0);
  tft.print(tps);    
*/

//Voltagem da Bateria
if( bat < 10 && toggleinforight == true){toggleinforight = false; tft.fillRect(425,45,55,120,BLACK);}
if( bat >= 10 && toggleinforight == false){toggleinforight = true;}
  tft.setTextSize(3);
  tft.setTextColor(RED,BLACK);
  tft.setCursor(325,45);
  tft.print("BATT-");
  tft.setTextSize(3);
  tft.setCursor(425,45);
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
//Avanço da ignição
if( adv < 10 && toggleinforight == true){toggleinforight = false; tft.fillRect(425,45,55,120,BLACK);}
if( adv >= 10 && toggleinforight == false){toggleinforight = true;}
  tft.setTextSize(3);
  tft.setTextColor(GREEN,BLACK);
  tft.setCursor(325,85);
  tft.print("IGN-"); 
  tft.setTextSize(3);
  tft.setCursor(415,85);
  tft.print(adv);
  
  delay(10);  


//Do the outlines
  tft.drawRect(320,31,160, 180,RED);//Right
  tft.drawRect(0,155,320,55,RED);//RPM OUTLINE
  tft.drawRect(0,280,480,40,RED);//BOTTOM
  tft.drawRect(0,210,240,70,RED);
    tft.drawRect(240,210,240,70,RED);
  
  
}
void processData() {  
 
  rpm = ((speedyResponse [15] << 8) | (speedyResponse [14])); 
  afr = speedyResponse[10];
  mapData = ((speedyResponse [5] << 8) | (speedyResponse [4]));
  psi = (mapData / 6.895);
  clt = speedyResponse[7];
  afrConv = afr/10;
  iat = speedyResponse[6];
  //tps = speedyResponse[24];
  bat = speedyResponse[9];
  adv = speedyResponse[23];
  
}
