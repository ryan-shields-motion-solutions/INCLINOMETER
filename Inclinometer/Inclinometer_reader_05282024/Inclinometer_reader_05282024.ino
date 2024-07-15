#include <Arduino_BuiltIn.h>

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  //register the LCD at address 0x27 as a 16x2 LCD display
char serialdatax[12];
char serialdatay[12];
int serialdatabufferlen = 12;
long inclinometerdefzerox = 32768;
long inclinometerdefcountsperdegx = 976;
long inclinometerzerox = inclinometerdefzerox;
long inclinometercountsperdegx = inclinometerdefcountsperdegx;
long inclinometerdefzeroy = 32768;
long inclinometerdefcountsperdegy = 976;
long inclinometerzeroy = inclinometerdefzeroy;
long inclinometercountsperdegy = inclinometerdefcountsperdegy;
long xmax = 25;
long xmin = -25;
long ymax = 25;
long ymin = -25;
unsigned long currenttime=0;
unsigned long previoustime=0;
unsigned long screenprinttime=500;
long convertxdata(long rawdatax=999999);
long convertydata(long rawdatay =999999);
void lcdprintx(long sensorxdata = 999999);
void lcdprinty(long sensorydata = 999999);
void setup() {

  lcd.init();         //initilaize the lcd
  lcd.backlight();    //turn on the lcd backlight
  lcd.clear();        //clear the display
  delay(1000);        //wait for 1 second
  lcd.noBacklight();  //turn off the lcd backlight
  delay(1000);        //wait for 1 second
  lcd.backlight();    //turn on the lcd backlight
delay(10000);
  //Setup the Arduino to Computer serial port to be at 9600 baudrate
  Serial.begin(9600);

  //Setup the Arduino to Inclinometer to be at 9600 baudrate
  Serial1.begin(9600);
}

void loop() {
  long rawx = 0;
  long rawy = 0;
  long finx = 0;
  long finy = 0;
  delay(0.5);
 // Serial.println("Getting X Data...");
  rawx = getxdata();
  //Serial.print("Raw X Data long is:");
  //Serial.println(rawx);
  finx = convertxdata(rawx);
  //Serial.print("Post Conversion X data is: ");
  //Serial.println(finx);

 
   // Serial.println("Getting Y Data...");
  rawy = getydata();
  finy = convertydata(rawy);
  //Serial.println(finy);
  currenttime=millis();
  if ((currenttime-previoustime)>screenprinttime){
   lcdprintx(finx);
lcdprinty(finy);
previoustime=currenttime;
  }



  //lcd.setCursor(1, 0);         //set the cursor to column 1, line 0
  // lcd.print("Yaw:    deg");    //print the yaw value from the inclinometer
  //lcd.setCursor(1, 1);         //set the cursor to column 2, line 0
  //lcd.print("Pitch:    deg");  //print the pitch value from the inclinometer

  // delay(2000);
  //lcd.setCursor(15, 1);
  // lcd.cursor_on();
  //lcd.blink_on();
  // delay(2500);
  // lcd.blink_off();
  // lcd.cursor_off();
}


long getxdata() {
  int i = 0;

  int data_available = 0;
  char incomingByte = 0;
  long rawdata = 0;

  //clear the x axis serial data
  Serial1.flush();
  for (i = 0; i > serialdatabufferlen; i = i + 1) {
    serialdatax[i] = 0;
  }

  while (Serial1.available() <= 0) {
    delay(1);
    Serial1.print("x");
    Serial1.flush();
    //Serial.print("requesting x data");
    //Serial.println();
    delay(1);
  }

  i = -1;
  while (Serial1.available() > 0) {
    delay(1);
    data_available = Serial1.available();
    i = i + 1;
    incomingByte = Serial1.read();
    //Serial.print(incomingByte,HEX);
    //Serial.println();
    serialdatax[i] = incomingByte;
    //Serial.print(serialdatax[i],DEC);
  }
  //delay(15);
  //Serial.println();
  //Serial.println(serialdatax);
  rawdata = atol(serialdatax);
  //Serial.print(rawdata);
  //Serial.println();
  return { rawdata };
}

long getydata() {
  int i = 0;

  int data_available = 0;
  char incomingByte = 0;
  long rawdata = 0;

  //clear the x axis serial data
  Serial1.flush();
  for (i = 0; i > serialdatabufferlen; i = i + 1) {
    serialdatay[i] = 0;
  }

  while (Serial1.available() <= 0) {
    delay(1);
    Serial1.print("y");
    Serial1.flush();
    delay(1);
  }

  i = -1;
  while (Serial1.available() > 0) {
    delay(1);
    data_available = Serial1.available();
    i = i + 1;
    incomingByte = Serial1.read();
        //Serial.print(incomingByte,HEX);
    //Serial.println();
    serialdatay[i] = incomingByte;
        //Serial.print(serialdatay[i],DEC);
        //Serial.println();
  }
  //Serial.println(serialdatay);
  rawdata = atol(serialdatay);
  //Serial.print(rawdata);
  return { rawdata };
}

long convertxdata(long rawdatax){
  float unroundedvalue = 0;
  long roundedvalue = 0;
  //Serial.print("Value to convert is: ");
  //Serial.println(rawdatax);
  //chech if a proper value has been input. If not return an error.
  if (rawdatax < 0 || rawdatax > 65536) {
    return { 99999999};
  };
  unroundedvalue = float(float(rawdatax - inclinometerzerox) / inclinometercountsperdegx);
  roundedvalue = long(unroundedvalue * 100);
  return { roundedvalue };
}

long convertydata(long rawdatay) {
  float unroundedvalue = 0;
  long roundedvalue = 0;
  //chech if a proper value has been input. If not return an error.
  if (rawdatay < 0 || rawdatay > 65536) {
    return { 99999999 };
  };
  unroundedvalue = float(float((rawdatay - inclinometerzeroy)) / inclinometercountsperdegy);
  roundedvalue = long(unroundedvalue * 100);
  return { roundedvalue };
}


void lcdprintx(long sensorxdata) {
  char buffer[4];
  int bufferlength;
  char bufferdec[3];
  int bufferdeclength;
  String desiredtext;
  //Serial.println("X Sensor Data to LCD:");
  //Serial.println(sensorxdata);
  if (sensorxdata < (xmin * 100)) {
    lcd.setCursor(0, 0);
    lcd.print("X:-LIM       ");
    return;
  }
  if (sensorxdata > (xmax * 100)) {
    lcd.setCursor(0, 0);
    lcd.print("X:+LIM        ");
    return;
  }
  lcd.setCursor(0, 0);  //set the cursor to column 1, line 0
if((sensorxdata/100)<0){
desiredtext=String("X:-");
}
else{
desiredtext=String("X: ");
}
if(abs(sensorxdata/100)<10){
desiredtext+=String(" ");
}
  bufferlength=snprintf(buffer, 4, "%d", (abs(sensorxdata) / 100));
  //Serial.print("Non decimal length is: ");
  //Serial.println(bufferlength);
  bufferdeclength=snprintf(bufferdec, 3, "%d", abs(sensorxdata % 100));
  
  //Serial.print("Decimal length is: ");
  //Serial.println(bufferdeclength);
  //desiredtext = String("X:");
  desiredtext += String(buffer);
  desiredtext += String(".");
  desiredtext += String(bufferdec);
  if(bufferdeclength=2){
    desiredtext += String(" deg");
  }
  else{
  desiredtext += String("deg");
  }

  Serial.print(desiredtext);
  Serial.println();
  lcd.print(desiredtext);  //print the yaw value from the inclinometer
  return;
}

void lcdprinty(long sensorydata) {
   char buffer[4];
  int bufferlength;
  char bufferdec[3];
  int bufferdeclength;
  String desiredtext;
  //Serial.println("X Sensor Data to LCD:");
  //Serial.println(sensorxdata);
  if (sensorydata < (ymin * 100)) {
    lcd.setCursor(0, 1);
    lcd.print("Y:-LIM       ");
    return;
  }
  if (sensorydata > (ymax * 100)) {
    lcd.setCursor(0, 1);
    lcd.print("Y:+LIM        ");
    return;
  }
  lcd.setCursor(0, 1);  //set the cursor to column 1, line 0
if((sensorydata/100)<0){
desiredtext=String("Y:-");
}
else{
desiredtext=String("Y: ");
}
if(abs(sensorydata/100)<10){
desiredtext+=String(" ");
}
  bufferlength=snprintf(buffer, 4, "%d", (abs(sensorydata) / 100));
  //Serial.print("Non decimal length is: ");
  //Serial.println(bufferlength);
  bufferdeclength=snprintf(bufferdec, 3, "%d", abs(sensorydata % 100));
  
  //Serial.print("Decimal length is: ");
  //Serial.println(bufferdeclength);
  //desiredtext = String("X:");
  desiredtext += String(buffer);
  desiredtext += String(".");
  desiredtext += String(bufferdec);
  if(bufferdeclength=2){
    desiredtext += String(" deg");
  }
  else{
  desiredtext += String("deg");
  }

  Serial.print(desiredtext);
  Serial.println();
  lcd.print(desiredtext);  //print the yaw value from the inclinometer
  return;
}
