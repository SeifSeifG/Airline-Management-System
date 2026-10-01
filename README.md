# Airline Reservation and Management System

A console-based airline system written in modern C++ (C++17). It simulates flight
scheduling, fleet and crew management, passenger booking with seat selection,
check-in, and reporting, with role-based access for three kinds of users:

| Role | What they can do |
|---|---|
| Administrator | Manage user accounts, flights, aircraft, and crew assignments; generate reports |
| Booking Agent | Search flights and handle reservations on behalf of passengers |
| Passenger | Search flights, book, select seats, check in, and manage their profile and loyalty account |

Data is persisted to JSON files (nlohmann/json). The optional enhancements from the
project brief (GUI, email notifications, advanced analytics, extra security,
multi-threading) are out of scope.

## Demo

Demo video: in linked in probably

## Architecture

The code is split into five layers. Each `include/<layer>/` folder has a matching
`src/<layer>/` folder.

```
include/domain/       Entities: Person -> User -> {Administrator, BookingAgent, Passenger},
                      Person -> CrewMember -> {Pilot, FlightAttendant}, Flight, Aircraft,
                      SeatLayout, MaintenanceRecord, LoyaltyAccount,
                      BookingRequest / CheckInRequest / FinishedRequest, Defs (enums, Date)
include/persistence/  Repository<T> and its concrete repositories, plus Loader and Saver
include/services/     Business logic: Auth, User, FlightManagement, AircraftManagement,
                      Crew, Booking/Check-in, Reporting, PasswordHasher
include/app/          AirlineApplication, the composition root
include/ui/           ConsoleUI, the per-role console menus
src/app/main.cpp      Entry point
data/                 JSON data files used at runtime
```

### How the layers fit together

- **`AirlineApplication`** owns every repository and every service by value and wires
  them together. The UI talks only to this class, never directly to services or
  repositories.
- **Repositories** are built on `Repository<T>`, an `unordered_map` of
  `shared_ptr<T>` keyed by id. `UserRepository<T>` specializes it for the user roles
  and enforces with a `static_assert` that `T` derives from `User`. There are
  concrete repositories for flights, aircraft, crew, passengers, and booking requests.
- **Services** hold references to the repositories they need. For example,
  `FlightManagementService` uses the flight and aircraft repositories plus
  `BookingService`, and `CrewService` uses the pilot and flight attendant
  repositories.
- **Ownership model:** repositories (and the application's request lists) own the
  objects. Domain classes that refer to each other, such as a `Flight` pointing at its
  `Aircraft`, `Pilot`s, and `FlightAttendant`s, or a `BookingRequest` pointing at its
  `Passenger` and `Flight`, hold `weak_ptr`s, so they observe without owning and
  create no ownership cycles.
- **Persistence:** `Loader` and `Saver` read and write the JSON files in `data/` and
  are friends of `AirlineApplication`, so they can fill and read its repositories
  directly.
- **Authentication:** `AuthService` logs users in against the three user
  repositories. Passwords are stored hashed (`PasswordHasher`, using the bundled
  `picosha2` header).

### Crew regulations

A crew member can be assigned to a flight only if they meet the flight's minimum
flight-hours requirement (`CrewRegulations::minFlightHours`).

## Modern C++ features used

- Smart pointers: `shared_ptr` for shared entities, `weak_ptr` for non-owning links,
  `unique_ptr` for exclusively owned parts (for example a passenger's
  `LoyaltyAccount`)
- STL containers: `vector`, `unordered_map`, `deque`, `optional`, `tuple`
- Templates: `Repository<T>`, `UserRepository<T>`, `UserService<T>`
- Scoped enums (`enum class`) for roles, statuses, seat classes, and payment states
- JSON serialization with nlohmann/json

## Class diagrams (UML)

The UML is generated from the source with
[clang-uml](https://github.com/bkryza/clang-uml) and rendered with PlantUML, so it
always matches the code. It is split into five diagrams, one per concern, because a
single diagram of the whole project is unreadable.

| Diagram | Shows |
|---|---|
| `people` | The `Person` hierarchy (users and crew) and `LoyaltyAccount` |
| `flight_booking` | Flight, aircraft, seat layout, booking/check-in requests, and `Passenger` |
| `persistence` | `Repository<T>` and its concrete repositories |
| `services` | The service classes and the repositories they depend on |
| `application` | `AirlineApplication` as composition root, with `ConsoleUI`, `Loader`, and `Saver` |

![People] <img width="1219" height="1326" alt="people" src="https://github.com/user-attachments/assets/5cb6c17e-cfaf-42dc-ab10-b880994168ed" />
![Flight and booking]<img width="1263" height="3100" alt="flight_booking" src="https://github.com/user-attachments/assets/ba1a46f9-0c44-4aa0-a744-f5fb7b99e5d1" />
![Persistence]<img width="2426" height="454" alt="persistence" src="https://github.com/user-attachments/assets/a28db61b-5b66-4a67-9531-976fbe4723dd" />
![Services]<img width="4096" height="1130" alt="services" src="https://github.com/user-attachments/assets/e0aef03d-478d-4108-ae99-9c02adaf11bf" />
![Application] <img width="2715" height="2201" alt="application" src="https://github.com/user-attachments/assets/4a27dde2-9ed7-427c-a0cc-a5c63be61c63" />

### Regenerating the diagrams

Requirements: `clang-uml`, `bear`, `plantuml`.

```bash
bear -- make -B                              # records compile_commands.json
clang-uml                                    # reads .clang-uml, writes diagrams/*.puml
plantuml -tpng diagrams/*.puml               # renders the PNGs
```

The diagram definitions are in `.clang-uml` at the project root. If a PNG comes out
cropped, prefix the last command with `PLANTUML_LIMIT_SIZE=8192`.

## Build and run

Requirements: a C++17 compiler (GCC or Clang), `make`, and
[nlohmann/json](https://github.com/nlohmann/json).

```bash
make build   # compiles everything into build/airline_system
make run     # builds, then runs the application
make clean   # removes build/
```

## Data

Runtime data lives in `data/` as JSON files. `Loader` reads them into the
repositories and `Saver` writes the current state back, including booking and
check-in requests.
