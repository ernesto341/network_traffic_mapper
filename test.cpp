#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gnu/libc-version.h>

#define REQ assert(atof(gnu_get_libc_version()) > 3.0);

int main(void)
{
	float ver = atof(gnu_get_libc_version());
	assert(ver > 2.0);
	printf("%f\n", ver);
	return(0);
}
