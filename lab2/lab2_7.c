#include <stdio.h>
#include <stdlib.h>

main() {
	execl("/bin/abc", "abc", NULL);
	printf("What happened?\n");
}
