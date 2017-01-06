/*
* Test in real conditions, with a classic program
* 
* Expected result : return fibo(10) : 55
*/

int main() {

    int i = 0;
    int j = 1;
    int temp;
    int k;
    int n = 9;

    for (k = 0; k < n; k++) {
        temp = i + j;
        i = j;
        j = temp;
    }

    return temp;
}
