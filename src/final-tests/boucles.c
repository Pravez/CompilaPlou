/*
* Test the basic use of while and for
* A bit a stupid test, only idea is to do some operations ...
* Expected result : 1
*/

int main() {
	int a = 5;
	int cpt1, cpt2;
	cpt1 = 0;
	cpt2 = 0;
	while (a < 25) {
		cpt1++;
		a *= 5;
	}

	for (a=5; a < 25; a*=5)
	{
		cpt2++;
	}

	int i;
	for(i=0;i<25;i++);

	int avg = (cpt1 + cpt2) /2;

	return (a/avg)/i;
}
