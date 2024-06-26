#include <stdio.h>

int main() {
	
	int n = 10;
	int A[10];
	int B[10];
	int C[10];
	int a = 0;
	int b = 0;
	for(int i = 0; i<n;i++) {
		a++;
		A[i]=a;
		printf("I: %d\n",A[i]);
	
	}
	
	for(int j = 0; j<n;j++){
		b+=2;
		B[j]=b;
		printf("J: %d\n",B[j]);
}

	for(int k = 0; k<n;k++) {
		C[k] = k;
		printf("K: %d\n",C[k]);
	}


return 0;
}
