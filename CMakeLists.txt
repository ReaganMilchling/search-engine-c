cmake_minimum_required(VERSION 3.27)

project(search_engine_c
    VERSION 0.1
    DESCRIPTION "Naive TF-IDF search engine"
    LANGUAGES C
)


add_executable(search 
    src/search.c
    src/objects.c
    src/arraylist/arraylist.c
    src/hashmap/hashmap.c
    src/postings/postings.c
    src/dictionary/dictionary.c
    src/trie/trie.c
    resources/stoplist.txt
    )

target_link_libraries(search m)
