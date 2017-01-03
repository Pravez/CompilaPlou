int a, b;

double c(){
    return 5.0;
}

int c(){
    return 3;
}

double add_one(int a){
    return (double) ++a;
}

int main(){
    int a = 0;
    return ((int) (add_one(a) -1 + a));
}