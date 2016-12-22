int square(int x) {
  return x*x;
}
int add(int x,int y) {
  return x+y;
}
int main() {
  int i,x;
  for (i=0; i<1000; i++) x = add(i,x);
  return x;
}
