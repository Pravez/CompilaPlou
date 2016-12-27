#include "scope.h"
#include "tools.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int hachage(char *s) {
    unsigned int hash = 0;
    while (*s != '\0') hash = hash * 31 + *s++;
    return hash % HASH_SIZE;
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
    for (int i = 0; i <= hashmap->higher_level; i++) {
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
    hashmap->higher_level++;
    hash__clean_level(hashmap, hashmap->higher_level);

    for(int i=0;i<declarator.declarator.function.var_list_size;i++){
        //Because the upper level is truly new, the probability to have an error is zero ?
        if(!hash__key_exists_all(hashmap, declarator.declarator.function.var_list[i].identifier)) {
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
        }else{
            last_error = report_error( DEFINED_FUNC_VAR, declarator.declarator.function.var_list[i].identifier);
            return false;
        }
    }

    return true;
}

/**
 * Main function to add an item. Finds the place of the declarator and affects values
 * @param hashmap
 * @param key
 * @param declarator
 * @return
 */
bool hash__add_item(struct Scope *hashmap, char *key, struct Declarator declarator) {
    //If the item is not present in the hashmap
    if (!hash__key_exists_all(hashmap, key)) {
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
void hash__upper_level(struct Scope *hashmap) {
    hashmap->current_level++;
    if(hashmap->current_level > hashmap->higher_level) {
        hashmap->higher_level++;
        hash__clean_level(hashmap, hashmap->current_level);
    }
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
    for(int i=0;i<list.size;i++){
        if(list.declarator_list[i].decl_type == FUNCTION){
            if(!hash__add_item(hashmap, list.declarator_list[i].declarator.function.identifier, list.declarator_list[i])){
                if(error_flag == 0) {
                    last_error = report_error(DEFINED_FUNC, list.declarator_list[i].declarator.function.identifier);
                }
                return false;
            }
        }else{
            if(!hash__add_item(hashmap, list.declarator_list[i].declarator.variable.identifier, list.declarator_list[i])){
                last_error = report_error( DEFINED_VAR, list.declarator_list[i].declarator.variable.identifier);
                return false;
            }
        }

    }

    return true;
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
    if(!hash__key_exists_all(scope, identifier)){
        if(type == VARIABLE)
            last_error = report_error( UNDEFINED_VAR, identifier);
        else
            last_error = report_error( UNDEFINED_FUNC, identifier);

        return false;
    }

    return true;
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
            last_error = concatenate_strings(3, "Can't assign a ", value,
                                             " to a ", assigned);
            return false;
        }
    }

    return true;
}