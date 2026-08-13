# 4-Sprint Plan:

## Sprint 1 Plan:

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

## Sprint 2:

### Laura — Testing and Architecture

* Organized and improved the project testing system.
* Added more tests for JSON parsing, query parsing, arrays, nested data, and edge cases.
* Helped organize the overall project structure.
* Improved how the JSON parser, query parser, and query reader work together.
* Made sure Sprint 1 features still worked after the new changes.

### James — Unicode Support and Optimization

* Added Unicode support to the JSON parser.
* Added support for Unicode escape sequences and special characters.
* Improved how Unicode keys are handled in queries.
* Optimized the JSON parser to reduce unnecessary work.
* Improved object and array searching.
* Added performance tests for different types of JSON data.
* Made the JSON parser about 4× faster than the original version**.

### Moustafa — Query Reader and Query Constructor

* Created the query reader.
* Created the `QueryConstruct` structure for storing queries.
* Added support for `FIND`, `FILTER`, `DISPLAY`, and `ALLOF`.
* Converted parsed queries into structured commands, paths, and parameters.
* Added support for different types of query parameters.
* Helped separate query parsing from query execution.
* Prepared the query system to work with the JSON parser.


## Sprint 3:
Error handling

Examples: Invalid data, empty keys, etc


## Sprint 4:
Optimization.
