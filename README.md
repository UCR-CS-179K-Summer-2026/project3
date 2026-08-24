# Group 3 - JSON Query Engine
By Laura Canon, Moustafa Soliman, James Mace

A C++20 command-line query engine for searching structured JSON documents.

The project uses a recursive query parser and direct JSON traversal to support:

- `FIND` — check whether requested JSON components exist
- `DISPLAY` — retrieve selected JSON values
- `FILTER` — filter array elements using numeric ranges or regular expressions

The engine also includes GoogleTest-based correctness testing and benchmark utilities for evaluating traversal performance.

## Documentation

Full project documentation:

https://ucr-cs-179k-summer-2026.github.io/project3/

The documentation includes:

- Getting Started
- Features and query examples
- Architecture
- Algorithms
- Testing
- Performance
- Limitations

## Quick Start

### 1. Clone the repository

```bash
git clone https://github.com/UCR-CS-179K-Summer-2026/project3.git
cd project3
````

### 2. Build the project

```bash
cmake -S . -B build
cmake --build build
```

### 3. Run the query engine

```bash
./build/json
```

Enter the JSON filename when prompted, then enter a query.

Example:

```text
DISPLAY {"users" {1 {"name"}}}
```

Example result:

```text
"Marco Ruiz"
```

## Running Tests

Run the GoogleTest suite directly:

```bash
./build/JSONQL
```

Or run the tests through CTest:

```bash
cd build
ctest --output-on-failure
```

Current test status:

```text
96 tests passed
0 tests failed
```

## Running Benchmarks

```bash
./build/benchmark
```

The benchmark executable includes small-file, controlled traversal, and experimental large-file benchmark modes.

See the documentation website for benchmark methodology and current results.

## System Requirements

* C++20-compatible compiler
* CMake
* Git
* Unix-like environment recommended for the memory-mapped large-file path

The large-file implementation uses POSIX APIs such as `mmap()`, so that feature may require changes on non-POSIX systems.

## Project Structure

```text
include/       Public headers
src/           Query and JSON parser implementation
tests/         GoogleTest test suite
jsonFiles/     JSON fixtures and benchmark data
main.cpp       Command-line application
benchmark.cpp  Performance benchmarks
```
