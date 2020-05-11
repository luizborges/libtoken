/**
 *
 * @descripion: funciona como um header que contém todas as bibliotecas, é necessário apenas adicionar
 * essa biblioteca para ter acesso a todas a biblioteca.
 */
#ifndef CWEBPP_H
#define CWEBPP_H


////////////////////////////////////////////////////////////////////////////////
// Includes - default libraries - C
////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
// Includes - my libraries
////////////////////////////////////////////////////////////////////////////////
#include <util.hpp>
////////////////////////////////////////////////////////////////////////////////
// Includes - namespace
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// namespace
////////////////////////////////////////////////////////////////////////////////
namespace cweb 
{

////////////////////////////////////////////////////////////////////////////////
// Includes - Url Percent Encode/Decode
////////////////////////////////////////////////////////////////////////////////
// implementing in file -
extern bool
decode(char* out,
	   const char* in,
	   const int maxDecode);

/**
 * substituiu o character ' ' pelo character '+'.
 * @return retorna a string codificada em caso de acerto.
 * em caso de erro, retorna NULL.
 */
extern char*
encode(const char* in,
	   const int maxEncode);


////////////////////////////////////////////////////////////////////////////////
// Includes - Cookie
////////////////////////////////////////////////////////////////////////////////
namespace cookie
{
	extern bool init();

	/**
	 * @arg key: if the value is NULL, this function return NULL.
	 * @return: NULL if the key does not exists in map or the value is NULL.
	 */
	extern const char* get(const char *key);
	
	
	/**
	 * Retorna uma string contendo todo o conteúdo do cookie.
	 * os valores key e value, não podem ser NULL e também não podem ser uma
	 * string vazia.
	 * Insert the string: " GMT" ao final do tempo designado.
	 * O tempo é recuperado com a função localtime(time(NULL) + expires_sec)
	 * Se o argumento "expires_sec" é 0, ele não é inserido.
	 * Se os argumentos "domain" e "path" são NULLs, eles não são inseridos.
	 * se os argumentos "isSecure" e "isHttpOnly" são false, eles não são inseridos.
	 * Ao final da string do cookie é incluído o charactere '\n'
	 * OBS: se o valor do expires_sec for -1, o tempo será setado para
	 * "expires=Thu, 01 Jan 1970 00:00:00 GMT" este valor é usado para expirar o cookie
 	*/
	extern char*
	set(const char *key, const char *value,
		const long expires_sec = 0,
		const char *domain = getenv("SERVER_NAME"),
		const char *path = "/",
		const bool isSecure = false,
		const bool isHttpOnly = false);
	
	
	/**
	 * A saída é gravada pela função fprintf(stdout, ... );
	 * A gravação é feita por partes, ou seja, não é criada toda a string e então
	 * inserida no stdout.
	 * os valores key e value, não podem ser NULL e também não podem ser uma
	 * string vazia.
	 * Insert the string: " GMT" ao final do tempo designado.
	 * O tempo é recuperado com a função localtime(time(NULL) + expires_sec)
	 * Se o argumento "expires_sec" é 0, ele não é inserido.
	 * Se os argumentos "domain" e "path" são NULLs, eles não são inseridos.
	 * se os argumentos "isSecure" e "isHttpOnly" são false, eles não são inseridos.
	 * Ao final da string do cookie é incluído o charactere '\n'
	 * OBS: se o valor do expires_sec for -1, o tempo será setado para
	 * "expires=Thu, 01 Jan 1970 00:00:00 GMT" este valor é usado para expirar o cookie
	 */
	extern void
	print(const char *key, const char *value,
		const long expires_sec = 0,
		const char *domain = getenv("SERVER_NAME"),
		const char *path = "/",
		const bool isSecure = false,
		const bool isHttpOnly = false);
}

} // end namespace cweb
#endif // CWEBPP_H

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////


































