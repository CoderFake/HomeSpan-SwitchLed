/*********************************************************************************
 *  MIT License
 *
 *  Copyright (c) 2023 HoangDieuIT
 *
 *  https://github.com/HomeSpan/HomeSpan
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 ********************************************************************************/

#define NEOPIXEL_RGB_PIN       25
#define TABLELAMP_PIN          33 
#define DECORATELAMP_PIN       32
#define LIGHT_PIN              27
#define buzzerPin              16 
#define DEVICE_SUFFIX          ""
#include "HomeSpan.h"
#include "extras/Pixel.h"  

struct NeoPixel_RGB : Service::LightBulb {      
  Characteristic::On power{0,true};
  Characteristic::Hue H{0,true};
  Characteristic::Saturation S{0,true};
  Characteristic::Brightness V{100,true};
  Pixel *pixel;
  uint8_t nPixels;
  int pre_Status;
  
  NeoPixel_RGB(uint8_t pin, uint8_t nPixels) : Service::LightBulb(){

    V.setRange(0,100,1);                      
    pixel=new Pixel(pin);           
    this->nPixels=nPixels; 
    this->pre_Status = !digitalRead(5);                   
    update();                                 
  }
  boolean update() override {

    int p = power.getNewVal();     
    Pixel::Color color;

    int current_Status = digitalRead(5);
    if(V.getVal() != V.getNewVal() || H.getVal() != H.getNewVal()|| S.getVal() != S.getNewVal()){
      pixel->set(color.HSV(H.getNewVal()*p, S.getNewVal()*p, V.getNewVal()*p),nPixels); 
      if(V.getNewVal() == 0 && pre_Status != current_Status){
        p = false;
      } 
      else p = true;
    }
    if (pre_Status == current_Status) {
      return true;
    }
    if (current_Status != p) {
      float v = (p == true ? 0 : 100);
      p = current_Status;
      power.setVal(digitalRead(5), true);
      float h=H.getNewVal<float>();       
      float s=S.getNewVal<float>();   
      pixel->set(color.HSV(h*p, s*p, v*p),nPixels);
      pre_Status = current_Status;
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin,LOW);
      delay(200); 
    } else {
      p = current_Status;
      pre_Status = current_Status;
      power.setVal(digitalRead(5), true);
      float h=H.getNewVal<float>();       
      float s=S.getNewVal<float>();
      float v = V.getNewVal<float>();    
      if(v == 0) p = 0;
      pixel->set(color.HSV(h*p, s*p, v*p),nPixels);
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin,LOW);
      delay(200);
    }
    return(true);  
  }
  void loop()
  {
    update();
  }
};


struct TableLamp : Service::LightBulb {
  int lampPin;
  SpanCharacteristic *lampPower;
  int led_button;
  SpanButton *Button;
  int pre_buttonStatus; 

  TableLamp(int lampPin, int led_button) : Service::LightBulb() {
    lampPower = new Characteristic::On();
    Button = new SpanButton(led_button);
    this->led_button = led_button;
    this->lampPin = lampPin;
    this->pre_buttonStatus = !digitalRead(led_button);
    pinMode(lampPin, OUTPUT);
    lampPower->setVal(digitalRead(led_button));
  }
  boolean update() {
    int lampStatus = lampPower->getNewVal();
    int current_buttonStatus = digitalRead(led_button);
    if (lampStatus == digitalRead(lampPin)) {
      digitalWrite(lampPin, !lampStatus);
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin,LOW);
      delay(200); 
    }
    if (pre_buttonStatus == current_buttonStatus) {
      return true;
    }

    if (current_buttonStatus == digitalRead(lampPin)) {
      lampPower->setVal(1 - lampStatus);
      pre_buttonStatus = current_buttonStatus;
      digitalWrite(lampPin, !current_buttonStatus);
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin,LOW);
      delay(500); 
    } else {
      lampPower->setVal(current_buttonStatus);
      pre_buttonStatus = current_buttonStatus;
      digitalWrite(lampPin, !current_buttonStatus);
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin,LOW);
      delay(200);
    }

    return true;
  }
  void loop(){
      update(); 
  }

};



void setup() {
  
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT); 
  homeSpan.begin(Category::Lighting,"HoangDieu Led" DEVICE_SUFFIX);                                             
  SPAN_ACCESSORY("Neo RGB");
    new NeoPixel_RGB(NEOPIXEL_RGB_PIN,179); 
    new TableLamp(TABLELAMP_PIN, 19);
    new TableLamp(DECORATELAMP_PIN, 18);
    new TableLamp(LIGHT_PIN, 17); 
}

void loop() {
  homeSpan.poll(); 
}