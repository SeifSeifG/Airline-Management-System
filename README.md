# Airline Reservation and Management System

## Layout

```
include/domain/       Entity headers (Person/User/CrewMember hierarchies, Flight, etc.)
include/persistence/  Repository interfaces + JSON/CSV implementations (not started)
include/services/     Business logic: Auth, Flight, Booking, CheckIn, Maintenance, Reporting (not started)
include/ui/           Per-role console menus (not started)
src/...               Matches include/ layout, one .cpp per header
src/app/main.cpp      Entry point -- currently a domain-layer smoke test only
data/                 JSON/CSV data files at runtime
```

## Status

Domain layer only (Day 1 of the suggested timeline): `Person`, `User` and its
role subclasses, `CrewMember` and its subclasses, `Aircraft`, `Flight`,
`Seat`/`SeatMap`, `Reservation`, `Payment`, `LoyaltyAccount`,
`MaintenanceRecord`. Persistence, services, and UI are stubbed as empty
directories, to be filled in incrementally.

## Build

```
make build   # compiles everything into build/airline_system
make run     # builds, then runs it
make clean   # removes build/
```
