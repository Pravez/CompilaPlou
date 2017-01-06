/*
* Display the main errors implemented
* 
* Expected result : display of errors : DEFINED_VAR, UNDEFINED_FUNC, MISSING_CONDITION, UNARY_ON_FUNCTION,
 *                                      VOID_TYPE_USED_AS_VALUE, FUNCTION_ARG_WRONG_TYPE
 * 					display of warning : MAIN_NOT_EXISTING, FUNCTION_WRONG_RETURN_TYPE, UNINTIALIZED_VAR
*/
int good_function_nope(){
    return 5.0; /* wrong return type */
}

void not_a_good_function(int parameter, double thing){
    int thing; /* already declared */
    int parameter;

}

void test_unary(){
    int a;
    a = a-1; /* uninitialized var */
    good_function_nope()++; /* unary on function */
    int b = not_a_good_function(1, 2); /* void assignation*/
}

int fun() { /* main_not_existing */
	int a = 5;
	int b;
	int a = 8; /* defined_var */
	fantom(); /* undefined_function */

    for(b = 0;;b < 10){ /* missing_condition */
        b++;
    }

	return 1;
}
