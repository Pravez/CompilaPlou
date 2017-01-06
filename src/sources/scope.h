#ifndef COMPLL_SCOPE_H
#define COMPLL_SCOPE_H

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
bool scope__key_exists_current(struct Scope *hashmap, char *key);
bool scope__key_exists(struct Scope *hashmap, char *key);
struct Declarator scope__get_declarator(struct Scope *hashmap, char *key);
struct Declarator* scope__get_decl_address(struct Scope* hashmap, char*key);
int scope_decl_find_position(struct Scope* hashmap, char *key, int level);
bool scope__add_item(struct Scope *hashmap, char *key, struct Declarator declarator);
bool scope__add_item_function(struct Scope *hashmap, struct Declarator declarator);
bool scope__add_individual_function_item(struct Scope *hashmap, struct Declarator declarator);
bool scope__add_item_extern_function(struct Scope *hashmap, char* key, struct Declarator declarator);
void scope__clean_level(struct Scope *hashmap, int level);
void scope__prepare_next_level(struct Scope *hashmap);
bool scope__next_level(struct Scope *hashmap);
void scope__previous_level(struct Scope *hashmap);
void scope__init(struct Scope *hashmap);

/**BONUS**/
bool scope__add_items(struct Scope *hashmap, struct DeclaratorList list);
bool no_function_in_list(struct DeclaratorList list);
bool is_declared(struct Scope *scope, char* identifier, enum DECL_TYPE type);
bool is_of_type(struct Scope *scope, char* identifier, enum TYPE type);
bool set_initialized(struct Scope* scope, char* identifier);
bool is_main_existing(struct Scope* scope);

/**For debug**/
void display_scope(struct Scope scope);


#endif //COMPLL_HASH_H
