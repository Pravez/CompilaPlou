/*
* Display the main errors implemented
* 
* Expected result : display of errors : DEFINED_VAR, UNDEFINED_FUNC, MISSING_CONDITION
 * 					display of warning : MAIN_NOT_EXISTING
*/

int fun() { /* main_not_existing */
	int a =5;
	int b;
	int a = 8; /* defined_var */
	fantom(); /* undefined_function */

    for(b = 0;;b < 10){ /* missing_condition */
        b++;
    }

	return 1;
}
