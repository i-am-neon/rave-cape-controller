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
#include "led/Emanate.h"
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

// BLE writes arrive on the Bluetooth task; queue them so only loop() touches animation state
#define MAX_MESSAGE_LENGTH 128
QueueHandle_t messageQueue;

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    Serial.println("Client Connected");
  }

  void onDisconnect(BLEServer *pServer)
  {
    Serial.println("Client Disconnected");
    pServer->startAdvertising(); // Stay discoverable so the phone can reconnect
  }
};

void parseColors(const std::string &value);
void parseAnimationAndColor(const std::string &value);
void parseChosenColors(const std::string &colorString);

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();
    if (value.length() == 0 || value.length() >= MAX_MESSAGE_LENGTH)
    {
      Serial.println("Msg: Empty or too long");
      return;
    }

    char message[MAX_MESSAGE_LENGTH] = {0};
    value.copy(message, value.length());
    if (xQueueSend(messageQueue, message, 0) != pdTRUE)
    {
      Serial.println("Msg: Queue full, dropped");
    }
  }
};

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
  else if (value.find("emanate:") == 0)
  {
    currentAnimation = "emanate";
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

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; // Wait for serial port to connect. Needed for native USB only.
  setupStrips();
  messageQueue = xQueueCreate(8, MAX_MESSAGE_LENGTH);

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
  char message[MAX_MESSAGE_LENGTH];
  while (xQueueReceive(messageQueue, message, 0) == pdTRUE)
  {
    Serial.print("Msg: ");
    Serial.println(message);
    parseColors(message);
  }

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
  else if (currentAnimation == "emanate" && !chosenColors.empty())
  {
    setEmanate(chosenColors);
  }
}
