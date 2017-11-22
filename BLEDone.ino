/*
   Copyright (c) 2015 Intel Corporation.  All rights reserved.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
   This sketch example partially implements the standard Bluetooth Low-Energy Heart Rate service.
   For more information: https://developer.bluetooth.org/gatt/services/Pages/ServicesHome.aspx
*/

#include <CurieBLE.h>
#include "MutichannelGasSensor.h"

#define TH1 45
#define TH2 95
#define TH3 200
#define TH4 400
#define TH5 600

// Conversion factor - CPM to uSV/h
#define CONV_FACTOR 0.00812

int ledArray [] = {10,11,12,13,9};
int geiger_input = 2;
long count = 0;

BLEPeripheral blePeripheral;       // BLE Peripheral Device (the board you're programming)
BLEService DustService("180D"); // BLE Heart Rate Service
/*BLEService COService("180A");
BLEService NO2Service("180B");*/


// BLE Heart Rate Measurement Characteristic
BLECharacteristic DustChar("2A37",  // standard 16-bit characteristic UUID
    BLERead | BLENotify, 8);  // remote clients will be able to get notifications if this characteristic changes
                              // the characteristic is 2 bytes long as the first field needs to be "Flags" as per BLE specifications
                              // https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.heart_rate_measurement.xml
BLECharacteristic COChar("2A29", BLERead | BLENotify, 5);
BLECharacteristic NO2Char("2B29", BLERead | BLENotify, 5);
BLECharacteristic RadChar("2C29", BLERead | BLENotify, 8);                              




void setup() {
  Serial.begin(9600);    // initialize serial communicatio
  //pinMode(13, OUTPUT);   // initialize the LED on pin 13 to indicate when a central is connected

  //pinMode(8,INPUT);

  pinMode(geiger_input, INPUT);
  digitalWrite(geiger_input,HIGH);
  for (int i=0;i<5;i++){
  pinMode(ledArray[i],OUTPUT);
  }

   attachInterrupt(0,countPulse,FALLING);
   
  //gas.begin(0x04);//the default I2C address of the slave is 0x04
  //gas.powerOn();
  
  /* Set a local name for the BLE device
     This name will appear in advertising packets
     and can be used by remote devices to identify this BLE device
     The name can be changed but maybe be truncated based on space left in advertisement packet */
  blePeripheral.setLocalName("TestFloatNumber");
  blePeripheral.setAdvertisedServiceUuid(DustService.uuid());  // add the service UUID
  blePeripheral.addAttribute(DustService);   // Add the BLE Heart Rate service
  blePeripheral.addAttribute(DustChar); // add the Heart Rate Measurement characteristic
  blePeripheral.addAttribute(COChar); // add the Heart Rate Measurement characteristic
  blePeripheral.addAttribute(NO2Char); // add the Heart Rate Measurement characteristic
  blePeripheral.addAttribute(RadChar); // add the Heart Rate Measurement characteristic
/*
  blePeripheral.setAdvertisedServiceUuid(COService.uuid());  // add the service UUID
  blePeripheral.addAttribute(COService);   // Add the BLE Heart Rate service
  blePeripheral.addAttribute(COChar); // add the Heart Rate Measurement characteristic

  blePeripheral.setAdvertisedServiceUuid(NO2Service.uuid());  // add the service UUID
  blePeripheral.addAttribute(NO2Service);   // Add the BLE Heart Rate service
  blePeripheral.addAttribute(NO2Char); // add the Heart Rate Measurement characteristic*/

  /* Now activate the BLE device.  It will start continuously transmitting BLE
     advertising packets and will be visible to remote BLE central devices
     until it receives a new connection */
  blePeripheral.begin();
  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    // turn on the LED to indicate the connection:
    digitalWrite(13, HIGH);

    // check the heart rate measurement every 200ms
    // as long as the central is still connected:
    while (central.connected()) {
   
          getDust();
          getCO();  
          getNO2();
          getRad_act();
          delay(5000);
    }
    // when the central disconnects, turn off the LED:
    digitalWrite(13, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}

void getDust() {
    float Dust;
    Dust = updateDust();

    String numberDust = String(Dust);
    
    Serial.print("test numberDust");
    Serial.println(numberDust);
    int sizeArray ;
    unsigned char testArray [sizeArray];
    char charBuf[10];
    numberDust.toCharArray(charBuf,10);
      if(Dust > 10000){
        Serial.println("loop 10000");
         sizeArray = 8;
          for(int i=0;i<sizeof(testArray);i++){
            int num = charBuf[i];
            testArray[i] = (char)num;
          }
      }else if(Dust > 1000){
        Serial.println("loop 1000");
         sizeArray = 7;
          for(int i=0;i<sizeof(testArray);i++){
            int num = charBuf[i];
            testArray[i] = (char)num;
          }
      }else if(Dust > 100){
        Serial.println("loop 100");
         sizeArray = 6;
          for(int i=0;i<sizeof(testArray);i++){
            int num = charBuf[i];
            testArray[i] = (char)num;
          }
      }else if(Dust > 10){
        Serial.println("loop 10");
         sizeArray = 5;
          for(int i=0;i<sizeof(testArray);i++){
            int num = charBuf[i];
            testArray[i] = (char)num;
          }
      }else if(Dust > 0){
        Serial.println("loop 0");
         sizeArray = 4;
          for(int i=0;i<sizeof(testArray);i++){
            int num = charBuf[i];
            testArray[i] = (char)num;
          }
      }
     
    
    Serial.print("testarray 0 =");
    Serial.println(testArray[0]);
    Serial.print("testarray 1 =");
    Serial.println(testArray[1]);
    Serial.print("testarray 2 =");
    Serial.println(testArray[2]);
    Serial.print("testarray 3 =");
    Serial.println(testArray[3]);
    Serial.print("testarray 4 =");
    Serial.println(testArray[4]);
     Serial.print("testarray 5 =");
    Serial.println(testArray[5]);
     Serial.print("testarray 6 =");
    Serial.println(testArray[6]);
     Serial.print("testarray 7 =");
    Serial.println(testArray[7]);
      Serial.print("\n");
   
    DustChar.setValue(testArray , sizeArray);
}

void getCO(){
    
    float CO;

    CO = gas.measure_CO();

    String numberCO = String(CO);
 
    Serial.print("test numberCO =");
    Serial.println(numberCO);
 
    
    int sizeArray ;
    unsigned char testArray [sizeArray];
    
    char charBuf[10];
    numberCO.toCharArray(charBuf,10);
      if(CO > 10){
        Serial.println("loop 10");
         sizeArray = 5;
          for(int i=0;i<sizeof(testArray);i++){
            int num = charBuf[i];
            testArray[i] = (char)num;
          }
      }else if(CO > 0){
        Serial.println("loop 0");
         sizeArray = 4;
          for(int i=0;i<sizeof(testArray);i++){
            int num = charBuf[i];
            testArray[i] = (char)num;
          }
      }
    
    Serial.print("testarray 0 =");
    Serial.println(testArray[0]);
    Serial.print("testarray 1 =");
    Serial.println(testArray[1]);
    Serial.print("testarray 2 =");
    Serial.println(testArray[2]);
    Serial.print("testarray 3 =");
    Serial.println(testArray[3]);
    Serial.print("testarray 4 =");
    Serial.println(testArray[4]);
      Serial.print("\n");
   
    COChar.setValue(testArray , sizeArray);
 
}

void getNO2(){
    
    float NO2;

    NO2 = gas.measure_NO2();

    String numberNO2 = String(NO2);
 
    Serial.print("test numberNO2 =");
    Serial.println(numberNO2);
 
    
    int sizeArray ;
    unsigned char testArray [sizeArray];
    
    char charBuf[10];
    numberNO2.toCharArray(charBuf,10);
      if(NO2 > 10){
        Serial.println("loop 10");
         sizeArray = 5;
          for(int i=0;i<sizeof(testArray);i++){
            int num = charBuf[i];
            testArray[i] = (char)num;
          }
      }else if(NO2 > 0){
        Serial.println("loop 0");
         sizeArray = 4;
          for(int i=0;i<sizeof(testArray);i++){
            int num = charBuf[i];
            testArray[i] = (char)num;
          }
      }
    
    Serial.print("testarray 0 =");
    Serial.println(testArray[0]);
    Serial.print("testarray 1 =");
    Serial.println(testArray[1]);
    Serial.print("testarray 2 =");
    Serial.println(testArray[2]);
    Serial.print("testarray 3 =");
    Serial.println(testArray[3]);
    Serial.print("testarray 4 =");
    Serial.println(testArray[4]);
      Serial.print("\n");
   
    NO2Char.setValue(testArray , sizeArray);
}

void getRad_act(){
    
    float radiationValue = 0.0;
    long countPerMinute = 0;

    countPerMinute = 6*count;
    radiationValue = countPerMinute * CONV_FACTOR;

    String numberRad_act = String(radiationValue);
 
    Serial.print("test numberRad_act =");
    Serial.println(numberRad_act);
 
    
    int sizeArray ;
    unsigned char testArray [sizeArray];
    
    char charBuf[10];
    numberRad_act.toCharArray(charBuf,10);
       if(radiationValue > 100){
        Serial.println("loop 100");
         sizeArray = 6;
          for(int i=0;i<sizeof(testArray);i++){
            int num = charBuf[i];
            testArray[i] = (char)num;
          }
      }
      else if(radiationValue > 10){
        Serial.println("loop 10");
         sizeArray = 5;
          for(int i=0;i<sizeof(testArray);i++){
            int num = charBuf[i];
            testArray[i] = (char)num;
          }
      }else if(radiationValue >= 0){
        Serial.println("loop 0");
         sizeArray = 4;
          for(int i=0;i<sizeof(testArray);i++){
            int num = charBuf[i];
            testArray[i] = (char)num;
          }
      }
    
    Serial.print("testarray 0 =");
    Serial.println(testArray[0]);
    Serial.print("testarray 1 =");
    Serial.println(testArray[1]);
    Serial.print("testarray 2 =");
    Serial.println(testArray[2]);
    Serial.print("testarray 3 =");
    Serial.println(testArray[3]);
    Serial.print("testarray 4 =");
    Serial.println(testArray[4]);
      Serial.print("\n");


    if(countPerMinute <= TH1) ledVar(0);
    if((countPerMinute <= TH2)&&(countPerMinute>TH1)) ledVar(1);
    if((countPerMinute <= TH3)&&(countPerMinute>TH2)) ledVar(2);
    if((countPerMinute <= TH4)&&(countPerMinute>TH3)) ledVar(3);
    if((countPerMinute <= TH5)&&(countPerMinute>TH4)) ledVar(4);
    if(countPerMinute>TH5) ledVar(5);

    count = 0;

    
    RadChar.setValue(testArray , sizeArray);
 
}


float updateDust(){
  
  int pin = 8;
  unsigned long duration;
  unsigned long starttime;
  unsigned long sampletime_ms = 2000;//sampe 30s&nbsp;;
  unsigned long lowpulseoccupancy = 0;
  float ratio = 0;
  float concentration = 0;
  duration = pulseIn(pin, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;

 
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=&gt;100
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
    lowpulseoccupancy = 0;
    starttime = millis();

     double pi = 3.14159;
     // All particles are spherical, with a density of 1.65E12 µg/m3
     double density = 1.65 * pow (10, 12);
     // The radius of a particle in the PM2.5 channel is .44 µm
    double r25 = 0.44 * pow (10, -6);
     double vol25 = (4/3) * pi * pow (r25, 3);
     double mass25 = density * vol25; // ug
     double K = 3531.5; // per m^3 
     
    return concentration * K * mass25;
    
}


void countPulse(){
  detachInterrupt(0);
  count++;
  while(digitalRead(2)==0){
  }
  attachInterrupt(0,countPulse,FALLING);
}

void ledVar(int value){
  if (value > 0){
    for(int i=0;i<=value;i++){
      digitalWrite(ledArray[i],HIGH);
    }
    for(int i=5;i>value;i--){
      digitalWrite(ledArray[i],LOW);
    }
  }
  else {
    for(int i=5;i>=0;i--){
      digitalWrite(ledArray[i],LOW);
    }
  }
}



