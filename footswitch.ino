#include <ArduinoBLE.h>

BLEService midiService("03B80E5A-EDE8-4B33-A751-6CE34EC4C700");
BLECharacteristic midiChar("7772E5DB-3868-4112-A1A9-F2669D106BF3",
                            BLERead | BLEWrite | BLEWriteWithoutResponse | BLENotify,
                            20);

const uint8_t NUM_SWITCHES = 3;
const uint8_t SWITCH_PINS[NUM_SWITCHES] = {2, 3, 4};
const uint8_t CC_NUMBERS[NUM_SWITCHES]  = {20, 21, 22}; // unique CC per switch

const unsigned long DEBOUNCE_MS  = 20;
const unsigned long KEEPALIVE_MS = 1000; // macOS BLE MIDI drops idle links; send Active Sense as a heartbeat

uint8_t lastStable[NUM_SWITCHES]         = {HIGH, HIGH, HIGH};
uint8_t lastReading[NUM_SWITCHES]        = {HIGH, HIGH, HIGH};
unsigned long lastChangeMs[NUM_SWITCHES] = {0, 0, 0};
unsigned long lastTxMs                   = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  for (uint8_t i = 0; i < NUM_SWITCHES; i++) {
    pinMode(SWITCH_PINS[i], INPUT_PULLUP);
  }

  if (!BLE.begin()) {
    Serial.println("BLE init failed");
    digitalWrite(LED_BUILTIN, HIGH);
    while (1);
  }

  BLE.setLocalName("Footswitch");
  BLE.setDeviceName("Footswitch");
  BLE.setAdvertisedService(midiService);
  midiService.addCharacteristic(midiChar);
  BLE.addService(midiService);
  BLE.advertise();

  Serial.println("BLE MIDI advertising");
}

void writeMidi(const uint8_t* payload, uint8_t plen) {
  uint16_t ts = millis() & 0x1FFF;
  uint8_t buf[20];
  buf[0] = 0x80 | ((ts >> 7) & 0x3F);
  buf[1] = 0x80 | (ts & 0x7F);
  for (uint8_t i = 0; i < plen; i++) buf[2 + i] = payload[i];
  midiChar.writeValue(buf, 2 + plen);
  lastTxMs = millis();
}

void sendCC(uint8_t cc, uint8_t value) {
  uint8_t p[3] = { 0xB0, cc, value }; // Control Change, channel 1
  writeMidi(p, 3);
}

void sendActiveSense() {
  uint8_t p[1] = { 0xFE }; // silent keepalive
  writeMidi(p, 1);
}

void loop() {
  BLE.poll();

  BLEDevice central = BLE.central();
  digitalWrite(LED_BUILTIN, central ? HIGH : LOW);

  unsigned long now = millis();

  for (uint8_t i = 0; i < NUM_SWITCHES; i++) {
    uint8_t reading = digitalRead(SWITCH_PINS[i]);

    if (reading != lastReading[i]) {
      lastChangeMs[i] = now;
      lastReading[i] = reading;
    }

    if ((now - lastChangeMs[i]) >= DEBOUNCE_MS && reading != lastStable[i]) {
      lastStable[i] = reading;
      uint8_t value = (reading == LOW) ? 127 : 0;
      Serial.print("SW");
      Serial.print(i + 1);
      Serial.print(" CC");
      Serial.print(CC_NUMBERS[i]);
      Serial.print("=");
      Serial.println(value);
      if (central) sendCC(CC_NUMBERS[i], value);
    }
  }

  if (central && (now - lastTxMs >= KEEPALIVE_MS)) {
    sendActiveSense();
  }
}
