/*
* Test the basic use of while and for
* 
* Expected result : 5
*/

int main() {
	int a = 5;
	int cpt1 =0, cpt2=0;
	while (a < 25) {
		cpt1++;
		a *= 5;
	}
	
	for (a=5; a < 25; a*=5)
	{
		cpt2++;
	}
	
	int avg = (cpt1 + cpt2) /2;
	
	return avg;
}
