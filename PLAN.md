# 4-Sprint Plan:

## Sprint 1 Plan:

Goal: Set up the project, document the main features, define the query format, and prepare the C++ environment.

### Laura — Documentation, JSON Files, and Testing

- Updated `README.md`, `PLAN.md`, and GitHub Pages.
- Documented the project overview, query format, examples, and edge cases.
- Created `employee.json`, `product.json`, and `university.json`.
- Added new query-parser edge-case tests.
- Added tests for valid, missing, and empty JSON file paths.
- Verified that the project builds and all Sprint 1 tests pass.

### James — JSON Parser

- Implemented the initial JSON parser.
- Added support for receiving JSON data and a query path.
- Traversed JSON data using keys and array indices.
- Added support for nested objects and arrays.
- Returned the value located at the requested path.
- Connected the parser components to the project build.

### Moustafa — Query Parser

- Implemented the initial query parser.
- Converted query strings into `std::vector<JSONTypes>` structures.
- Added support for commands, arguments, and nested `{}` groups.
- Used recursive structures to represent nested queries.
- Added tests for simple, nested, and deeply nested queries.
- Prepared the parsed query format for later JSON lookup.

### Sprint 1 Results

- The C++ project builds successfully.
- The query parser handles simple and nested queries.
- The JSON parser can navigate JSON data using keys and indices.
- Sample JSON files are available for testing and demonstrations.
- Sprint 1 tests pass.
- Project documentation and GitHub Pages are updated.

## Sprint 2:
Complex structure (arrays, etc) and querying.

Examples: Finding all data corresponding to a specific key


## Sprint 3:
Error handling

Examples: Invalid data, empty keys, etc


## Sprint 4:
Optimization.
