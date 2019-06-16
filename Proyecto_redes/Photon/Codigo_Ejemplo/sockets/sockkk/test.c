#include <stdio.h>
char buffer[6];
int main()
{
 	int a = 12345;
 	sprintf(buffer,"%d",a);
 	printf("%c\n",buffer[0]);
 	printf("%c\n",buffer[6]);  
 	return 0;
}