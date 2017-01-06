/*
* Test in real conditions, with a classic program
* 
* Expected result : return fibo(10) : 55
*/

int fibo(int n) {
	
}


int main() { /* main_not_existing */
   int n, first = 0, second = 1, next, c;
   
   for (c = 0 ; c < 10 ; c++ )
   {
      if ( c <= 1 )
         next = c;
      else
      {
         next = first + second;
         first = second;
         second = next;
      }
   }
 
   return next;
}
