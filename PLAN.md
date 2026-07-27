# Group 3, Project 3 Plan.
Laura, Moustafa, James


## What option did you choose?
We chose option 2, the high-performance JSON analytics engine.


## What exactly will your software do?
Our software will take in a JSON file and query path as input, and then return the data that path finds as output. It will utilize different optimization techniques to ensure that it is fast even if the data input is large or query is complex.


## What language(s) will you use?
Rust


## What do you hope to accomplish in the remaining 2 hours?
Learn more about JSON format, JSON parsing and optimization techniques. 

## Queries

JSON example
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
      "salary": 90000
    },
    {
      "name": "James",
      "department": "Engineering",
      "salary": 90000
    }
  ]
}
```
```text
FIND, FILTER, DISPLAY, ALLOF

FIND {key} 
FIND {key, subkey, sub-subkey, etc}

EX) 
Query: FIND {employees, 0, name}
Result: Laura


FILTER {regex}
FILTER {key, regex}
FILTER {{key, subkey, sub-subkey, etc}, regex}
FILTER {key (number), lower-bound, upper-bound}
FILTER {{key, subkey, sub-subkey, etc} (number), lower-bound, upper-bound}

EX) 
Query: FILTER {employees, name, "^L"}
Result: employees with names starting with L

Query: FILTER {{a, b, c, d}, 0, 10}
Result: All numbers in subkey d between 0 and 10 inclusive.

DISPLAY {key}
DISPLAY {key, subkey, sub-subkey, etc}

DISPLAYTYPE {key}
DISPLAYTYPE {key, subkey, sub-subkey, etc}

EX)
Query: DISPLAY {a} 
Result: All data under key ‘a’ EXCLUDING data in subkeys of a. Subkeys will be marked, for subkey b, {subkey b}

ALLOF {key}
ALLOF {key, subkey, sub-subkey, etc}

EX)
Query: ALLOF {employees, name}
Result: [ "Laura", "Moustafa", "James" ]
```

## 4-Sprint Plan:
### Sprint 1:
Basic structure and parsing. 

Examples: Preventing duplicate keys

### Sprint 2:
Complex structure (arrays, etc) and querying.

Examples: Finding all data corresponding to a specific key


### Sprint 3:
Error handling

Examples: Invalid data, empty keys, etc


### Sprint 4:
Optimization.
