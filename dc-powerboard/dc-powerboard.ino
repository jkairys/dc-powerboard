#include <IOThing.h>
#define WIFI_SSID "mushroom"
#define WIFI_PASS "gumboots"
#define IOT_HOSTNAME "dc-powerboard"
#define MQTT_SERVER "192.168.0.3"
#define NTP_SERVER "192.168.0.3"

IOThing iot(IOT_HOSTNAME);

#define PIN_RELAY_1 D1
#define PIN_RELAY_2 D2
#define PIN_RELAY_3 D3
#define PIN_RELAY_4 D4

#define PIN_SW_1 D0
#define PIN_SW_2 D5
#define PIN_SW_3 D6
#define PIN_SW_4 D7



#define RELAY_ON 0
#define RELAY_OFF 1

byte relay_pin[4] = {PIN_RELAY_1, PIN_RELAY_2, PIN_RELAY_3, PIN_RELAY_4};
byte relay_state[4] = {RELAY_OFF, RELAY_OFF, RELAY_OFF, RELAY_OFF};


void bg(){
  iot.loop();
  yield();
}

void set_relay(byte _relay_number, byte _relay_state){
  Serial.println("Setting " + String(_relay_number) + " to state " + String(_relay_state));
  digitalWrite(relay_pin[_relay_number-1], _relay_state);
  relay_state[_relay_number-1] = _relay_state;
}

byte get_relay(byte _relay_number){
  return relay_state[_relay_number -1];
}

void debounce(){
  unsigned long tStop = millis() + 500;
  while (millis() < tStop){
    bg();
  }
}


void toggle_relay(byte _relay_number){
  byte newState = relay_state[_relay_number - 1] == RELAY_ON ? RELAY_OFF : RELAY_ON;
  set_relay(_relay_number, newState);
}

void intSw1(){
  toggle_relay(1);
  Serial.println("SW1");
  debounce();
}

void intSw2(){
  toggle_relay(2);
  Serial.println("SW2");
  debounce();
}
void intSw3(){
  toggle_relay(3);
  Serial.println("SW3");
  debounce();
}
void intSw4(){
  toggle_relay(4);
  Serial.println("SW4");
  debounce();
}



void attachInterrupts(){

  pinMode(PIN_SW_2, INPUT);
  attachInterrupt(PIN_SW_2, intSw2, FALLING);

  pinMode(PIN_SW_3, INPUT);
  attachInterrupt(PIN_SW_3, intSw3, FALLING);

  pinMode(PIN_SW_4, INPUT);
  attachInterrupt(PIN_SW_4, intSw4, FALLING);

}

void setup(){
  Serial.begin(115200);

  Serial.println("Configuring WiFi");
  iot.useWiFi(WIFI_SSID, WIFI_PASS);

  Serial.println("Configuring OTA");
  iot.useOTA();

  Serial.println("Configuring NTP");
  iot.useNTP(NTP_SERVER);

  Serial.println("Configuring MQTT");
  iot.useMQTT(MQTT_SERVER, [](String topic, String message){
    Serial.println("Got setting: " + topic + " = " + message);
  });

  pinMode(PIN_SW_1, INPUT);
  pinMode(PIN_SW_2, INPUT);
  pinMode(PIN_SW_3, INPUT);
  pinMode(PIN_SW_4, INPUT);

  pinMode(PIN_RELAY_1, OUTPUT);
  pinMode(PIN_RELAY_2, OUTPUT);
  pinMode(PIN_RELAY_3, OUTPUT);
  pinMode(PIN_RELAY_4, OUTPUT);

  Serial.println("Initialising relay states");
  for(byte i=1;i <= 4;i++){
    set_relay(i, relay_state[i-1]);
  }


  Serial.println("Subscribing to 'garage/pb1/set/#'");
  iot.topicSubscribe("garage/pb1/set/#",[](String topic, String payload){
    int relay_number = topic.toInt();
    int relay_state = payload.toInt();
    set_relay(relay_number, relay_state ? RELAY_ON : RELAY_OFF);
    Serial.println("Set Relay: " + topic + " = " + payload);
  });

}



void loop(){
  bg();
  if(!digitalRead(PIN_SW_1)) intSw1();
  if(!digitalRead(PIN_SW_2)) intSw2();
  if(!digitalRead(PIN_SW_3)) intSw3();
  if(!digitalRead(PIN_SW_4)) intSw4();

}
