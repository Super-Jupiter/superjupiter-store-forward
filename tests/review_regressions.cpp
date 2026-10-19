// SPDX-License-Identifier: MIT
#include <SuperJupiterStoreForward.h>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
static unsigned checks = 0;
#define CHECK(x)                                                               \
  do {                                                                         \
    ++checks;                                                                  \
    if (!(x)) {                                                                \
      std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #x);         \
      std::abort();                                                            \
    }                                                                          \
  } while (false)
static bool close(double a, double b) {
  return std::fabs(a - b) <= 1e-12 * (1 + std::fabs(b));
}
using namespace superjupiter::store_forward;

int main() {
  (void)close;

  StoreForward<int, 3> queue;
  int value = 0;
  uint64_t id = 0;
  for (uint64_t i = 1; i <= 100; ++i) {
    CHECK(queue.enqueue(i, static_cast<int>(i)) == Status::Ok);
    CHECK(queue.reserve(static_cast<uint32_t>(i), id, value) == Status::Ok);
    CHECK(id == i);
    CHECK(queue.acknowledge(id) == Status::Ok);
    CHECK(queue.acknowledge(id) == Status::WrongId);
  }
  CHECK(queue.enqueue(100, 1) == Status::InvalidId);
  CHECK(queue.reserve(0, id, value) == Status::Empty);

  std::printf("PASS SuperJupiterStoreForward review: %u checks\n", checks);
}
