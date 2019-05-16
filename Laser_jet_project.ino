#define S0 2
#define S1 3
#define S2 4
#define S3 5
#define sensorOut 6
#define laser 12
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
double listOfResults[51] = {0.0};
int index = 0;
boolean menuFlag = true;

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
      Serial.print("Red: ");
      Serial.println(red);
  }
  delay(100);

  if (aktualnyCzasSENSOR - zapamietanyCzasSENSOR_Green >= 300UL) {
    zapamietanyCzasSENSOR_Green = aktualnyCzasSENSOR;
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    green = pulseIn(sensorOut, LOW);
    Serial.print("Green ");
    Serial.println(green);
  }
  delay(100);

  if (aktualnyCzasSENSOR - zapamietanyCzasSENSOR_Blue >= 300UL) {
    zapamietanyCzasSENSOR_Blue = aktualnyCzasSENSOR;
    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    blue = pulseIn(sensorOut, LOW);
       Serial.print("Blue ");
       Serial.println(blue);
  }
  delay(100);
  breakLaser();
}

void breakLaser() {
  
  switch (state) {
    case 0:

      if (menuFlag) {
        bluetoothSerial.println("Wybierz S na Start lub L by wyświetlić listę wyników.");
        menuFlag = false;
      }
      blueToothRead();
      break;

    case 1:
      if (countLaserBreak(START)) {
        blueToothWrite("Start! ");
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
      blueToothWrite("Koniec! ");

      if (index < 51) {
        listOfResults[index++] = finishTime() / 1000;
      } else {
        blueToothWrite("Nie można dodać więcej wyników.");
      }
      state = 0;
      break;

    case 4:
      blueToothWrite("Wyniki: ");
      if (index == 0) {
        blueToothWrite("Lista jest pusta.");
      }
      Serial.print("index: ");
      Serial.println(index);
      for (int z = 0; z < index; z++) {
        Serial.print(z);
        Serial.print(". ");
        Serial.print(listOfResults[z]);
        Serial.println(" s.");

        bluetoothSerial.print(z);
        bluetoothSerial.print(". ");
        bluetoothSerial.print(listOfResults[z]);
        bluetoothSerial.println(" s.");

      }
      state = 0;
      break;
   case 5:
     //kalibracja
     digitalWrite(laser, HIGH);
     blueToothWrite("Kalibracja lasera 5s.");
     delay(5000); 
     digitalWrite(laser, LOW);
     state = 0; 
     break; 
    default:
      blueToothWrite("DEFAULT");
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
    int resultTime = ((millis() - startCounterTime) / 1000);
    bluetoothSerial.print(resultTime);
    bluetoothSerial.print(" s.");
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
    content.trim();
    if (content.equals("S")) {
      state = 1;
    } else if (content.equals("L")) {
      state = 4;
    } else if (content.equals("K")) {
      state = 5;
    }
    blueToothWrite(content);
  }

  return content;
}

void blueToothWrite(String writeContent) {

  Serial.println(writeContent);
  bluetoothSerial.println(writeContent);
}
