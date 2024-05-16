#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "shared/LedStrip.h"
#include "led/BlueLEDs.h"
#include "led/RedLEDs.h"
#include "led/RainbowFlow.h"
#include "led/Pulse.h"
#include "led/EmanateOneColor.h"
#include "led/FlowOneColor.h"
#include "led/Solid.h"
#include "led/Flow.h" // Include the new header

// Define the UUIDs for the BLE service and characteristic
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;

std::string currentAnimation = "";
std::vector<uint32_t> chosenColors;

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
      parseColors(value);
    }
    else
    {
      Serial.println("Empty");
    }
  }

  void parseColors(const std::string &value)
  {
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
    else if (value.find("solid:") == 0)
    {
      currentAnimation = "solid";
      parseChosenColors(value.substr(value.find(':') + 1));
    }
    else if (value.find("brightness:") == 0)
    {
      int newBrightness = atoi(value.substr(11).c_str());
      setBrightness(newBrightness);
    }
    else if (value.find("flow:") == 0)
    {
      currentAnimation = "flow";
      parseChosenColors(value.substr(value.find(':') + 1));
    }
    else
    {
      parseAnimationAndColor(value);
    }
  }

  void parseAnimationAndColor(const std::string &value)
  {
    size_t colonPos = value.find(':');
    if (colonPos != std::string::npos)
    {
      currentAnimation = value.substr(0, colonPos);
      parseChosenColors(value.substr(colonPos + 1));
    }
  }

  void parseChosenColors(const std::string &colorString)
  {
    chosenColors.clear();
    size_t pos = 0;
    while (pos < colorString.length())
    {
      size_t nextComma = colorString.find(',', pos);
      if (nextComma == std::string::npos)
        nextComma = colorString.length();
      uint32_t color = strtol(colorString.substr(pos, nextComma - pos).c_str(), NULL, 16);
      chosenColors.push_back(color);
      pos = nextComma + 1;
    }
  }
};

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; // Wait for serial port to connect. Needed for native USB only.
  setupStrips();

  // Starting animation
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
  else if (currentAnimation == "flow_one_color" && !chosenColors.empty())
  {
    setFlowOneColor(chosenColors[0]);
  }
  else if (currentAnimation == "pulse" && !chosenColors.empty())
  {
    setPulse(chosenColors);
  }
  else if (currentAnimation == "emanate_one_color" && !chosenColors.empty())
  {
    setEmanateOneColor(chosenColors[0]);
  }
  else if (currentAnimation == "solid" && !chosenColors.empty())
  {
    setSolid(chosenColors);
  }
  else if (currentAnimation == "flow" && chosenColors.size() >= 2)
  {
    setFlow(chosenColors);
  }
}
