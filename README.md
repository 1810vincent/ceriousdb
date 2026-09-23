# CeriousDB

An ultra lightweight, modular, and high-performance persistent key-value database engine written from scratch in C99 with zero dependencies.

> *Inspired by and initially created as a C reimplementation of @danieldeer's [seriousdb](https://github.com/danieldeer/seriousdb)*

## Quick Start

### 1. Clone the repository
```bash
git clone https://github.com/1810vincent/ceriousdb.git
```

### 2. Build the project
Compile both development and release targets:
```bash
cd ceriousdb
make build
```

Alternatively, build specific profiles:
* **All** (compile both development and release targets, as well as test targets, and run the test suite):
```bash
make all
```

* **Development** (with AddressSanitizer, UndefinedBehaviorSanitizer, and debug symbols):
```bash
make dev
```

* **Release** (optimized with `-O2`, stripping, and Link-Time Optimization):
```bash
make release
```

* **Tests** (compile and run all tests (unit tests)):
```bash
make test
```

### 3. Configure the environment
Copy the environment template and adjust settings locally:
```bash
cp .env.example .env
```

### 4. Run the server
Start the development or release server (optionally overriding host and port):
```bash
bin/dev/dev-server --host 127.0.0.1 --port 8080
```

*Access the server via the configured address and path (default: `http://127.0.0.1:8080/db`).*

---

## Configuration

Configuration is handled via environment variables managed by the built-in `.env` module.

| Variable | Default | Description |
| --- | --- | --- |
| `CERIOUSDB_DB_FILE` | `.cdb` | Path to the on-disk database file. |
| `LOG_LEVEL` | `INFO` | Logging level printed to the server terminal. |
| `CERIOUSDB_HOST` | `127.0.0.1` | Server binding host address. |
| `CERIOUSDB_PORT` | `8080` | Server binding port number. |

---

## Architecture

CeriousDB is structured into modular C99 components, drawing conceptual parallels from clean Python architectures:

* **`ceriousdb` (Core & Main):** Core DB server application logic (`main.c`), responsible for initialization and database operations.
* **`ceriousapi`:** Custom internal web framework providing routing and core utilities.
* **`cache`:** In-memory caching layer for high-speed record retrieval.
* **`config`:** Manages runtime options and database file bindings.
* **`dotenv`:** Lightweight environment variable parser supporting local `.env` loading.
* **`dictionary`:** Custom implementation of a high-performance hash dictionary (utilizing `xxhash`), optimized for string only keys and values.
* **`interface`:** Custom interactive command-line interface (CLI) for managing the running server instance.

---

## Documentation

* [Changelog](CHANGELOG.md)

---

## Current State and Vision

### Current State
* Stable and running server core
* Automatic path routing is currently pending implementation
* Data persistence layer is under active development

### Vision
* Full core functionality matching the project specifications
* Lightweight `CeriousDB-CLI` tool for efficient client-side interaction
* Custom low-overhead protocol besides HTTP for faster, more efficient data transport (used in CeriousDB-CLI)

---

## AI Disclaimer
Certain modules and utility functions in this project were generated or assisted by AI (Google Gemini 3.5):
* **`README.md`**
* **`Makefile`**
* **`interface.c`**: `get_uptime_fstring()`
* **`logging.c`**: `get_timestamp_fstring()`
* **`dotenv.c`**: `load_dotenv_f()`
* **`str_dictionary.c`**: several functionalities
