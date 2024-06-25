#include <stdio.h>

int main() {
	
	int n = 10;
	int A[10];
	int B[10];
	int C[10];
	for(int i = 0; i<n;i++) {
		A[i]=i;
		printf("I: %d\n",A[i]);
	
	}
	
	for(int j = 0; j<n;j++){
		B[j]=A[j]+2;
		printf("J: %d\n",B[j]);
}

	for(int k = 0; k<n;k++) {
		C[k] = B[k]*3;
		printf("K: %d\n",C[k]);
	}


return 0;
}
