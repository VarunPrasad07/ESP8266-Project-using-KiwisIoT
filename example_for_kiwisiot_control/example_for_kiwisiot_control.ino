#include <KiwisIoT.h>
 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";
KiwisIoT kiwisiot(ssid, pass, topic);
#define LED D2
void setup(){
  Serial.begin(115200);
  kiwisiot.begin();
  kiwisiot.onReceive(onControl);
  
}
void onControl(String ch, String val){
  Serial.println(val);
  val.trim();
  if (ch!="1") return;
    if (val=="1"){
      digitalWrite(LED,HIGH);
    }
    else {
      digitalWrite(LED,LOW);
    }
  }
  
  void loop(){
    kiwisiot.run();
  }
