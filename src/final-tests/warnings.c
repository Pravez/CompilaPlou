/*
* Display the main warnings implemented
* 
* Expected result : warnings : ASSIGN_DOUBLE_TO_INT, UNINTIALIZED_VAR, MISSING_AN_INITIALISATION, MISSING_A_MOVING
*					return 10;
*/

int maint() { /* main_not_existing */
	int a =5.0;
	int b;
	
	for( ;a<10; ) {
		b++;
		a++;
	}
	return a;
}
