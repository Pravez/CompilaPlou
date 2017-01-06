#ifndef COMPILAPIOU_SCOPE_H
#define COMPILAPIOU_SCOPE_H

#include "type.h"
#include "errors.h"
#include "tools.h"

#define HASH_SIZE 100 // nb max de IDENTIFIER stockable par level
#define NB_LEVELS 50 // nb max of nested levels

#define true 1
#define false 0
typedef int bool;

struct hashmap_item {
    char *key;
    struct Declarator value;
    int next;
};

//////Scope
struct Scope {
    struct hashmap_item scope_maps[NB_LEVELS][HASH_SIZE];
    int current_level; //Pas necessaire
    int higher_level;
};
////////////
struct Scope scope;
////////////

/**HASH FUNCTIONS**/
bool hash__key_exists_current(struct Scope *hashmap, char *key);
bool hash__key_exists_all(struct Scope *hashmap, char *key);
struct Declarator hash__get_item(struct Scope *hashmap, char *key);
struct Declarator* hash__get_item_reference(struct Scope* hashmap, char*key);
int hash__item_find_position(struct Scope* hashmap, char *key, int level);
bool hash__add_item(struct Scope *hashmap, char *key, struct Declarator declarator);
bool hash__add_item_function(struct Scope *hashmap, struct Declarator declarator);
bool hash__add_individual_item_function(struct Scope *hashmap, struct Declarator declarator);
bool hash__add_item_extern_function(struct Scope *hashmap, char* key, struct Declarator declarator);
void hash__clean_level(struct Scope *hashmap, int level);
void hash__prepare_upper_level(struct Scope *hashmap);
bool hash__upper_level(struct Scope *hashmap);
void hash__lower_level(struct Scope *hashmap);
void hash__init(struct Scope *hashmap);

/**BONUS**/
bool hash__add_items(struct Scope *hashmap, struct DeclaratorList list);
bool verify_no_function(struct DeclaratorList list);
bool is_declared(struct Scope *scope, char* identifier, enum DECL_TYPE type);
bool is_of_type(struct Scope *scope, char* identifier, enum TYPE type);
bool set_initialized(struct Scope* scope, char* identifier);
bool check_main_exists(struct Scope* scope);

/**For debug**/
void display_scope(struct Scope scope);


#endif //COMPILAPIOU_HASH_H
