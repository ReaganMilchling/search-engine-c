# search-engine-c
A Naive TF-IDF search engine in C

Made with neovim

# notes
- There's an extra makefile in src that I use for quicker dev turnaround: run ```make all``` from /src
- Trie is not being used

# todo
- add proper data structure for postings file
- implement searching the postings file
- setup output directories properly
- cli stuff instead of hardcode

# cmake build
First time build/run from main dir
```
mkdir build && cd build
cmake ..
make
```
Second time from main dir
```
cmake --build build
./build/search
```
