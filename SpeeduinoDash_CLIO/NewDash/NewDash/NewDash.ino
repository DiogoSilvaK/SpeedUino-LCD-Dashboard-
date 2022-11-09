#include <Arduino.h>
#include <SPI.h>
//#include <Waveshare_ILI9486_GFX.h>
#include <MCUFRIEND_kbv.h>
#include  <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h> 
#include <Waveshare4InchTftShield.h>
#include <EEPROM.h>

MCUFRIEND_kbv display; 
namespace{Waveshare4InchTftShield Waveshield;Adafruit_GFX &tft = Waveshield;}

uint8_t Orientation = 2;    //PORTRAIT
 
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









void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
