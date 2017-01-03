int a, b;
double c = (5.0 + 5);


double add_one(int a){
    return (double) ++a;
}

int main(){
    int a = 0;
    return ((int) (add_one(a) -1 + a));
}