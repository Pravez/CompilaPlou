/*
* Display the main errors implemented
* 
* Expected result : display of errors : DEFINED_VAR, UNDEFINED_FUNC, MAIN_NOT_EXISTING, MISSING_A_CONDITION
*/

int fun() { /* main_not_existing */
	int a =5;
	int b;
	int a = 8; /* defined_var */
	fantom(); /* undefined_function */
	for (i=0; ; i++) {
		a++;
	}
	return 1;
}
