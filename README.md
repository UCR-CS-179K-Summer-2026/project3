# Group 3 - JSON Query Engine
By Laura Canon, Moustafa Soliman, James Mace

A C++20 command-line query engine for searching structured JSON and JSONL documents.

The project uses a recursive query parser and direct JSON traversal to support:

- `FIND` — check whether requested JSON components exist
- `DISPLAY` — retrieve selected JSON values
- `FILTER` — filter array elements using numeric ranges or regular expressions
- JSONL support — query all records or select one record with a zero-based index

The engine also includes memory-mapped file loading, optional output-to-file mode, GoogleTest-based correctness testing, and benchmark utilities for evaluating traversal performance.

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

Choose the output mode when prompted, then enter a `.json` or `.jsonl` filename and a query.

JSONL files can be queried across all records, or a numeric first argument can select one zero-based record.

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
136 tests passed
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

The memory-mapped file-loading path uses POSIX APIs such as `mmap()`, so that feature may require changes on non-POSIX systems.

## Project Structure

```text
include/       Public headers
src/           Query and JSON parser implementation
tests/         GoogleTest test suite
jsonFiles/     JSON fixtures and benchmark data
main.cpp       Command-line application
benchmark.cpp  Performance benchmarks
```
