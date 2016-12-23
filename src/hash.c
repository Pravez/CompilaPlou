#include "hash.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

int hachage(char *s) {
    unsigned int hash = 0;
    while (*s != '\0') hash = hash * 31 + *s++;
    return hash % HASH_SIZE;
}

bool hash__key_exists_current(struct Scope *hashmap, char *key) {
    int position = hachage(key);
    do{
        if (strcmp(hashmap->scope_maps[hashmap->current_level][position].key, key) == 0) {
            return true;
        }
        position = hashmap->scope_maps[hashmap->current_level][position].next;
    }while(position != -1);

    return false;
}

bool hash__key_exists_all(struct Scope *hashmap, char *key) {
    for (int i = 0; i <= hashmap->current_level; i++) {
        int position = hachage(key);
        do{
            if (strcmp(hashmap->scope_maps[i][position].key, key) == 0) {
                return true;
            }
            position = hashmap->scope_maps[i][position].next;
        }while(position != -1);
    }

    return false;
}

struct Declarator hash__get_item(struct Scope *hashmap, char *key) {
    for (int i = 0; i <= hashmap->current_level; i++) {
        int position = hachage(key);
        do{
            if (strcmp(hashmap->scope_maps[i][position].key, key) == 0) {
                return hashmap->scope_maps[i][position].value;
            }
            position = hashmap->scope_maps[i][position].next;
        }while(position != -1);
    }

    struct Declarator empty_decl;
    return empty_decl;
}


bool hash__add_item(struct Scope *hashmap, char *key, struct Declarator declarator) {
    if (!hash__key_exists_all(hashmap, key)) {
        int position = hachage(key);
        struct hashmap_item *item = &hashmap->scope_maps[hashmap->current_level][position];
        int incr = 0;
        while (item->key != "") {
            if (incr++ == 100)
                break;
            position = position == HASH_SIZE - 1 ? 0 : position + 1;
            item->next = position;
            item = &hashmap->scope_maps[hashmap->current_level][position];
        }
        if(incr != 100){
            item->key = key;
            item->value = declarator;

            return true;
        }
    }

    return false;
}

void hash__upper_level(struct Scope *hashmap) {
    hashmap->current_level++;
    for (int i = 0; i < HASH_SIZE; i++) {
        hashmap->scope_maps[hashmap->current_level][i] = EMPTY_ITEM;
        hashmap->scope_maps[hashmap->current_level][i].next = -1;
    }
}

void hash__lower_level(struct Scope *hashmap) {
    hashmap->current_level--;
}

void hash__init(struct Scope *hashmap) {
    hashmap->current_level = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        hashmap->scope_maps[hashmap->current_level][i] = EMPTY_ITEM;
        hashmap->scope_maps[hashmap->current_level][i].next = -1;
    }
}
