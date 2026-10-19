namespace superjupiter {
struct Config {};
struct Result {};
namespace unrelated {
struct State {};
} // namespace unrelated
} // namespace superjupiter
#include <SuperJupiterStoreForward.h>
using namespace superjupiter::store_forward;
int main() {
  StoreForward<double, 8> queue;
  queue.enqueue(1, 20.5);
  uint64_t id = 0;
  double reading = 0;
  queue.reserve(0, id, reading);
  // Call acknowledge(id) only after the application proves server acceptance.
  return 0;
}
