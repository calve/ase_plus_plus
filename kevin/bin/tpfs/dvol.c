#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mbr.h"

int main()
{
	init_mbr();
	list_volumes();
	exit(EXIT_SUCCESS);
}