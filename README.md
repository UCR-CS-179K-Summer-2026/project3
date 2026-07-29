# Group 3, Project 3 Plan.
Laura Canon, Moustafa Soliman, James Mace


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
```text
FIND, FILTER, DISPLAY, ALLOF

FIND {"key"} 
FIND {"key", "subkey", "sub-subkey"}

returns boolean indicating if key exists or not

EX) 
Query: FIND {"employees", "0", "name"}
Result: true


FILTER {"regex"}
FILTER {"key", "regex"}
FILTER {{key, subkey, sub-subkey, etc}, regex}
FILTER {key (number), lower-bound, upper-bound}
FILTER {{key, subkey, sub-subkey, etc} (number), lower-bound, upper-bound}

returns the values that match the keys and filters

EX) 
Query: FILTER {"employees", "salary", "80000,100000"}
Result: ["Laura", "James"]


DISPLAY {"key"}
DISPLAY {"key", "subkey", "sub-subkey", ...}

EX)
Query {"employees", "0", "name"}
result: "Laura"

ALLOF {"key"}
ALLOF {"key", "subkey", "sub-subkey", ...}

EX)
Query: ALLOF {"employees", "name"}
Result: [ "Laura", "Moustafa", "James" ]
```
