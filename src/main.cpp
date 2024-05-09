#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "shared/LedStrip.h"
#include "led/BlueLEDs.h"
#include "led/RedLEDs.h"
#include "led/RainbowFlow.h"

// Define the UUIDs for the BLE service and characteristic
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;

std::string currentAnimation = "";

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
        currentAnimation = ""; // Stop continuous animations
        setAllLEDsToBlue();
      }
      else if (value == "red")
      {
        currentAnimation = ""; // Stop continuous animations
        setAllLEDsToRed();
      }
      else if (value == "rainbow_flow")
      {
        currentAnimation = "rainbow_flow";
      }
      else if (value.rfind("brightness:", 0) == 0)
      { // Check if the command starts with "brightness:"
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
  currentAnimation = "rainbow_flow";
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
}
