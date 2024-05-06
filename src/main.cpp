#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// Define the UUIDs for the BLE service and characteristic
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"        // UUID for the BLE Service
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8" // UUID for the BLE Characteristic

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;

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
    Serial.print("New Value Received: ");
    if (value.length() > 0)
    {
      Serial.println(value.c_str());
    }
    else
    {
      Serial.println("Empty");
    }
  }
};

void setup()
{
  Serial.begin(115200); // Set the baud rate to 115200
  while (!Serial)
    ; // Wait for serial port to connect
  Serial.println("Starting BLE work!");

  BLEDevice::init("MyESP32BLE"); // Initialize the device with a name for BLE
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->setValue("Hello BLE"); // Set initial value for characteristic
  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  pService->start();                  // Start the service
  pServer->getAdvertising()->start(); // Start advertising

  Serial.println("BLE is ready and waiting for connections.");
}

void loop()
{
  // Nothing to do here, just wait for BLE events
  delay(1000);
  Serial.println("in loop...");
}
