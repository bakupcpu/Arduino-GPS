/*********************************************************************
 * This is an example sketch for our Monochrome Nokia 5110 LCD Displays
 * 
 * Pick one up today in the adafruit shop!
 * ------> http://www.adafruit.com/products/338
 * 
 * These displays use SPI to communicate, 4 or 5 pins are required to
 * interface
 * 
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 * 
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 * BSD license, check license.txt for more information
 * All text above, and the splash screen must be included in any redistribution
 *********************************************************************/

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <TinyGPS.h>
#include <SoftwareSerial.h>
unsigned long fix_age;

SoftwareSerial GPS(5,4); //5 = RXD 4 = TXD front the device
TinyGPS gps;
void gpsdump(TinyGPS &gps);
bool feedgps();
void getGPS();
long lat, lon;
float LAT, LON, fkmph, falt, M1, M2, PLat, PLon, fc;
int BK, UP, DN, UP1, DN1, ENT, V1, H1, V2, H2, x, y, Sat;
int Menu = 0;
int MOD = 0;
int SEL = 0;
int LED = 235;
int GMT = 0;
int H = 0;
int CTR = 40;
int year;
byte month, day, hour, minute, second, hundredths;

// pin 9 - Serial clock out (SCLK)
// pin 10 - Serial data out (DIN)
// pin 11 - Data/Command select (D/C)
// pin 12 - LCD chip select (CS)
// pin 13 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(9, 10, 11, 12, 13);

void setup() {
  pinMode(8,INPUT); //BK
  pinMode(7,INPUT); //UP
  pinMode(A0, INPUT); //DN
  pinMode(A1, INPUT); //DN2
  pinMode(A3, INPUT); //UP2
  pinMode(2, INPUT); //ENT
  pinMode(6,OUTPUT);
  digitalWrite(8,HIGH);//Turn on the pullup
  digitalWrite(7,HIGH);//Turn on the pullup
  digitalWrite(A0,HIGH);//Turn on the pullup
  digitalWrite(A1,HIGH);//Turn on the pullup
  digitalWrite(A3,HIGH);//Turn on the pullup
  digitalWrite(2,HIGH);//Turn on the pullup

  GPS.begin(9600);
  display.begin();
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(CTR);
}

void loop() {
  BK = digitalRead(8);
  UP = digitalRead(7);
  DN = digitalRead(A0);
  UP1 = digitalRead(A1);
  DN1 = digitalRead(A3);
  ENT = digitalRead(2);
  analogWrite(6,LED);
  if(UP == LOW){
    delay(250);
    SEL--;
  }
  if(DN == LOW){
    delay(250);
    SEL++;
  }
  if(BK == LOW){
    SEL = 0;
    Menu = 0;
  }
  if(Menu == 0){ 
    Menu1();
  }
  if(Menu == 1){
    if(MOD == 0){
      ModeGPS();
    }
    if(MOD == 1){
      Speed();
    }
    if(MOD == 2){
      Mapping();
    }
    if(MOD == 3){
      Compass();
    }
    if(MOD == 4){
      Tracker();
    }
    if(MOD == 5){
      Settings();
    }
    if(MOD == 6){
      GOTO();
    }
  }
}


void getGPS(){
  bool newdata = false;
  unsigned long start = millis();
  // Every 1 seconds we print an update
  while (millis() - start < 1000)
  {
    if (feedgps ()){
      newdata = true;
    }

  }
  if (newdata)
  {
    gpsdump(gps);
  }


}

bool feedgps(){
  while (GPS.available())
  {
    if (gps.encode(GPS.read()))
      return true;
  }
  return 0;
}

void gpsdump(TinyGPS &gps)
{

  gps.crack_datetime(&year, &month, &day,
  &hour, &minute, &second, &hundredths, &fix_age);
  gps.get_position(&lat, &lon);
  Sat = gps.satellites();

  // returns +/- latitude/longitude in degrees
  falt = gps.f_altitude(); // +/- altitude in meters
  fc = gps.f_course(); // course in degrees
  //float fk = gps.f_speed_knots(); // speed in knots
  //float fmph = gps.f_speed_mph(); // speed in miles/hr
  //float fmps = gps.f_speed_mps(); // speed in m/sec
  fkmph = gps.f_speed_kmph(); // speed in km/hr
  LAT = lat;
  LON = lon;
  LAT = LAT/100000;
  LON = LON/100000;
  {
    feedgps(); // If we don't feed the gps during this long routine, we may drop characters and get checksum errors
  }
}

byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}
void Speed(){
  getGPS();
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(4);
  display.print(fkmph,0);
  display.setCursor(0,30);
  display.setTextSize(2);
  display.print("Km/h");
  display.display();
}
void ModeGPS(){
  getGPS();
  H = hour + GMT;
  if(H < 0){ 
    H = 24 + (hour + GMT);
  }
  display.clearDisplay();
  display.drawRect(80,0,4,40,BLACK);
  display.fillRect(80,0,4,40-(Sat*3),BLACK);
  display.print("Lat Alt:");
  display.print(falt,1);
  display.setCursor(0,8);
  display.println(LAT,7);
  display.println("Lon");
  display.println(LON,7);
  display.print(day);
  display.print("/");
  display.print(month);
  display.print("/");
  display.println(year);
  if(H > 11){
    display.print("PM ");
  }
  else{
    display.print("AM ");
  }
  if(H > 12){
    H = H - 12; 
  }
  if(H == 0){
    H = 12;
  }
  display.print(H);
  display.print(":");
  if(minute < 10){
    display.print("0");
  }
  display.print(minute);
  display.print(":");
  if(second < 10){
    display.print("0");
  }
  display.print(second);
  display.display();
}

void Menu1(){
  ENT = digitalRead(2);
  if(ENT == LOW){
    MOD = SEL;
    SEL = 0;
    Menu = 1;
  }
  if(SEL < 0){
    SEL = 5;
  }
  if(SEL > 5){
    SEL = 0;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,SEL*8);
  display.print(">");
  display.setCursor(6,0);
  display.print("GPS Mode");
  display.setCursor(6,8);
  display.print("Speed");
  display.setCursor(6,16);
  display.print("Mapping");
  display.setCursor(6,24);
  display.print("Compass");
  display.setCursor(6,32);
  display.print("Tracker");
  display.setCursor(6,40);
  display.print("Setting");
  display.display();
  delay(50);
}

void Settings(){
  UP1 = digitalRead(A1);
  DN1 = digitalRead(A3);
  if(UP1 == 0){
    delay(100);
    if(SEL == 0){ 
      CTR--; 
    }
    if(SEL == 1){ 
      LED = LED + 5; 
    }
    if(SEL == 2){ 
      GMT--; 
    }
  }

  if(DN1 == 0){
    delay(100);
    if(SEL == 0){ 
      CTR++; 
    }
    if(SEL == 1){ 
      LED = LED - 5; 
    }
    if(SEL == 2){ 
      GMT++; 
    }
  }

  if(CTR > 100){
    CTR = 100;
  }
  if(CTR < 0){
    CTR = 0;
  }
  if(LED < 0){
    LED = 0;
  }
  if(LED > 255){
    LED = 255;
  }
  if(GMT < -12){
    GMT = -12;
  }
  if(GMT > 12){
    GMT = 12;
  }
  if(SEL < 0){
    SEL = 2;
  }
  if(SEL > 2){
    SEL = 0;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,(SEL+1)*8);
  display.print(">");
  display.setCursor(0,0);
  display.print("Settings");
  display.setCursor(6,8);
  display.print("Contrast: ");
  display.print(CTR);
  display.setCursor(6,16);
  display.print("Light: ");
  display.print(255-LED);
  display.setCursor(6,24);
  display.print("GMT: ");
  display.print(GMT);
  display.display();
  display.setContrast(CTR);
  delay(50);
}

void Mapping(){
  UP1 = digitalRead(A1);
  DN1 = digitalRead(A3);
  ENT = digitalRead(2);
  if(ENT == LOW && SEL == 16){
    MOD = 6;
    M1 = 0;
    M2 = 0;
    getGPS();
    GOTO();
  }

  if(SEL < 0){
    SEL = 16;
  }
  if(SEL > 16){
    SEL = 0;
  }
  if( UP1 == 0){
    delay(100);
    if(SEL == 0){ 
      V1 = V1 - 1000;
    }
    if(SEL == 1){ 
      V1 = V1 - 100;
    }
    if(SEL == 2 ){ 
      V1 = V1 - 10;
    }
    if(SEL == 3){ 
      V1--;
    }
    if(SEL == 4){ 
      H1 = H1 - 1000;
    }

    if(SEL == 5){ 
      H1 = H1 - 100;
    }

    if(SEL == 6){ 
      H1 = H1 - 10;
    }
    if(SEL == 7){ 
      H1--;
    }

    if(SEL == 8){ 
      V2 = V2 - 1000;
    }

    if(SEL == 9){ 
      V2 = V2 - 100;
    }
    if(SEL == 10){ 
      V2 = V2 - 10;
    }

    if(SEL == 11){ 
      V2--;
    }

    if(SEL == 12){ 
      H2 = H2 - 1000;
    }

    if(SEL == 13){ 
      H2 = H2 - 100;
    }

    if(SEL == 14){ 
      H2 = H2 - 10;
    }

    if(SEL == 15){ 
      H2--;
    }
  }
  if( DN1 == 0){
    delay(100);
    if(SEL == 0){ 
      V1 = V1 + 1000;
    }
    if(SEL == 1){ 
      V1 = V1 + 100;
    }
    if(SEL == 2 ){ 
      V1 = V1 + 10;
    }
    if(SEL == 3){ 
      V1++;
    }
    if(SEL == 4){ 
      H1 = H1 + 1000;
    }

    if(SEL == 5){ 
      H1 = H1 + 100;
    }

    if(SEL == 6){ 
      H1 = H1 + 10;
    }
    if(SEL == 7){ 
      H1++;
    }

    if(SEL == 8){ 
      V2 = V2 + 1000;
    }

    if(SEL == 9){ 
      V2 = V2 + 100;
    }
    if(SEL == 10){ 
      V2 = V2 + 10;
    }

    if(SEL == 11){ 
      V2++;
    }

    if(SEL == 12){ 
      H2 = H2 + 1000;
    }

    if(SEL == 13){ 
      H2 = H2 + 100;
    }

    if(SEL == 14){ 
      H2 = H2 + 10;
    }

    if(SEL == 15){ 
      H2++;
    }
  }
  if(V1 > 9999){
    V1 = 0;
  }
  if(V1 < 0){
    V1 = 9999;
  }
  if(H1 > 9999){
    H1 = 0;
  }
  if(H1 < 0){
    H1 = 9999;
  }
  if(V2 > 9999){
    V2 = 0;
  }
  if(V2 < 0){
    V2 = 9999;
  }

  if(H2 > 9999){
    H2 = 0;
  }
  if(H2 < 0){
    H2 = 9999;
  }
  display.clearDisplay();
  display.setTextSize(1);
  if(SEL == 16){
    display.setCursor(0,40);
    display.print("Next");
  }
  if(SEL < 4){
    display.setCursor(SEL*6,16);
    display.print("^");
  }
  if(SEL > 3 && SEL < 8){
    display.setCursor((SEL+2)*6,16);
    display.print("^");
  }
  if(SEL > 7 && SEL < 12){
    display.setCursor((SEL-8)*6,40);
    display.print("^");
  }
  if(SEL > 11 && SEL < 16){
    display.setCursor((SEL-6)*6,40);
    display.print("^");
  }

  display.setCursor(0,0);
  display.print("Current Coor");
  display.setCursor(0,8);
  display.print(V1);
  display.print("  ");
  display.print(H1);
  display.setCursor(0,24);
  display.print("Target Coor");
  display.setCursor(0,32);
  display.print(V2);
  display.print("  ");
  display.print(H2);
  display.display();


}

void Compass(){
  getGPS();
  display.clearDisplay();
  display.setCursor(0,0);
  display.print((fc*(6400/360)),0);
  display.setCursor(66,0);
  display.print("Mil");
  fc = (fc/57.29577951);
  x = (40+(sin(fc)*20));
  y = (21+(cos(fc)*20));
  display.setCursor(x,y);
  display.print("S");

  x = (40+(sin(fc+1.75079633)*20));
  y = (21+(cos(fc+1.75079633)*20));
  display.setCursor(x,y);
  display.print("E");

  x = (40+(sin(fc+3.14159265)*20));
  y = (21+(cos(fc+3.14159265)*20));
  display.setCursor(x,y);
  display.print("N");

  x = (40+(sin(fc+4.71238898)*20));
  y = (21+(cos(fc+4.71238898)*20));
  display.setCursor(x,y);
  display.print("W");
  /*display.setCursor(40,0);
   display.print("N");
   display.setCursor(40,41);
   display.print("S");
   display.setCursor(20,21);
   display.print("W");
   display.setCursor(60,21);
   display.print("E");*/
  display.drawCircle(42,24,16,BLACK);
  display.drawLine(42,24,42,10,BLACK);
  display.drawLine(42,10,45,17,BLACK);
  display.drawLine(42,10,39,17,BLACK);
  display.display();
}

void GOTO(){
  //gps.get_position(&lat, &lon, &fix_age);
  getGPS();
  M1 = M1+((LAT-PLat)*111320);
  M2 = M2+(((LON*(cos(LAT)))-(PLon*(cos(PLat))))*111320);
  PLat = LAT;
  PLon = LON;

  display.clearDisplay();
  display.setTextSize(1);
  display.println("Target: ");
  display.print(V2);
  display.print(" ");
  display.println(H2);
  display.println("Current: ");
  display.print(V1+(M1/10),1);
  display.print(" ");
  display.println(H1+(M2/10),1);
  display.display();
}

void Tracker(){
  getGPS();
  ENT = digitalRead(2);
  if(ENT == LOW){
    PLat = LAT;
    PLon = LON;
  }
  M1 = gps.course_to(PLat,PLon,LAT,LON);
  M1 = (M1/57.29577951);
  fc = (fc/57.29577951);

  M2 = gps.distance_between(LAT,LON,PLat,PLon);
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Dist: ");
  if(M2 < 1000){display.print(M2,1); display.print(" M");}
  else{display.print(M2/1000,1); display.print(" Km");}
  x = (42+(sin(M1)*10));
  y = (26-(cos(M1)*10));
  display.drawLine(42,26,x,y,BLACK);
  x = (42+(sin(fc+3.14159265)*16));
  y = (26+(cos(fc+3.14159265)*16));
  display.drawLine(42,26,x,y,BLACK);
  display.drawCircle(42,26,17,BLACK);
  //display.drawCircle(67,33,14,BLACK);
  display.display();

}











