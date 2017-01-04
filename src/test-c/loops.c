//#include<stdio.h>

int main(){

    int a;

    a = 0;

    if(a++){
        a = 20;
        //printf("a++ est vrai");
    }

    a = 0;

    if(++a){
        a = 30;
        //printf("++a est vrai");
    }
    
    int b;

    for(a = 0; a < 10; a++){
        b++;
        //printf("a %d\n", a);
    }

    return 0;
}
