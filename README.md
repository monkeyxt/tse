# A Tiny Search Engine

A tiny search engine. Includes a concurrent crawler, and indexer and a querier.

## Requirements
`libcurl4-gnutls-dev`

## Build
```
cd utils && make
make all
```
The executables will be in the `build` folder.

## Concurrent Crawler
The concurrent crawler crawls pages from a user specified URL.
```
usage: crawler <seedurl> <pagedir> <maxdepth>

seedurl: the seed url
pagedir: where to store the crawled HTML pages
maxdepth: maximum depth to crawl to

example:
./crawler "https://thayer.github.io/engs50/" "../pages/" 2 3
```

## Indexer
The indexer reads the crawled pages and associates keywords with pages
```
usage: indexer <pagedir> <indexnm>

pagedir: where the crawler stored the HTML pages
indexnm: name of the output index file

examples:
./indexer ../pages index.file 
```

## Querier
The querier takes the index file and queries user searches.
```
usage: query <pagedir> <indexnm> [-q]

pagedir: where the crawler stored the HTML pages
indexnm: index.file location

-q: quiet mode, reads a series of queries from file

examples: 
 ./querier ../pages index.file
 ./querier ../pages index.file -q good-queries.txt ranking
 ./querier ../pages index.file -q bad-queries ranking
```