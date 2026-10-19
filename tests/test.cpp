#include <SuperJupiterStoreForward.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <new>
static unsigned long checks = 0;
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    ++checks;                                                                                      \
    if (!(x)) {                                                                                    \
      std::fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x);                                    \
      std::abort();                                                                                \
    }                                                                                              \
  } while (false)
static bool near(double a, double b) { return std::fabs(a - b) <= 1e-9 * (1 + std::fabs(b)); }
using namespace superjupiter::store_forward;
void *operator new(std::size_t) { std::abort(); }
void *operator new[](std::size_t) { std::abort(); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
int main() {
  (void)near;
  StoreForward<int, 2> q;
  int out = 0;
  uint64_t id = 0;
  CHECK(q.reserve(0, id, out) == Status::Empty);
  CHECK(q.enqueue(1, 10) == Status::Ok);
  CHECK(q.enqueue(1, 20) == Status::InvalidId);
  CHECK(q.enqueue(2, 20) == Status::Ok);
  CHECK(q.enqueue(3, 30) == Status::Full);
  CHECK(q.reserve(0, id, out) == Status::Ok && id == 1 && out == 10);
  CHECK(q.reserve(0, id, out) == Status::Busy);
  CHECK(q.acknowledge(2) == Status::WrongId);
  CHECK(q.fail(1, UINT32_MAX - 49, 100) == Status::Ok);
  CHECK(q.reserve(0, id, out) == Status::Waiting);
  CHECK(q.reserve(50, id, out) == Status::Ok && id == 1);
  CHECK(q.acknowledge(1) == Status::Ok);
  CHECK(q.enqueue(3, 30) == Status::Ok);
  CHECK(q.reserve(51, id, out) == Status::Ok && id == 2);
  q.acknowledge(2);
  q.reserve(52, id, out);
  CHECK(id == 3 && out == 30);

  StoreForward<int, 4> stream;
  for (uint64_t i = 1; i <= 1000; ++i) {
    CHECK(stream.enqueue(i, static_cast<int>(i)) == Status::Ok);
    CHECK(stream.reserve(static_cast<uint32_t>(i), id, out) == Status::Ok);
    CHECK(id == i && out == static_cast<int>(i));
    CHECK(stream.acknowledge(id) == Status::Ok);
    CHECK(stream.size() == 0);
  }

  std::printf("PASS SuperJupiterStoreForward: %lu checks\n", checks);
}
