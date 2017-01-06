#include "scope.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "external_function.h"

#define AVG_LOADED_REG 64

#define CHECK_LEVEL_SIZE(hashmap) hashmap->current_level == NB_LEVELS - 1

int hachage(char *s) {
    if(s != NULL) {
        unsigned int hash = 0;
        while (*s != '\0') hash = hash * 31 + *s++;
        return hash % HASH_SIZE;
    }else{
        return 0;
    }
}

/**
 * Function to verifiy if the key exists in the current scope of the hashmap
 * @param hashmap
 * @param key
 * @return
 */
bool hash__key_exists_current(struct Scope *hashmap, char *key) {
    int position = hachage(key);
    do{
        if (strcmp(hashmap->scope_maps[hashmap->current_level][position].key, key) == 0) {
            return true;
        }
        //If we hash function returned an already known position, then we try to follow the next values
        position = hashmap->scope_maps[hashmap->current_level][position].next;
    }while(position != -1);

    return false;
}

/**
 * Function to verify if a key exists in every level of a scope
 * @param hashmap
 * @param key
 * @return
 */
bool hash__key_exists_all(struct Scope *hashmap, char *key) {
    for (int i = hashmap->higher_level; i >= 0; i--) {
        int position = hachage(key);
        do{
            if (strcmp(hashmap->scope_maps[i][position].key, key) == 0) {
                return true;
            }
            //To verify if hash function returned already set value
            position = hashmap->scope_maps[i][position].next;
        }while(position != -1);
    }

    return false;
}

/**
 * Function to get an item according to its key (a bit the same as functions to verify presence)
 * @param hashmap
 * @param key
 * @return
 */
struct Declarator hash__get_item(struct Scope *hashmap, char *key) {
    for (int i = hashmap->current_level; i >= 0; i--) {
        int position = hachage(key);
        do{
            if (strcmp(hashmap->scope_maps[i][position].key, key) == 0) {
                return hashmap->scope_maps[i][position].value;
            }
            position = hashmap->scope_maps[i][position].next;
        }while(position != -1);
    }

    struct Declarator empty_decl;
    empty_decl.decl_type = -1;
    return empty_decl;
}

/**
 * Exactly the same as hash__get_item but returns a pointer
 * @param hashmap
 * @param key
 * @return
 */
struct Declarator* hash__get_item_reference(struct Scope* hashmap, char*key){
    for (int i = hashmap->current_level; i >= 0; i--) {
        int position = hachage(key);
        do{
            if (strcmp(hashmap->scope_maps[i][position].key, key) == 0) {
                return &hashmap->scope_maps[i][position].value;
            }
            position = hashmap->scope_maps[i][position].next;
        }while(position != -1);
    }

    return NULL;
}

/**
 * Function to find an item position in a scope and level
 * @param hashmap
 * @param key
 * @param level
 * @return
 */
int hash__item_find_position(struct Scope* hashmap, char *key, int level){
    //first we get the eventual position according to the hash function
    int position = hachage(key);
    struct hashmap_item *item = &hashmap->scope_maps[level][position];
    int incr = 0;
    //After getting the item, we see if the place is occupied
    //If it is, we link the concerned place with the next one we'll visit (simple position +1 modulo 100)
    while (strcmp(item->key, "") != 0) {
        //If we increased the position 100 times, then there is no place
        if (incr++ == 100)
            break;
        position = position == HASH_SIZE - 1 ? 0 : position + 1;
        item->next = position;
        item = &hashmap->scope_maps[hashmap->current_level][position];
    }
    //If there is no place, it's an error
    if(incr != 100)
        return position;
    else
        return -1;
}

/**
 * Function to add an item function in the hashmap. Tries to add parameters.
 * @param hashmap
 * @param key
 * @param declarator
 * @return
 */
bool hash__add_item_function(struct Scope *hashmap, struct Declarator declarator){
    //Because we cannot declare a function inside a function, we always have 1 unit difference with current_level
    if(hashmap->higher_level == hashmap->current_level) {
        hashmap->higher_level++;
        hash__clean_level(hashmap, hashmap->higher_level); // clean next level
    }

    for(int i=0;i<declarator.declarator.function.var_list_size;i++){
        //Because the upper level is truly new, the probability to have an error is zero ?
        int position = hash__item_find_position(hashmap, declarator.declarator.function.var_list[i].identifier,
                                                hashmap->higher_level);
        if (position == -1) {
            return false;
        } else {
            //Getting the iem
            struct hashmap_item *item = &hashmap->scope_maps[hashmap->higher_level][position];
            item->key = declarator.declarator.function.var_list[i].identifier;
            //Converting the function's variable to a declarator for the new level
            struct Declarator variable;
            variable.decl_type = VARIABLE;
            variable.declarator.variable = declarator.declarator.function.var_list[i];
            item->value = variable;
        }
    }

    return true;
}

bool hash__add_individual_item_function(struct Scope *hashmap, struct Declarator declarator){
    if(!hash__key_exists_current(hashmap, declarator.declarator.variable.identifier)){
        int position = hash__item_find_position(hashmap, declarator.declarator.variable.identifier, hashmap->higher_level);
        if(position == -1){
            return false;
        }else{
            //Getting the iem
            struct hashmap_item *item = &hashmap->scope_maps[hashmap->higher_level][position];
            item->key = declarator.declarator.variable.identifier;
            item->value = declarator;

            return true;
        }
    }else{
        report_error( DEFINED_FUNC_VAR, declarator.declarator.variable.identifier);
        return false;
    }
}

/**
 * Main function to add an item. Finds the place of the declarator and affects values
 * @param hashmap
 * @param key
 * @param declarator
 * @return
 */
bool hash__add_item(struct Scope *hashmap, char *key, struct Declarator declarator) {
    if(CHECK_LEVEL_SIZE(hashmap)){
        report_error(SCOPE_MAX_IDENT, 0);
        return false;
    }

    //If the item is not present in the hashmap
    if (!hash__key_exists_current(hashmap, key)) {
        //We find the position
        int position = hash__item_find_position(hashmap, key, hashmap->current_level);
        if(position != -1){
            struct hashmap_item *item = &hashmap->scope_maps[hashmap->current_level][position];
            //If the item is a function
            if(declarator.decl_type == FUNCTION){
                //If we could add the function
                if(hash__add_item_function(hashmap, declarator)){
                    item->key = key;
                    item->value = declarator;
                }else{
                    return false;
                }
            }else{
                item->key = key;
                item->value = declarator;
            }


            return true;
        }
    }

    return false;
}

bool hash__add_item_extern_function(struct Scope *hashmap, char* key, struct Declarator declarator){
    if(!hash__key_exists_all(hashmap, key)){
        int position = hash__item_find_position(hashmap, key, hashmap->current_level);
        if(position != -1) {
            struct hashmap_item *item = &hashmap->scope_maps[hashmap->current_level][position];
            item->key = key;
            item->value = declarator;

            return true;
        }
    }

    return false;
}

/**
 * Function to clean a level, setting keys and next values to default ones
 * @param hashmap
 * @param level
 */
void hash__clean_level(struct Scope *hashmap, int level){
    for (int i = 0; i < HASH_SIZE; i++) {
        hashmap->scope_maps[level][i].key = "";
        hashmap->scope_maps[level][i].next = -1;
    }
}

/**
 * Add a level. If higher_level is higher is not bigger, we clean next level (because not already set)
 * @param hashmap
 */
bool hash__upper_level(struct Scope *hashmap) {
    if(hashmap->current_level != NB_LEVELS -1) {
        hashmap->current_level++;
        if (hashmap->current_level > hashmap->higher_level) {
            hashmap->higher_level++;
            hash__clean_level(hashmap, hashmap->current_level); //clean next level
        }
    }else{
        report_error(SCOPE_MAX_LEVEL, 0);
        return 0;
    }

    return 1;
}

/**
 * Lower level of a scope
 * @param hashmap
 */
void hash__lower_level(struct Scope *hashmap) {
    if(hashmap->current_level != 0) {
        hashmap->current_level--;
        hashmap->higher_level--;
    }
}

/**
 * Init a scope
 * @param hashmap
 */
void hash__init(struct Scope *hashmap) {
    hashmap->current_level = 0;
    hashmap->higher_level = 0;
    hash__clean_level(hashmap, hashmap->current_level);
}

/**
 * Function to call the adding of an item on a list of Declarator
 * @param hashmap
 * @param list
 * @return
 */
bool hash__add_items(struct Scope *hashmap, struct DeclaratorList list){
    if(verify_no_function(list))
        return false;

    if(CHECK_LEVEL_SIZE(hashmap)){
        report_error(SCOPE_MAX_IDENT, 0);
        return false;
    }

    for(int i=0;i<list.size;i++){
        if(list.declarator_list[i].decl_type == FUNCTION){
            if(!hash__add_item(hashmap, list.declarator_list[i].declarator.function.identifier, list.declarator_list[i])){
                if(error_flag == 0) {
                    if(hash__get_item(hashmap, list.declarator_list[i].declarator.function.identifier).decl_type == FUNCTION)
                        report_error(DEFINED_FUNC, list.declarator_list[i].declarator.function.identifier);
                    else
                        report_error(DEFINED_VAR, list.declarator_list[i].declarator.function.identifier);
                }
                return false;
            }
        }else{
            if(list.declarator_list[i].declarator.variable.type == T_VOID){
                report_error(VOID_UNAUTHORIZED, list.declarator_list[i].declarator.variable.identifier);
                return false;
            }
            if(!hash__add_item(hashmap, list.declarator_list[i].declarator.variable.identifier, list.declarator_list[i])){
                if(hash__get_item(hashmap, list.declarator_list[i].declarator.variable.identifier).decl_type == FUNCTION)
                    report_error(DEFINED_FUNC, list.declarator_list[i].declarator.variable.identifier);
                else
                    report_error(DEFINED_VAR, list.declarator_list[i].declarator.variable.identifier);
                return false;
            }
        }

    }

    return true;
}

bool verify_no_function(struct DeclaratorList list){
    bool f_presence = false;
    for(int i=0;i<list.size;i++) {
        if (list.declarator_list[i].decl_type == FUNCTION) {
            f_presence = true;
            report_error(INVALID_FUNC_DECLARATION, list.declarator_list[i].declarator.function.identifier);
        }
    }

    return f_presence;
}

/**
 * Debug function to display an entire scope
 * @param scope
 */
void display_scope(struct Scope scope){
    for(int i=0;i<=scope.current_level;i++){
        printf("Level %d\n", i);
        for(int j=0;j<HASH_SIZE;j++){
            if(strcmp(scope.scope_maps[i][j].key, "") != 0){
                printf("%s identifier : %s", scope.scope_maps[i][j].value.decl_type == FUNCTION ? "FUNCTION" : "VARIABLE",
                       scope.scope_maps[i][j].key);
                if(scope.scope_maps[i][j].value.decl_type == FUNCTION){
                    printf(" (");
                    for(int k = 0;k<scope.scope_maps[i][j].value.declarator.function.var_list_size;k++){
                        char* type;
                        switch(scope.scope_maps[i][j].value.declarator.function.var_list[k].type){
                            case T_INT: type = "INT";break;
                            case T_DOUBLE: type = "DOUBLE";break;
                            case T_VOID: type = "VOID";break;
                        }
                        printf("%s %s", type, scope.scope_maps[i][j].value.declarator.function.var_list[k].identifier);
                    }

                    printf(")");
                }
                printf("\n");
            }
        }
    }
}

/**
 * Verify if a variable or function has already been declared
 * @param scope
 * @param identifier
 * @param type
 * @return
 */
bool is_declared(struct Scope *scope, char* identifier, enum DECL_TYPE type){
    struct Declarator declarator = hash__get_item(scope, identifier);
    if(declarator.decl_type != -1){
        if(declarator.decl_type == type){
            return true;
        }else{
            if(type == VARIABLE){
                report_error(FUNCTION_AS_VARIABLE, identifier);
            }else{
                report_error(NOT_A_FUNCTION, identifier);
            }
        }
    }else{
        if(type == VARIABLE){
            report_error( UNDEFINED_VAR, identifier);
        }else{
            if(!add_if_registered_as_external(scope, identifier)){
                report_error( UNDEFINED_FUNC, identifier);
            }else{
                return true;
            }
        }
    }

    return false;
}

bool is_of_type(struct Scope *scope, char* identifier, enum TYPE type){
    struct Declarator decl = hash__get_item(scope, identifier);
    if(decl.decl_type == VARIABLE){
        if(decl.declarator.variable.type != type){
            char* assigned;
            char* value;
            switch(type){
                case T_INT: assigned = "\033[31;1mdouble\033[0m"; value="\033[31;2mint\033[0m";break;
                case T_DOUBLE: assigned = "\033[31;2mint\033[0m"; value="\033[31;1mdouble\033[0m";break;
                case T_VOID: break; //LATER
            }
            concatenate_strings(3, "Can't assign a ", value,
                                             " to a ", assigned);
            return false;
        }
    }

    return true;
}

bool set_initialized(struct Scope* scope, char* identifier){
    struct Declarator* declarator = hash__get_item_reference(scope, identifier);
    if(declarator != NULL){
        declarator->declarator.variable.initialized = 1;
        return true;
    }

    return false;
}

bool check_main_exists(struct Scope* scope){
    struct Declarator main_decl = hash__get_item(scope, "main");
    if(main_decl.decl_type == FUNCTION){
        return true;
    }

    report_warning(MAIN_NOT_EXISTING, 0);
    return false;
}