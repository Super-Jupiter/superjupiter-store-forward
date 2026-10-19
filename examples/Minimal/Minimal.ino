#include <Arduino.h>
#include <SuperJupiterStoreForward.h>
using namespace superjupiter::store_forward;
void setup() {
  Serial.begin(115200);
  StoreForward<double, 8> queue;
  queue.enqueue(1, 20.5);
  uint64_t id = 0;
  double reading = 0;
  queue.reserve(0, id, reading);
  // Call acknowledge(id) only after the application proves server acceptance.
  Serial.println("SuperJupiter StoreForward example ran. Hardware validation pending.");
}
void loop() {}
