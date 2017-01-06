/*
* test the basical use of function and condition if
* 
* Expected result : 1
*/

int fun() {
	int a = 5;
	a += 8;
	return a;
}

int main()
{
	int a = fun();
	if (a < 13) {
		return 0;
	} else {
		return 1;
	}

    return 10;
}
