# Vehicle Service Management System

A console-based management system for an auto-service garage, written in C.
Submitted as the final project for the C Programming course in my Electrical
Engineering degree.

The program manages employees, customers, vehicles, parts inventory and sales
through a role-based menu, with all data persisted to mixed text and binary
files. It supports three permission levels (Admin, Employee, Practitioner),
a daily purchase limit, a 14-day return window, and an append-only audit log.

## Features

### Authentication & Authorization
- Username / password login with **3 failed-attempt lockout**.
- Three permission levels exposing different menus:
  - **Level 1 — Admin:** full access (manage employees, customers, sales, returns, inventory).
  - **Level 2 — Employee:** customer and sales operations.
  - **Level 3 — Practitioner:** read-only customer and inventory views.
- A default `admin / 12345678` account is bootstrapped on first launch.
- Admins can **block** users by setting their level to `0`, which denies future logins.

### Customers & Vehicles
- Add customers together with their vehicle (plate, brand, color, entry/exit dates).
- Lookup a customer and their vehicle by license plate.
- Sort the customer file by registration date (`qsort` with a custom comparator).
- Remove a customer and cascade-remove their vehicles and sales.

### Inventory & Sales
- Pre-seeded inventory of 20 common auto-parts (oil, filters, brake pads, tires…).
- In-place binary updates of stock levels (`fseek` + `fwrite`) — no rewriting the whole file.
- Add a sale: validates stock, enforces a **daily purchase limit** per customer,
  decrements inventory and updates the customer's total spent atomically.
- Return a product within a **14-day window** (computed via `mktime` + `difftime`);
  the unit is restored to inventory.
- View per-customer sales history.

### Logging
- Every meaningful operation (login, add, remove, error) is appended to `log.txt`
  with a timestamp, giving a simple audit trail.

## Project Structure

```
c-garage-management/
├── main.c          # All program logic (single translation unit)
├── Makefile        # GCC/Clang build (Linux/macOS/MinGW)
├── LICENSE         # MIT
├── .gitignore
└── README.md
```

### Data model (in-memory `struct`s)

| Struct          | Purpose                                       | Persisted to       |
|-----------------|-----------------------------------------------|--------------------|
| `Employee`      | username, password, first name, level         | `Employee.txt`     |
| `Client`        | customer info + total spent + service flag    | `clients.txt`      |
| `Item`          | vehicle: plate, brand, color, entry/exit date | `items.bin`        |
| `Inventoryitem` | product name, quantity, price                 | `inventory.bin`    |
| `Sale`          | plate, product, price, date                   | `sales.bin`        |

Mixing text and binary formats was a deliberate choice: human-editable employee
and customer rosters in plain text, fixed-size records in binary so I could
seek and update them in place.

## Build & Run

### Linux / macOS / MinGW (GCC or Clang)

```bash
make          # produces ./garage (or garage.exe on MinGW)
make run      # builds and runs
make clean    # removes the binary and any runtime data files
```

### Windows (MSVC, "Developer Command Prompt for VS")

```cmd
cl /W4 /Fe:garage.exe main.c
garage.exe
```

The first run creates `Employee.txt` with the default admin and seeds
`inventory.bin` with the catalog.

## Sample Session

```
=====================================
  Welcome to the Vehicle System!
=====================================

Employee file not found. Creating default admin...
Inventory file not found. Initializing inventory database...

=== Login ===
Please enter username:
admin
Please enter password:
12345678
Hi maneger!

=== Admin Menu (Level 1) ===
1. Add Employee
2. Block Employee
3. Add Client
4. View Client Sales
5. Check Vehicle Exit Date
6. Return Sale
7. Add Sale
8. Display Inventory
9. Exit
Enter your choice: 8

Available products:
------------------------------------------------------
| Product Name              | Quantity   | Price      |
------------------------------------------------------
| oil change                | 10         | 50.00      |
| air filter                | 15         | 30.00      |
...
```

## Engineering Highlights

A short tour of the things in this project I would point an interviewer at:

- **In-place binary updates.** `UpdateInventory` and the sale path use
  `fseek(file, -(long)sizeof(Inventoryitem), SEEK_CUR)` to rewrite a single
  record without touching the rest of the file.
- **Safe rewrite pattern for text files.** Removals/edits write to a
  `temp_*.txt` and then atomically `remove` + `rename` over the original, so a
  crash mid-write never leaves the data file in a half-written state.
- **`qsort` with a custom comparator.** `SortClientsByDate` parses
  `DD-MM-YYYY` into a sortable integer (`year*1000 + month*100 + day`) and
  feeds the comparator to `qsort`.
- **Date arithmetic via `mktime` / `difftime`** for the 14-day return window.
- **Defensive I/O.** Every `fopen` is checked, every error path closes
  already-opened handles, and `scanf` width specifiers (`%19s`, `%9s`) prevent
  buffer overruns on user input.
- **Clean compile under `cl /W4`** — no warnings.

## What I Learned

- File I/O in C is genuinely subtle — text vs. binary modes, `fseek` semantics,
  and the read/write boundary on `r+` streams all bit me before I got it right.
- Mixing text and binary persistence is fine for a small project but doesn't
  scale; a real version would use SQLite or a single binary catalog with a
  schema header.
- Strict compiler warnings (`/W4` / `-Wall -Wextra`) catch real bugs early
  (e.g. the classic `if (0 < x < 4)` mistake, which always evaluates true in C).

## License

MIT — see [LICENSE](LICENSE).

## Author

**Guy Ifergan** — Electrical Engineering student.
GitHub: [@guyifergan1](https://github.com/guyifergan1)
