# Vehicle Service Management System

![Language](https://img.shields.io/badge/Language-C-A8B9CC?logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey)
![License](https://img.shields.io/badge/License-MIT-yellow.svg)
![Build](https://img.shields.io/badge/Build-GCC%20%7C%20Clang%20%7C%20MSVC-green)

> Console-based garage management system in C — role-based auth, mixed text/binary persistence, in-place binary updates, and a 14-day return window. Final project for the C Programming course in an Electrical Engineering degree.

---

## Features

### Authentication & Authorization
- Username / password login with **3 failed-attempt lockout**.
- Three permission levels exposing different menus:
  - **Level 1 — Admin:** full access (employees, customers, sales, returns, inventory).
  - **Level 2 — Employee:** customer and sales operations.
  - **Level 3 — Practitioner:** read-only customer and inventory views.
- Default `admin / 12345678` account bootstrapped on first launch.
- Admins can **block** users by setting their level to `0`.

### Customers & Vehicles
- Add customers together with their vehicle (plate, brand, color, entry/exit dates).
- Lookup a customer by license plate.
- Sort the customers file by registration date (`qsort` with a custom comparator).
- Remove a customer — cascades to their vehicles and sales in one operation.

### Inventory & Sales
- Pre-seeded catalog of 20 common auto-parts (oil, filters, brake pads, tires…).
- In-place binary updates of stock levels (`fseek` + `fwrite`) — no rewriting the whole file.
- Add a sale: validates stock, enforces a **daily purchase limit** per customer, decrements inventory atomically.
- Return a product within a **14-day window** (computed via `mktime` + `difftime`); unit restored to inventory.
- View per-customer sales history.

### Audit Logging
- Every meaningful operation (login, add, remove, error) appended to `log.txt` with a timestamp.

---

## Menu Structure

```
Login
│
├── Admin (Level 1)
│   ├── Add / Block Employee
│   ├── Add / Remove Client
│   ├── Find Client by Plate
│   ├── Sort Clients by Date
│   ├── View Client Sales
│   ├── Check Vehicle Exit Date
│   ├── Add Sale
│   ├── Return Sale
│   ├── Display Inventory
│   └── Logout
│
├── Employee (Level 2)
│   ├── Add / Remove Client
│   ├── Find Client by Plate
│   ├── Add Sale / Return Sale
│   └── Logout
│
└── Practitioner (Level 3)
    ├── Find Client by Plate
    ├── Display Inventory
    └── Logout
```

---

## Project Structure

```
c-garage-management/
├── main.c          # All program logic (single translation unit)
├── Makefile        # GCC / Clang build (Linux / macOS / MinGW)
├── LICENSE         # MIT
├── .gitignore
└── README.md
```

### Data Model

| Struct | Purpose | Persisted to |
|--------|---------|-------------|
| `Employee` | username, password, first name, level | `Employee.txt` |
| `Client` | customer info, total spent, service flag | `clients.txt` |
| `Item` | vehicle: plate, brand, color, entry/exit date | `items.bin` |
| `Inventoryitem` | product name, quantity, price | `inventory.bin` |
| `Sale` | plate, product, price, date | `sales.bin` |

Employees and customers use plain text files (human-editable); vehicles, inventory, and sales use fixed-size binary records for in-place seeks.

---

## Build & Run

### Linux / macOS / MinGW (GCC or Clang)

```bash
make          # produces ./garage  (or garage.exe on MinGW)
make run      # builds and runs
make clean    # removes the binary and runtime data files
```

### Windows (MSVC — Developer Command Prompt)

```cmd
cl /W4 /Fe:garage.exe main.c
garage.exe
```

First run creates `Employee.txt` with the default admin and seeds `inventory.bin` with the catalog.

---

## Sample Session

```
=====================================
  Welcome to the Vehicle System!
=====================================

Employee file not found. Creating default admin...
Inventory file not found. Initializing inventory database...

=== Login ===
Please enter username: admin
Please enter password: ********
Hi maneger!

=== Admin Menu (Level 1) ===
 1. Add Employee        6. Sort Clients by Date
 2. Block Employee      7. View Client Sales
 3. Add Client          8. Check Vehicle Exit Date
 4. Remove Client       9. Add Sale
 5. Find Client         10. Return Sale
                        11. Display Inventory
                         0. Logout

Enter your choice: 11

Available products:
------------------------------------------------------
| Product Name              | Quantity   | Price      |
------------------------------------------------------
| oil change                | 10         | 50.00      |
| air filter                | 15         | 30.00      |
| brake pads                | 8          | 120.00     |
...
```

---

## Engineering Highlights

**In-place binary updates** — `UpdateInventory` uses `fseek(file, -(long)sizeof(Inventoryitem), SEEK_CUR)` to rewrite a single record without touching the rest of the file. Same pattern used for sale records.

**Safe rewrite for text files** — Removals write to a `temp_*.txt`, then atomically `remove` + `rename` over the original. A crash mid-write never leaves data in a half-written state.

**`qsort` with a custom date comparator** — `SortClientsByDate` parses `DD-MM-YYYY` into a sortable integer (`year*10000 + month*100 + day`) and feeds it to `qsort`.

**14-day return window via `mktime` / `difftime`** — Sale and return dates are parsed into `struct tm`, converted to `time_t`, and diffed. No manual day arithmetic.

**Defensive I/O** — Every `fopen` is checked, every error path closes already-opened handles, and `scanf` width specifiers (`%19s`, `%9s`) prevent buffer overruns on user input.

**Clean compile under `/W4` and `-Wall -Wextra`** — No warnings on MSVC or GCC.

---

## Skills Demonstrated

| Area | Detail |
|------|--------|
| File I/O | Mixed text + binary persistence, `fseek`/`fwrite` in-place updates |
| Memory management | Stack-allocated structs, no heap leaks |
| Algorithms | `qsort` with custom comparator, date arithmetic |
| Security | Role-based access control, login lockout, input width guards |
| Robustness | Atomic file rewrite, graceful error handling on every syscall |
| Portability | Compiles clean on GCC, Clang, and MSVC with strict warnings |

---

## Future Improvements

- Replace flat files with **SQLite** for relational queries and crash safety.
- Add a **network layer** (socket-based CLI client / REST API) for multi-terminal use.
- Extract translation units — split `main.c` into `auth.c`, `inventory.c`, `sales.c`, etc.
- Add a **test suite** (Unity framework) covering boundary cases on the return window and lockout logic.

---

## License

MIT — see [LICENSE](LICENSE).

---

## Author

**Guy Ifergan** — Electrical Engineering student  
GitHub: [@guyifergan1](https://github.com/guyifergan1)