#include <iostream>
int main() {
 int i;
 int j;
 int k;
 i = 0;
 k = 13;
 j = 19;
 do {
   i = i+1;
   j = j*2;
   while (i<10) {
     i = i+1;
     j = j+1;
   }
 }
 while (i==10);
 for (k = 15;
      i>13;
      i = i-1;
     ) {
   k = i+k;
 }
 std::cin >>  i >>  j >>  k;
 std::cout << i << j << k;
    return 0;
}
