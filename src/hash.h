#ifndef COMPILAPIOU_HASH_H
#define COMPILAPIOU_HASH_H

#include "type.h"

#define HASH_SIZE 100 // nb max de IDENTIFIER stockable par level
#define HASH_NB 50 // nb max de bloc

struct hashmap_item {
    char *key;
    struct Declarator value;
    int next;
};

struct hashmap_item EMPTY_ITEM = {""};

//////Scope
struct Scope {
    struct hashmap_item scope_maps[HASH_NB][HASH_SIZE];
    int current_level; //Pas necessaire
};
////////////

bool hash__key_exists_current(struct Scope *hashmap, char *key);
bool hash__key_exists_all(struct Scope *hashmap, char *key);
struct Declarator hash__get_item(struct Scope *hashmap, char *key);
bool hash__add_item(struct Scope *hashmap, char *key, struct Declarator declarator);
void hash__upper_level(struct Scope *hashmap);
void hash__lower_level(struct Scope *hashmap);
void hash__init(struct Scope *hashmap);


#endif //COMPILAPIOU_HASH_H
