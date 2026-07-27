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
FIND, FILTER, DISPLAY, ALLOF

FIND {key}
FIND {key, subkey, sub-subkey, etc}

FILTER {regex}
FILTER {key, regex}
FILTER {key, subkey, regex}

DISPLAY {key}

DISPLAYTYPE {key}

ALLOF {key}


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