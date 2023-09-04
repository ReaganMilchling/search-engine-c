#ifndef TRIE_H
#define TRIE_H

#define N_SIZE 26

typedef struct TrieNode TrieNode;

struct TrieNode {
    char *data;
    TrieNode* children[N_SIZE];
    int is_leaf;
};

TrieNode* create_trienode(char data);

#endif /* TRIE_H */
