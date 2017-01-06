/*
* test the most simple program possible, to verify the basic process of compilation
* 
* Expected result : 15
*/

int main() {
	int a;
	a = 5;
	int b = 5;
	a = a + b;
	b += a;
	
	return b;
}
