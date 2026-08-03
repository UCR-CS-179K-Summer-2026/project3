# Group 3, JSON Analytics Engine

Laura Canon, Moustafa Soliman, James Mace

### Project Summary

Our software will take in a JSON or JSONL file and a query as input, then return data that matches the query.

The system will support different query operations such as finding paths, filtering values, displaying values, and retrieving all values under a repeated key.

We also plan to use optimization techniques to improve performance when working with large files or complex queries.

### Language

C++

### Example JSON

```json
{
  "employees": [
    {
      "name": "Laura",
      "department": "Product",
      "salary": 90000
    },
    {
      "name": "Moustafa",
      "department": "IT Support",
      "salary": 120000
    },
    {
      "name": "James",
      "department": "Engineering",
      "salary": 90000
    }
  ]
}
```

### Query Formatting Rules

* All JSON keys must be enclosed in double quotes.
* Array indexes are numbers and do not require quotes.
* String values and regular expressions must be enclosed in double quotes.
* Numeric bounds do not require quotes.
* Spaces after commas are optional.
* Spaces and commas inside quoted strings are preserved.

### Query Features

#### FIND

Checks whether a specified path exists.

Syntax:

```text
FIND "key"
FIND "key" "subkey" "sub-subkey"
```

Example:

```text
Query:
FIND "employees" 0 "name"

Result:
true
```

If the path does not exist:

```text
Query:
FIND "employees" 0 "age"

Result:
false
```

#### FILTER

Returns values or records that satisfy a filter.

Regex syntax:

```text
FILTER {"key" "subkey"} "regex"
```

Example:

```text
Query:
FILTER {"employees" "name"} "^L"

Result:
["Laura"]
```

Numeric range syntax:

```text
FILTER {"key" "subkey"} lower-bound upper-bound
```

Example:

```text
Query:
FILTER {"employees" "salary"} 80000 100000

Result:
["Laura", "James"]
```

The numeric range is inclusive.

#### DISPLAY

Returns the value stored at a specific path.

Syntax:

```text
DISPLAY "key"
DISPLAY "key" "subkey" "sub-subkey"
```

Example:

```text
Query:
DISPLAY "employees" 0 "name"

Result:
"Laura"
```

Another example:

```text
Query:
DISPLAY "employees" 1 "department"

Result:
"IT Support"
```

#### ALLOF

Returns all values for a specified key within an array.

Syntax:

```text
ALLOF "key" "subkey"
```

Example:

```text
Query:
ALLOF "employees" "name"

Result:
["Laura", "Moustafa", "James"]
```

Another example:

```text
Query:
ALLOF "employees" "salary"

Result:
[90000, 120000, 90000]
```

### Edge Cases

The query system should handle cases such as:

* Missing keys
* Invalid array indexes
* Empty arrays
* Null values
* Keys containing spaces
* Keys containing commas
* Invalid regular expressions
* Invalid query syntax

Example:

```text
Query:
FIND "employees" 10 "name"

Result:
false
```
