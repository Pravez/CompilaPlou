int rec(int i){
    if (i==0)
        return i;
    return rec(i-1);
}

int main(){
    rec(100);
}