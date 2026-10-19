# SuperJupiter StoreForward

Acknowledged bounded upload queue with explicit retry timing and record identities.

**Version 0.1.1: evolving preview. Hardware validation pending.**

Original MIT-licensed portable C++11 by [SuperJupiter](https://www.superjupiter.net).
No mandatory service, hidden clock reads, network calls or heap allocation in the core.

## What it does

Queue holds copies in RAM; persistence is not implied. Compose with RecordJournal for durable application recovery.
Caller supplies strictly increasing nonzero uint64 record IDs and a payload type with ordinary copy semantics.
reserve returns only the head record. A failed attempt releases it with retryAfterMs; acknowledgement removes only the reserved matching ID.
Retry delays and all clock comparisons must stay below 2^31 ms. A timeout cannot prove server failure.
Network calls, serialisation and authentication are outside this core. Delivery is at least once if the application retries; receivers must deduplicate IDs.
No automatic retry of an uncertain non-idempotent operation. Single task only or caller synchronisation.

## Install

PlatformIO exact release (after registry publication):

```ini
lib_deps = superjupiter/SuperJupiterStoreForward@0.1.1
```

Arduino: use the versioned [GitHub release ZIP](https://github.com/Super-Jupiter/superjupiter-store-forward/releases/tag/v0.1.1).
Library Manager registration and index propagation are separate steps; check the official index before relying on IDE availability.

## Example

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

For Arduino, open `examples/Minimal`. It uses synthetic data only.
Results are deliberately local so applications can inspect outcomes before adding I/O.

## CMake

```cmake
add_subdirectory(path/to/superjupiter-store-forward)
target_link_libraries(your_app PRIVATE SuperJupiter::StoreForward)
```

For an installed package use `find_package(SuperJupiterStoreForward 0.1 CONFIG REQUIRED)`.
Top-level CMake builds tests/examples; a subproject does not enable developer tools by default.

## Documentation and verification

- [API and boundaries](https://github.com/Super-Jupiter/superjupiter-store-forward/blob/v0.1.1/docs/API.md)
- [Tests](https://github.com/Super-Jupiter/superjupiter-store-forward/blob/v0.1.1/tests/test.cpp)
- [Validation status](https://github.com/Super-Jupiter/superjupiter-store-forward/blob/v0.1.1/docs/VALIDATION.md)
- [Shared conventions](https://github.com/Super-Jupiter/superjupiter-store-forward/blob/v0.1.1/docs/CONVENTIONS.md)
- [Migration policy](https://github.com/Super-Jupiter/superjupiter-store-forward/blob/v0.1.1/docs/MIGRATION.md)

```sh
cmake -S . -B build/cmake
cmake --build build/cmake
ctest --test-dir build/cmake --output-on-failure
python3 tools/check_release.py --tag v0.1.1
```

Keep raw observations and their provenance in your application. Derived results are not physical truth.
Do not use fast-math or finite-math-only compiler assumptions. AVR double may have float precision.
Software tests, board compilation and physical validation are reported separately.

## Fits the SuperJupiter measurement portfolio

Complements [SensorQuality](https://github.com/Super-Jupiter/superjupiter-sensor-quality) and
[SampleWindow](https://github.com/Super-Jupiter/superjupiter-sample-window) without a required dependency.
Each component can be used independently. Domain models, customer data and proprietary applications are excluded.

## Support and licence

MIT. Report reproducible bugs through repository issues; private security reports: info@superjupiter.net.
Include version, configuration, inputs, expected and observed behaviour. No credentials or private customer data.


## Compatibility review 0.1.1

This patch includes independent regression tests and a reviewed [interoperability contract](https://github.com/Super-Jupiter/superjupiter-store-forward/blob/v0.1.1/docs/INTEROPERABILITY.md). Hardware validation pending.

- Removed unused duplicated math helpers and unnecessary includes.
- Overflow-safe ring indexing; documented payload construction and persistent ID responsibilities.
