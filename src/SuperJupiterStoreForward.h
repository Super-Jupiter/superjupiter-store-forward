// SPDX-License-Identifier: MIT
// Copyright (c) 2026 SuperJupiter
#ifndef SUPERJUPITER_STORE_FORWARD_H
#define SUPERJUPITER_STORE_FORWARD_H
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "SuperJupiter requires IEEE non-finite checks; disable fast-math"
#endif
#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#define SUPERJUPITER_STORE_FORWARD_VERSION "0.1.0"
namespace superjupiter {
namespace store_forward {

namespace detail {
inline bool finite(double value) { return value == value && value >= -DBL_MAX && value <= DBL_MAX; }
inline double oneMinusExpNegative(double ratio) {
  // Stable at small elapsed/tau without requiring AVR's unavailable expm1.
  if (ratio < 0.01) {
    double term = ratio, total = ratio;
    for (unsigned n = 2; n <= 12; ++n) {
      term *= -ratio / n;
      total += term;
    }
    return total;
  }
  return 1 - ::exp(-ratio);
}
} // namespace detail

enum class Status { Ok, Full, InvalidId, Empty, Busy, Waiting, WrongId, InvalidTime };
template <typename Record, size_t Capacity> class StoreForward {
  static_assert(Capacity > 0 && Capacity <= 65535, "StoreForward requires capacity 1..65535");
  struct Entry {
    uint64_t id;
    Record value;
  };
  Entry entries_[Capacity];
  size_t head_, count_;
  uint64_t lastId_;
  bool reserved_, waiting_;
  uint32_t failedAt_, delay_;

public:
  StoreForward()
      : head_(0), count_(0), lastId_(0), reserved_(false), waiting_(false), failedAt_(0),
        delay_(0) {}
  size_t size() const { return count_; }
  Status enqueue(uint64_t id, const Record &value) {
    if (!id || id <= lastId_)
      return Status::InvalidId;
    if (count_ == Capacity)
      return Status::Full;
    entries_[(head_ + count_) % Capacity] = Entry{id, value};
    ++count_;
    lastId_ = id;
    return Status::Ok;
  }
  Status reserve(uint32_t nowMs, uint64_t &id, Record &out) {
    if (reserved_)
      return Status::Busy;
    if (!count_)
      return Status::Empty;
    if (waiting_) {
      const uint32_t dt = nowMs - failedAt_;
      if (dt >= 0x80000000UL)
        return Status::InvalidTime;
      if (dt < delay_)
        return Status::Waiting;
    }
    id = entries_[head_].id;
    out = entries_[head_].value;
    reserved_ = true;
    waiting_ = false;
    return Status::Ok;
  }
  Status acknowledge(uint64_t id) {
    if (!reserved_ || entries_[head_].id != id)
      return Status::WrongId;
    head_ = (head_ + 1) % Capacity;
    --count_;
    reserved_ = false;
    return Status::Ok;
  }
  Status fail(uint64_t id, uint32_t nowMs, uint32_t retryAfterMs) {
    if (!reserved_ || entries_[head_].id != id)
      return Status::WrongId;
    if (retryAfterMs >= 0x80000000UL)
      return Status::InvalidTime;
    reserved_ = false;
    waiting_ = true;
    failedAt_ = nowMs;
    delay_ = retryAfterMs;
    return Status::Ok;
  }
};
} // namespace store_forward
} // namespace superjupiter
#endif
