#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <zconf.h>
#include "../tools.h"

void test_concatenate(){
    printf("%s", __PRETTY_FUNCTION__); fflush(stdout);
    char* str1 = "un,";
    char* str2 = "deux,";
    char* str3 = "trois !";

    char* res = concatenate_strings(3, str1, str2, str3);
    assert(strcmp(res, "un,deux,trois !") == 0);
    free(res);
    printf("."); fflush(stdout);

    res = concatenate_strings(2, str1, str3);
    assert(strcmp(res, "un,trois !") == 0);
    free(res);
    printf("."); fflush(stdout);

    res = concatenate_strings(2, str1, str2, str3);
    assert(strcmp(res, "un,deux,") == 0);
    free(res);

    printf("."); fflush(stdout);

/*
    // Comportement indéfini
    for(int i =0; i < 100; ++i) {
        res = concatenate_strings(5, "coucou");
        printf("'%s'\n", res);
        assert(strcmp(res, "coucou") == 0);
    }
    free(res);
*/

    printf("done\n");
}

int main(){
    test_concatenate();

    return 0;
}