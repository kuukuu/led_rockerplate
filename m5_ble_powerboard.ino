#include <BLEDevice.h>
#include <M5Stack.h>
#include <FastLED.h>

/*
how to use.

1.set serviceUUID empty, like a bellow
  static BLEUUID serviceUUID("");
2.build and run to find your service UUID
3.you will find all BLE device list, check your power source's UUID
4.set your serviceUUID and DEF_BLE_ADDR.
5.set TARGET_CHARA empty and build & run.
5.check characteristic list ofrom your power source.
  XXXXXX63-XXXXXXXX******  is power data.
6.set your TARGET_CHARA
7.set your power zone at setRGB method.

tested device
- M5stack gray
  M5stack official ver.1.0.6
- WS2811 LED tape
- 12V 3A ac adapter
- wahoo kickr core

*/

//LED
#define LED_PIN     16
#define NUM_LEDS    100
#define BRIGHTNESS  255
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];


#define DEF_BLE_ADDR "c1:5a:e9:b1:04:2a" // my kickr core c1:5a:e9:b1:04:2a
#define TARGET_CHARA "00002a63-0000-1000-8000-00805f9b34fb" // its my kickr's id, find your charas

static BLEUUID serviceUUID("00001818-0000-1000-8000-00805f9b34fb");// its my kickr, find yours

static BLEAdvertisedDevice* myDevice;
static BLERemoteService* pRemoteService;
static BLEClient*  pClient;

/////////////////////////////////////////////////////////////////
/////LED
/////////////////////////////////////////////////////////////////
int rgb[]= {100, 100, 100}; // RGB
int power = 0;
int powerA1 = 0;
int powerA2 = 0;
int power3sec = 0;

void ledOn() {
  FastLED.clear();  // clear all pixel data
  for(int i=0; i<NUM_LEDS; i++) { // For each pixel...
    leds[i] = CRGB( rgb[0],rgb[2],rgb[1]);//RBG!!!
  }
    FastLED.show();
}
void setRGB() {
  power3sec = (power+powerA1+powerA2)/3;
  
  Serial.print(" power color: ");
  Serial.print(power);
  Serial.print(",");
  Serial.print(powerA1);
  Serial.print(",");
  Serial.print(powerA2);
  Serial.print("::: ");
  /*
    FTP 285 
    Zone 1 gray …-59%
    Zone 2 blue60-75%  170
    Zone 3 green 76-89% 215
    Zone 4 yellow 90-104% 255
    Zone 5 orange 105-118% 300
    Zone 6 red 119%-340
    
    // its my power. change it.
  */
  if(power3sec > 340){
    rgb[0] = 255;
    rgb[1] = 0;
    rgb[2] = 0;
  Serial.print("red");
  }else if(power3sec > 300){
    rgb[0] = 255;
    rgb[1] = 110;
    rgb[2] = 0;
  Serial.print("yellow");
  }else if(power3sec > 255){
    rgb[0] = 255;
    rgb[1] = 255;
    rgb[2] = 0;
  Serial.print("orange");
  }else if(power3sec > 215){
    rgb[0] = 0;
    rgb[1] = 255;
    rgb[2] = 0;
  Serial.print("green");
  }else if(power3sec > 170){
    rgb[0] = 0;
    rgb[1] = 0;
    rgb[2] = 255;
  Serial.print("blue");
  } else {
    rgb[0] = 30;
    rgb[1] = 30;
    rgb[2] = 30;
  Serial.print("gray");
  }
  Serial.print(power3sec);
  Serial.println();
  powerA1 = power;
  powerA2 = powerA1;
}
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

bool  doConnect = false;                
bool  doScan = false;                   
bool  deviceConnected = false;          
bool  isRegistered = false;          

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
  //Serial.print(" power: ");
  power = pData[3]*256+pData[2];
  //Serial.print(power);
  //Serial.println();
}

bool readFromServer() {
      //Serial.println("readFromServer");
}

bool getFromServer() {
      Serial.println("getFromServer");

      if (pRemoteService == nullptr) {
        pClient->disconnect();
        Serial.print("disconnect ");
      }
        
      Serial.print("TARGET_CHARA:");
      Serial.println(TARGET_CHARA);
  std::map<std::string, BLERemoteCharacteristic*>* mapCharacteristics = pRemoteService->getCharacteristics();
  for (std::map<std::string, BLERemoteCharacteristic*>::iterator i = mapCharacteristics->begin(); i != mapCharacteristics->end(); ++i) {

    if( TARGET_CHARA == i->first){
      Serial.println("/// found power mesurement!:");
      Serial.print(" getHandle:");
      Serial.printf("%u",i->second->getHandle());
      Serial.print(" readUInt8:");
      Serial.printf("%u",i->second->readUInt8());
      Serial.print(" readUInt16:");
      Serial.printf("%u",i->second->readUInt16());
      Serial.print(" readUInt32:");
      Serial.printf("%u",i->second->readUInt32());
      Serial.print(" readValue:");
      Serial.printf(i->second->readValue().c_str());
      Serial.print(" toString:");
      Serial.printf(i->second->toString().c_str());
      Serial.println("/// end power mesurement!:");
      if (i->second->canNotify()) {
        Serial.println(" - Add Notify");
        i->second->registerForNotify(notifyCallback);
        isRegistered = true;
      }
    }else{
      //Serial.print("other chara:");
      //Serial.println(i->first.c_str());
    }
  }
  return true;
}



/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

bool connectToServer() {
      Serial.println("connectToServer");
  
        Serial.println("doConnect1");
        pClient->connect(myDevice);
        Serial.println("doConnect2");
        pRemoteService = pClient->getService(serviceUUID);
}
class funcClientCallbacks: public BLEClientCallbacks {
    void onConnect(BLEClient* pClient) {
        Serial.println("funcClientCallbacks onConnect");
        deviceConnected = true;
    };
    void onDisconnect(BLEClient* pClient) {
        Serial.println("funcClientCallbacks onDisconne");
        deviceConnected = false;
        doConnect = false;
        isRegistered = false;
        startConnection();
    }
};
bool startConnection() {
        Serial.println("connectToServer");
        pClient  = BLEDevice::createClient();
        pClient->setClientCallbacks(new funcClientCallbacks());
        doConnect = true;
        //end scan
        Serial.println("end scan");
        BLEDevice::getScan()->stop();
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.print("BLE found");
      Serial.println(advertisedDevice.toString().c_str());
      if( DEF_BLE_ADDR == advertisedDevice.getAddress().toString() ){
        // connect if my target devicce
          Serial.println("connect:");
        BLEDevice::getScan()->stop();
        myDevice = new BLEAdvertisedDevice(advertisedDevice);
        Serial.print("connect to:");
        Serial.println(myDevice->getAddress().toString().c_str());
  
        Serial.println("ble_name:");
        Serial.println(myDevice->getName().c_str());
        Serial.println("getServiceUUID:");
        Serial.println(myDevice->getServiceUUID().toString().c_str());

        //start connection
        startConnection();
      }
    }
};
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  //LED
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
  
  Serial.begin(115200);
  Serial.println("BLE scan start");
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(10);
}
void loop() {
  //Serial.println("loop");
  if (doConnect == false) {
  }else if (deviceConnected == true) {
    if (isRegistered) {
      readFromServer();
    }else{
      getFromServer();
    }
  }else{
    connectToServer();
  }

  //LED
  setRGB();   
  ledOn();    
  delay(1000);
}
