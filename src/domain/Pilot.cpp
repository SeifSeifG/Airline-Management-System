#include "domain/Pilot.hpp"

namespace airline {

// TODO: confirm against the actual regulatory limit you're modeling
// (e.g. FAA Part 117 / EASA FTL); placeholder for now.
float Pilot::maxFlightHours() const { return 100.0f; }

}  // namespace airline
