# SuperJupiter StoreForward API

Include `SuperJupiterStoreForward.h`. All public types/functions are inside `superjupiter::store_forward`.

## Behaviour and limits

Queue holds copies in RAM; persistence is not implied. Compose with RecordJournal for durable application recovery.
Caller supplies strictly increasing nonzero uint64 record IDs and a payload type with ordinary copy semantics.
reserve returns only the head record. A failed attempt releases it with retryAfterMs; acknowledgement removes only the reserved matching ID.
Retry delays and all clock comparisons must stay below 2^31 ms. A timeout cannot prove server failure.
Network calls, serialisation and authentication are outside this core. Delivery is at least once if the application retries; receivers must deduplicate IDs.
No automatic retry of an uncertain non-idempotent operation. Single task only or caller synchronisation.

## Minimal complete usage

```cpp
#include <SuperJupiterStoreForward.h>
using namespace superjupiter::store_forward;
int main() {
StoreForward<double,8> queue;
queue.enqueue(1,20.5);
uint64_t id=0;double reading=0;
queue.reserve(0,id,reading);
// Call acknowledge(id) only after the application proves server acceptance.
return 0;
}

```

## Public declarations

The [public header](https://github.com/Super-Jupiter/superjupiter-store-forward/blob/v0.1.0/src/SuperJupiterStoreForward.h) is the complete API; constructors, result fields and status enums are visible there.
All time arguments are caller supplied. No method is thread-safe without caller synchronisation.
Non-success result fields must only be used where this contract explicitly defines them.
Memory is bounded by template capacities and local arrays; storage/network adapter implementations may have their own costs.
