# Function and Process Map

## Presentation

### DisplayHelper table and card rendering
- **Source / symbols:** `src/lib/shared/DisplayHelper.h`: `printBorder`, `printRow`, `printHeader`, `printCard`, `printTable`, `formatCurrency`; internal `detail::widthFor` and `detail::isRightAligned`.
- **Canonical owner:** `DisplayHelper` namespace; model `display*` methods and menu controllers supply presentation data.
- **Activity class:** Presentation adapter; synchronous active CLI request output.
- **Trigger / lifecycle:** Called by model display methods and menu actions; no background lifecycle, cancellation, retry, or persistent process.
- **Inputs / outputs / errors:** Vectors of widths, cells, alignment flags, fields, and stream targets; writes ASCII tables/cards to the supplied stream (default `std::cout`) and returns formatted currency strings. No exceptions are introduced by the helper's own control flow.
- **Side effects:** Stream output only; no file, network, database, process, timer, lock, or credential side effects.
- **State / compatibility:** No mutable shared state. Width fallback remains `cell.size() + 2`; non-positive border widths remain one dash; right alignment remains opt-in. `Entity::displayHeader`, `displayRow`, and `displayDetail` now accept optional streams while preserving zero-argument `std::cout` calls.
- **Consumers / characterization:** All model `displayHeader`, `displayRow`, and `displayDetail` implementations plus menu `printTable` calls. `tests/test_runner.cpp:testDisplayHelper` verifies borders, alignment, cards, model output, and `printTable` forwarding into `std::ostringstream`.
- **Refactor decision:** Keep one presentation owner. Extracted repeated width/alignment decisions into private namespace helpers and made stream ownership explicit through the existing entity contract; no second rendering path was introduced.
- **Comment convention:** Production function definitions and inline public functions carry a direct lowercase Vietnamese purpose comment with no space after `//` and no spaces after commas/slashes,matching the repository's established style. Lambdas and control statements are intentionally not documented as named functions.
