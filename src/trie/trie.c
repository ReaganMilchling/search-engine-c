#include <stdio.h>
#include <stdlib.h>
#include "trie.h"

TrieNode* create_trienode(char data)
{
    TrieNode* node = (struct TrieNode*) malloc(sizeof(TrieNode));
    for (uint i = 0; i < N_SIZE; ++i)
    {
        node->children[i] = NULL;
    }
    node->is_leaf = 0;
    node->data = &data;
    return node;
}

void free_trienode(TrieNode *node)
{
   for (uint i = 0; i < N_SIZE; ++i)
   {
        if (node->children[i] != NULL)
        {
            free_trienode(node->children[i]);
        }
   }
   free(node);
}

TrieNode* insert_trie(TrieNode* root, char* word)
{
    TrieNode *temp = root;

    for (uint i = 0; i < N_SIZE; ++i)
    {
        uint index = (int) word[i] - 'a';
        if (temp->children[i] == NULL)
        {
            temp->children[index] = create_trienode(word[i]);
        }
        temp = temp->children[index];
    }
    temp->is_leaf = 0;
    return root;
}

int search_trie(TrieNode* root, char* word)
{
    TrieNode* temp = root;

    for (uint i = 0; word[i]!='\0'; ++i)
    {
        uint pos = word[i] - 'a';
        if (temp->children[pos] == NULL)
        {
            return 0;
        }
        temp = temp->children[pos];
    }
    if (temp != NULL && temp->is_leaf == 1)
    {
        return 1;
    }
    return 0;
}

void print_trie(TrieNode* root)
{
    if (!root) { return; }

    TrieNode* temp = root;
    printf("%s ->", temp->data);
    for (uint i = 0; i < N_SIZE; ++i)
    {
        print_trie(temp->children[i]);
    }
}




