#include <stdio.h>

int main() {
	int n = 10;
	int A[n];
	int B[n];
	int C[n];
	int a = 0;
	int b = 0;
	for(int i = 0; i<n;i++) {
	    a = a + 2;
		A[i]=i;
		printf("I: %d\n",A[i]);
	}

	for(int j = 0; j<n;j++){
	    b = b + 2;
		B[j]=j;
		printf("J: %d\n",B[j]);
	}

	for(int k = 0; k<n;k++) {
		C[k] = k;
		printf("K: %d\n",C[k]);
	}


return 0;
}
