#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>


#include <unistd.h> // unix standard library

////////////////////////////////////////////////////////////////////////////////
// Includes - default libraries - C++
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cstring>
#include <map> 


using namespace std;
////////////////////////////////////////////////////////////////////////////////
// Includes - system dependent libraries
////////////////////////////////////////////////////////////////////////////////
#if defined(unix) || defined(__unix) || defined(__unix__) || (defined (__APPLE__) && defined (__MACH__)) // Unix (Linux, *BSD, Mac OS X)
#include <sys/types.h>
#include <dirent.h>
#endif

#include <cweb.hpp>


int main() {
	printf("hello world!\n");
	return 0;
}

