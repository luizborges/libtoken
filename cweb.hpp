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
		const bool isSecure = false,
		const bool isHttpOnly = false,
		const char *domain = getenv("SERVER_NAME"),
		const char *path = "/");
	
	
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
		const bool isSecure = false,
		const bool isHttpOnly = false,
		const char *domain = getenv("SERVER_NAME"),
		const char *path = "/");
}

////////////////////////////////////////////////////////////////////////////////
// Includes - In - input - get and post
////////////////////////////////////////////////////////////////////////////////
namespace in
{
	/**
	 * inicializa os métodos abaixos.
	 * a utilização de qualquer função abaixo, sem esta inicialização gera
	 * comportamento indefinido.
	 */
	extern bool init();
	
	/**
	 * serve tanto para 'get' quanto 'post'.
	 * a função init(); seta automaticamente qual é o method utilizado.
	 */
	extern const char* read(const char *key);
	
	/**
	 * apenas para o getenv("REQUEST_METHOD") = "GET".
	 * quando o getenv("REQUEST_METHOD") = "POST" o comportamento é indefinido.
	 */
	extern const char* get(const char *key);
	
	/**
	 * apenas para o getenv("REQUEST_METHOD") = "POST".
	 * quando o getenv("REQUEST_METHOD") = "GET" o comportamento é indefinido.
	 */
	extern const char* post(const char *key);
	
	/**
	 * Inicializa a estrutura para armazenagem dos valores se:
	 * getenv("REQUEST_METHOD") = "GET".
	 * OBS: não é necessário chamar, pois já é chamada implicitamente pelo init();
	 */
	extern bool init_get();
	
	/**
	 * Inicializa a estrutura para armazenagem dos valores se:
	 * getenv("REQUEST_METHOD") = "POST".
	 * OBS: não é necessário chamar, pois já é chamada implicitamente pelo init();
	 */
	extern bool init_post();
}

////////////////////////////////////////////////////////////////////////////////
// Includes - Session
////////////////////////////////////////////////////////////////////////////////
namespace session
{
	/**
	 * @dirFileSession: diretório em que será criado o arquivo de Sessão.
	 * @lifeSession: tempo que uma sessão dura - tempo em segundos.
	 * @del: tempo (em segundos) para se deletar, dentro do diretório, todos os aruqivos de
	 * sessão. A primeiro coisa que ele faz, é entrar dentro do diretório, e deletar
	 * todos os arquivos de sessão que estão dentro do diretório que foram criados
	 * a mais de 'del' segundos, onde del é esta variável.
	 * @OBS: se a variável @del, for menor que 0, está função não deleta os arquivos
	 * de sessão antigos no diretório indicado.
	 * @OBS: se a variável @del for menor que a variável @lifeSession, está função não
	 * deleta os arquivos da sessão antigos no diretório indicado.
	 * é como se o valor fosse @del < 1.
	 * @OBS: para a opção de deletar os arquivos antigos da sessão, deve-se ter:
	 * if(@del > @lifeSession)
	 */
	extern void
	config(const char *dirFileSession = "",
			const time_t lifeSession = 5*60, // valor default de 5 min
			const time_t del = 10*60); // valor default de 10 min
	
	
	extern bool load();
	
	/**
	 * Retorna o valor do cookie sid.
	 */
	extern char* save();
}
} // end namespace cweb
#endif // CWEBPP_H

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

































