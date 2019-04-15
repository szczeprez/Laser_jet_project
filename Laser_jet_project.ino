#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8
#define laser 12
#define button_time 2
#define button_select_time 3
#include <SoftwareSerial.h>
SoftwareSerial bluetoothSerial(10, 11); // RX, TX // DO USTAWIENIA PINY DLA BLUETOOTH

int START = 0;
int red = 0;
int green = 0;
int blue = 0;
int SENSOR_DELAY = 100;
int counterBreak = 0, state = 0;
uint32_t startCounterTime;
double finishCounterTime = 0.0;
unsigned long aktualnyCzas = 0;
unsigned long zapamietanyCzas = 0;
unsigned long roznicaCzasu = 0;
unsigned long aktualnyCzasSENSOR = 0;
unsigned long zapamietanyCzasSENSOR_Red = 0;
unsigned long zapamietanyCzasSENSOR_Green = 0;
unsigned long zapamietanyCzasSENSOR_Blue = 0;
double listOfResults[3] = {0.0};
int index = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  bluetoothSerial.begin(9600);
  bluetoothSerial.println("Witam w aplikacji fotokomórki. Będziesz miał możliwość mieżenia czasu i listowania członków wyścigu. Udanej zabawy! :)");

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  pinMode(button_time, INPUT_PULLUP);
  pinMode(button_select_time, INPUT_PULLUP);

  pinMode(laser, OUTPUT);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  //get average of 5 sec callibarion
  // and then start clock

}
void loop() {

  aktualnyCzasSENSOR = millis();
  if (aktualnyCzasSENSOR - zapamietanyCzasSENSOR_Red >= 300UL) {
    zapamietanyCzasSENSOR_Red = aktualnyCzasSENSOR;
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    red = pulseIn(sensorOut, LOW);
    //  Serial.println("Red");

  }
  delay(100);

  if (aktualnyCzasSENSOR - zapamietanyCzasSENSOR_Green >= 300UL) {
    zapamietanyCzasSENSOR_Green = aktualnyCzasSENSOR;
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    green = pulseIn(sensorOut, LOW);
    //Serial.println("Green");
  }
  delay(100);

  if (aktualnyCzasSENSOR - zapamietanyCzasSENSOR_Blue >= 300UL) {
    zapamietanyCzasSENSOR_Blue = aktualnyCzasSENSOR;
    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    blue = pulseIn(sensorOut, LOW);
    //   Serial.println("Blue");
  }
  delay(100);
  breakLaser();
}

void breakLaser() {
  switch (state) {
    case 0:

     // blueToothWrite("Choose S to Start or L to display the results.");
      bluetoothSerial.println("Choose S to Start or L to display the results.");

      if (blueToothRead() == "S" || digitalRead(button_time) == LOW) {
        state = 1;
      } else if (blueToothRead() == "L" || digitalRead( button_select_time) == LOW ) {
        state = 4;
      }
      break;

    case 1:
      if (countLaserBreak(START)) {
        Serial.println("Started! ");
      //  blueToothWrite("Started! ");
        startCzas();
        state = 2;
      }
      break;

    case 2:
      countingTime();
      if (countLaserBreak(2)) {
        state = 3;
      }
      break;

    case 3:
      Serial.println("Finished! ");
    //  blueToothWrite("Finished! ");

      listOfResults[index++] = finishTime() / 1000;
      state = 0;
      break;

    case 4:
      Serial.println("Results: ");
      for (int z = 0; z < index; z++) {
        Serial.print(z);
        Serial.print(". ");
        Serial.print(listOfResults[z]);
        Serial.println(" s.");

        //  blueToothWrite(z);
        //  blueToothWrite(". ");
        //blueToothWrite((String)listOfResults[z]);
        //blueToothWrite(" s.");

      }
      state = 0;
      break;

    default:
      Serial.println("DEFAULT");
   //   blueToothWrite("DEFAULT");
      Serial.end();
  }
}

boolean countLaserBreak(uint8_t number) {
  if (red > 150 || green > 150 || blue > 150) {
    counterBreak++;
  }

  if (counterBreak > number ) {
    counterBreak = 0;
    return true;
  }
  return false;
}

void startCzas() {
  digitalWrite(laser, HIGH);
  startCounterTime = millis();
  zapamietanyCzas = 0, roznicaCzasu = 0;
}

void countingTime() {
  long actualTime = millis();

  roznicaCzasu = actualTime - zapamietanyCzas;

  if (roznicaCzasu >= 1000UL) {
    zapamietanyCzas = actualTime;
    Serial.print((millis() - startCounterTime) / 1000);
    Serial.println(" s.");

   // blueToothWrite((millis() - startCounterTime) / 1000);
   // blueToothWrite(" s.");
  }
}

double finishTime() {
  finishCounterTime = millis() - startCounterTime;
  digitalWrite(laser, LOW);
  return finishCounterTime;
}

String blueToothRead() {
  String content = "";
  char character;

  while (bluetoothSerial.available()) {
    character = bluetoothSerial.read();
    content.concat(character);
  }

  if (content != "") {
    Serial.print(content);
  }
  return content;
}

void blueToothWrite() {

//  if (writeContent == "") {
//    Serial.print("The Bluetooth writeContent is empty");
//  }


  //bluetoothSerial.write(tab2);

    if (Serial.available()) {
    bluetoothSerial.write(Serial.read());
  } 
}
