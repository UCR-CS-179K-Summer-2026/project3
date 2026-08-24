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
### Example JSONL
```jsonl
{"id": 1, "name": "Alice", "role": "Admin"}
{"id": 2, "name": "Bob", "role": "User"}
{"id": 3, "name": "Charlie", "role": "Moderator"}
```

### Query Formatting Rules

* All JSON keys must be enclosed in double quotes.
* Array indexes are numbers and do not require quotes.
* String values and regular expressions must be enclosed in double quotes.
* Numeric bounds do not require quotes.
* Spaces after commas are optional.
* Spaces and commas inside quoted strings are preserved.
* For JSONL, the same rules apply, except that if you run a FIND or DISPLAY command, it will execute that command on every single entry of the JSONL. If you want to run that command on a specific entry of the JSONL, then you must index it, as seen in the examples below.

### Query Features

#### FIND

Checks whether all specified keys at each level exists.

Keys listed side by side are separate targets at the same level, and all of them must exist. A nested group descends from the key immediately before it.

Syntax:

```text
FIND {"key"}
FIND {"key" "other key"}
FIND {"key" { "subkey" { "sub-subkey" }}}
```

Example (JSON) :

```text
Query:
FIND {"employees" { "0" { "name" }}}

Result:
true
```

If the path does not exist:

```text
Query:
FIND {"employees" { "0" { "age" }}}

Result:
false
```

Example (JSONL) :

To be clear, these examples are running on the JSONL file, not the JSON file.
```text
Query:
FIND {"name}

Result:
true
```

If the path does not exist:

```text
Query:
FIND {"age"}

Result:
false
```

#### FILTER

Returns values or records that satisfy a filter.

Regex syntax:

```text
FILTER {"key" { "subkey" }} "regex"
```

Example:

```text
Query:
FILTER {"employees" { "name" }} "^L"

Result:
[
    {
      "name": "Laura",
      "department": "Product",
      "salary": 90000
    }
]
```

Numeric range syntax:

```text
FILTER {"key" { "subkey" }} lower-bound upper-bound
```

Example:

```text
Query:
FILTER {"employees" { "salary" }} 80000 100000

Result:
[
    {
      "name": "Laura",
      "department": "Product",
      "salary": 90000
    },
    {
      "name": "James",
      "department": "Engineering",
      "salary": 90000
    }
]
```

The numeric range is inclusive.

Example (JSONL) :

TODO !!!

#### DISPLAY

Returns the value stored at a specific path.

Keys listed side by side each name a value to show, and all of them must exist. A key followed by a nested group only names the way down, so its own value is not shown.

Syntax:

```text
DISPLAY {"key"}
DISPLAY {"key" "other key"}
DISPLAY {"key" { "subkey" { "sub-subkey" }}}
```

Example:

```text
Query:
DISPLAY {"employees" { "0" { "name" }}}

Result:
"Laura"
```

Another example:

```text
Query:
DISPLAY {"employees" { "1" { "department" }}}

Result:
"IT Support"
```

Several values at once:

```text
Query:
DISPLAY {"employees" { "0" { "name" "salary" }}}

Result:
["Laura", 90000]
```

Example (JSONL) :

```text
Query:
DISPLAY {"name"}

Result:
[Alice] 
[Bob]
[Charlie]
```

Several values at once:

```text
Query:
DISPLAY {"id" "name"}

Result:
[1, "Alice"]
[2, "Bob"]
[3, "Charlie"]
```

### Unicode Support

Unicode (UTF-16) is fully supported. Unicode is read in as \uXXXX, and converted to the corresponding byte sequence. Surrogates (\uXXXX\uXXXX) are also checked, and converted to their corresponding byte sequence for comparisons.
```
{
    "\u0061": "Bob",
    "\u0062": {
        "\u0063": "Apple",
        "\u0064": "\u0061"
    }
}
``` 
Returns "a" if the query DISPLAY {"b" {"d"}} is given. 

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
```json
{
    "": "Empty key",
    "empty_array": [],
    "empty_object": {}
}
```

```text
Query:
DISPLAY {""}

Result:
"Empty key"
```
