#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
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
#include "led/Spooky.h"
#include "led/LiveDraw.h"

// Define the UUIDs for the BLE service and characteristic
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;

std::string currentAnimation = "";
std::vector<uint32_t> chosenColors;

// Shuffle: the cape itself moves to a different preset every minute, so it keeps going
// with the phone locked in a pocket. Any other animation command stops it.
#define SHUFFLE_INTERVAL_MS 60000
static const char *shufflePlaylist[] = {
    "bonfire", "ghostfire", "witchfire", "inferno", "heartbeat", "ghost", "drip", "ecto",
    "cauldron", "banshee", "glitch", "reaper", "rain", "pumpkin", "candycorn", "aurora",
    "rainbow_flow",
    "pulse:833ab4,fd1d1d,fcb045", // sunset pulse
    "pulse:00ff40,6a00ff",        // toxic pulse
    "flow:ff4500,8000ff,00ff40",  // halloween flow
    "flow:ff0000,200000",         // blood flow
    "emanate:6a00ff,00ffaa",      // spectral emanate
    "emanate:ff2a00,ffb000",      // ember emanate
};
#define SHUFFLE_COUNT (sizeof(shufflePlaylist) / sizeof(shufflePlaylist[0]))
bool shuffling = false;
unsigned long lastShuffle = 0;
std::string playingCommand = "rainbow_flow"; // last animation command, so a pick always changes it

// BLE writes arrive on the Bluetooth task; queue them so only loop() touches animation state
#define MAX_MESSAGE_LENGTH 128
QueueHandle_t messageQueue;

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    Serial.println("Client Connected");
    // Keep advertising: if a phone's Bluetooth keeps a stale link after its app dies, the
    // cape must still be discoverable for a fresh connection.
    pServer->startAdvertising();
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
  else if (value.find("p:") == 0)
  {
    currentAnimation = "draw";
    handleDrawPoints(value.substr(2));
  }
  else if (value.find("u:") == 0)
  {
    handleDrawFingerUp(value.substr(2));
  }
  else if (findSpookyAnimation(value) != nullptr)
  {
    currentAnimation = value;
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

void shuffleNext()
{
  // Every tap (including the first) and every minute moves to something new
  int next;
  do
  {
    next = random(SHUFFLE_COUNT);
  } while (playingCommand == shufflePlaylist[next]);
  playingCommand = shufflePlaylist[next];
  lastShuffle = millis();
  parseColors(shufflePlaylist[next]);

  // Tell the phone what's playing; reconnecting phones read the same value
  std::string value = std::string("shuffle>") + shufflePlaylist[next];
  pCharacteristic->setValue(value);
  pCharacteristic->notify();
  Serial.printf("Shuffle: %s\n", shufflePlaylist[next]);
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; // Wait for serial port to connect. Needed for native USB only.
  setupStrips();
  messageQueue = xQueueCreate(16, MAX_MESSAGE_LENGTH); // live draw sends ~30 messages/second

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
          BLECharacteristic::PROPERTY_WRITE_NR | // live draw points are sent without response
          BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->addDescriptor(new BLE2902()); // lets the phone subscribe to notifications
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
    if (message[0] != 'p' || message[1] != ':') // live draw points would flood the log
    {
      Serial.print("Msg: ");
      Serial.println(message);
    }
    std::string value(message);
    if (value == "shuffle")
    {
      shuffling = true;
      shuffleNext();
    }
    else
    {
      if (value.find("brightness:") != 0)
      {
        shuffling = false; // the wearer picked something else
        if (value.find("p:") != 0 && value.find("u:") != 0)
          playingCommand = value;
      }
      parseColors(value);
    }
  }
  if (shuffling && millis() - lastShuffle >= SHUFFLE_INTERVAL_MS)
  {
    shuffleNext();
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
  else if (const SpookyAnimation *spooky = findSpookyAnimation(currentAnimation))
  {
    runSpookyAnimation(spooky);
  }
}
