#include <iostream>

int main() {
    int i;
    int j;
    int k;
    i = 0;
    k = 13;
    j = 19;
    if (i==0) {
     j = k;
    } else if (j==19){
     i = k;
    } else if (j<i){
     k = i;
    }
    return 0;
}
