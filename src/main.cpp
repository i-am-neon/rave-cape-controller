#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "shared/LedStrip.h"
#include "led/BlueLEDs.h"
#include "led/RedLEDs.h"
#include "led/RainbowFlow.h"
#include "led/PulseOneColor.h"
#include "led/EmanateOneColor.h"
#include "led/FlowOneColor.h"
#include "led/Gradient.h"

// Define the UUIDs for the BLE service and characteristic
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;

std::string currentAnimation = "";
uint32_t currentColor = 0xFF0000; // Default to red
uint32_t color1 = 0xFF0000;
uint32_t color2 = 0xFFFF00;

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    Serial.println("Client Connected");
  }

  void onDisconnect(BLEServer *pServer)
  {
    Serial.println("Client Disconnected");
  }
};

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();
    Serial.print("Msg: ");
    if (value.length() > 0)
    {
      Serial.println(value.c_str());
      if (value == "blue")
      {
        currentAnimation = "";
        setAllLEDsToBlue();
      }
      else if (value == "red")
      {
        currentAnimation = "";
        setAllLEDsToRed();
      }
      else if (value == "rainbow_flow")
      {
        currentAnimation = "rainbow_flow";
      }
      else if (value.find("flow_one_color:") == 0)
      {
        currentAnimation = "flow_one_color";
        currentColor = strtol(value.substr(16).c_str(), NULL, 16); // Parse the hex color code
        Serial.print("Color: ");
        Serial.println(currentColor, HEX);
      }
      else if (value.find("pulse_one_color:") == 0)
      {
        currentAnimation = "pulse_one_color";
        currentColor = strtol(value.substr(16).c_str(), NULL, 16); // Parse the hex color code
        Serial.print("Color: ");
        Serial.println(currentColor, HEX);
      }
      else if (value.find("emanate_one_color:") == 0)
      {
        currentAnimation = "emanate_one_color";
        currentColor = strtol(value.substr(16).c_str(), NULL, 16); // Parse the hex color code
        Serial.print("Color: ");
        Serial.println(currentColor, HEX);
      }
      else if (value.find("gradient:") == 0)
      {
        currentAnimation = "gradient";
        size_t colonPos = value.find(':');
        size_t commaPos = value.find(',', colonPos + 1);
        color1 = strtol(value.substr(colonPos + 1, commaPos).c_str(), NULL, 16);
        color2 = strtol(value.substr(commaPos + 1).c_str(), NULL, 16);
      }
      else if (value.find("brightness:") == 0)
      {
        int newBrightness = atoi(value.substr(11).c_str());
        setBrightness(newBrightness);
      }
    }
    else
    {
      Serial.println("Empty");
    }
  }
};

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; // Wait for serial port to connect. Needed for native USB only.
  setupStrips();
  // Set the initial animation
  currentAnimation = "gradient";
  // currentAnimation = "rainbow_flow";
  Serial.println("Starting BLE work!");

  BLEDevice::init("RaveCapeController");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->setValue("Hello From RaveCapeController!");
  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  pService->start();
  pServer->getAdvertising()->start();

  Serial.println("BLE is ready and waiting for connections.");
}

void loop()
{
  if (currentAnimation == "rainbow_flow")
  {
    setRainbowFlow();
  }
  else if (currentAnimation == "flow_one_color")
  {
    setFlowOneColor(0xFF0000);
    // setFlowOneColor(currentColor);
  }
  else if (currentAnimation == "pulse_one_color")
  {
    setPulseOneColor(currentColor);
  }
  else if (currentAnimation == "emanate_one_color")
  {
    setEmanateOneColor(0xFF0000);
    // setEmanateOneColor(currentColor);
  }
  else if (currentAnimation == "gradient")
  {
    setGradient(color1, color2);
  }
}
