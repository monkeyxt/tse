# Tests for indexer

## Build
Run `make` to build tests. Run `make clean` to clean up.

## Testing
- `indexer_test1`: tests if the indexer prints and normalizes word from a HTML
- `indexer_test2`: tests the hashing function for the indexer
- `indexer_test3`: tests if words are correctly associated with queues
- `indexer_test4`: tests indexer argument for page ids
- `indexer_test5`: tests indexer argument for multiple page ids

## External Modules
`pageio.h` and `indexio.h` along with their test files can both be found in `utils`.