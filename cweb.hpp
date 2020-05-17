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
#include <deque>

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
#include <headers/stackTracer.h>
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
	
	/**
	 * delete a cookie
	 * funciona como a função:
	 * cweb::cookie::set(key, "", -1);
	 */
	inline char* del(const char *key);
		
	/**
	 * print delete a cookie
	 * funciona como a função:
	 * cweb::cookie::print(key, "", -1);
	 */
	inline void pdel(const char *key);
}

////////////////////////////////////////////////////////////////////////////////
// Interface to treat File in getenv(\"CONTENT_TYPE\") =  "multipart/form-data"
////////////////////////////////////////////////////////////////////////////////
class File
{ public:
	virtual char* name() = 0;
	virtual long  size() = 0;
	virtual void* data() = 0;
};
////////////////////////////////////////////////////////////////////////////////
// Includes - In - input - get and post
////////////////////////////////////////////////////////////////////////////////
namespace in
{
	/**
	 * inicializa os métodos abaixos.
	 * a utilização de qualquer função abaixo, sem esta inicialização gera
	 * comportamento indefinido.
	 * @arg max_size = tamanho máximo da entrada, em bytes.
	 * Se o valor for 0, essa variável será desconsiderada, que é o valor default
	 */
	extern bool init(const long max_size = 0);
	
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
	extern bool init_post(const long max_size = 0);
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
	 * Sempre gera um novo sid.
	 * Uma vez que está função roda, a função 'save()' já conhece o sid.
	 */
	extern char* sid();
	
	/**
	 * Retorna o valor do cookie sid.
	 * uma vez que a função 'sid()' foi chamada, esta função já reconhece o sid().
	 * caso não seja chamada, e se deseja salvar a sessão, deve-se chamar a função como
	 * o parâmetro como true: cweb::session::save(true)
	 */
	extern char* save(const bool toSave = false);
	
	/**
	 * Retorna true em caso de inserção e falso em caso contrário.
	 */
	extern bool set(
		const char *key = NULL,
		const void *value = NULL,
		const int size = 0,
		const int numElem = 1);

	template <typename T>
	inline bool set(
		const char *key,
		T value);
	
	extern inline bool
	set(const char *key, const char *value);

	extern inline bool
	set(const char *key, const string& value);

	extern void* getv( // get vector
		const char *key = NULL,
		int *numElem = NULL,
		int *size = NULL);
	
	template <typename T>
	inline T get(const char *key);
	
	inline char* get(const char *key = NULL);
	
	extern void* delv( // del vector
		const char *key = NULL,
		int *numElem = NULL,
		int *size = NULL);
	
	template <typename T>
	inline T del(const char *key);
	
	inline char* del(const char *key = NULL);
	
	/**
	 * remove todas as chaves e seus valores da sessão.
	 * @return retorna o número de chaves excluídas.
	 */
	extern int clean(); 
}

////////////////////////////////////////////////////////////////////////////////
// Includes - Out - output
////////////////////////////////////////////////////////////////////////////////
namespace out
{
	////////////////////////////////////////////////////////////////////////////////
	// how to use
	////////////////////////////////////////////////////////////////////////////////
	/*
 	 * @descripion: como funciona o parser para encontrar a tag <?cweb CONTENT ?>
	 * '<?cweb #in "name" ?>' -> inclui o objeto chamado name, que foi colocado nas funções:
	 * void str(const char *name, const char *output);
	 * void file(const char *name, FILE *output);
	 * void file(const char *name, const char *file_name);
	 * onde "name" da tag é o parâmetro 'name'.
	 * '<?cweb #add "file_name" ?>' -> inclui o conteúdo do arquivo, cujo nome é "file_name"
	 * A diferença é que esta tag não necessita de ser inserida por meio das funções de inserção de saída.
	 * A tag será então substituída pelo conteúdo do objeto name, pode ser um arquivo ou uma string.
	 * Após a inserão, continuará a impressão do objeto de onde parou.
	 * '<?cweb@' -> imprime normalemnte o valor '<?cweb' -> ou seja, o caracter '@' será omitido,  
	 * porém isto somente ocorre com o primeiro caracter posterior.
	 * - deve-se utilizar tal artifício ('<?cweb@') nos comentários, pois o * parser não distingue se a linha está em um comentário ou não.
	 * @ TODO - criar uma nova versão que verifica se a linha está dentro de um comentário ou não *
	 * all character of name must be a letter (A to Z or a to z) or a digit (0 to 9) or special character ('_' and '-')
	 * O nome da tag inserida nas funções -> 'name', não pode ser "___tag_add___XXX"
	 * OBS1: "void file(const char *name, FILE *output);"
	 * A função NÃO chama a função rewind(FILE *output) explicitamente, nem está
	 * função nem a função "void print()"
	 * ou seja será impresso o arquivo, a partir da posição informada pelo poteiro
	 * FILE *output, no momeno em que a função "void print()" for chamada.
	 * OBS2: o conteúdo da variável "const char *name" é copiado para dentro da classe
	 * assim o usuário é livre para reutilizar a memória, sem se preocupar com resultado
	 * indefinido pela biblioteca.
	 * OBS3: o conteúdo das variáveis "const char *output" da função "void str(const char *name, const char *output)" e a variável "const char *file_name" da função "void file(const char *name, const char *file_name);" são copiados para dentro da biblioteca, para o usuário
	 * ser livre para reutilizar e manusear a memória como bem lhe parecer.
	 * para liberar essas memórias utilize a função "void clean()"
 */

	extern void str(const char *name, const char *output);
	
	extern void file(const char *name, FILE *output);
	
	extern void file(const char *name, const char *file_name);
	
	/**
	 * Envia o arquivo gerado na saída para a saída padrão do fast-cgi.
	 * NÃO É RECURSIVO - Se um arquivo já foi impresso, ele NÃO será novamente impresso.
	 * Isto é feito para evitar entradas em loops infinitos
	 */
	extern void print();
	
	/**
	 * se utilizado imediatamente antes da função print, o comportamento é indefinido.
	 * como a função copia 
	 */
	extern void clean();
	
	namespace error
	{
		extern void str(const char *name, const char *output);
	
		extern void file(const char *name, FILE *output);
	
		extern void file(const char *name, const char *file_name);
		
		extern void print();
		
		extern void clean();
	}
}
} // end namespace cweb


////////////////////////////////////////////////////////////////////////////////
// Implementation of templates and inline functions
////////////////////////////////////////////////////////////////////////////////
inline char*
cweb::cookie::del(const char *key)
{
	return cweb::cookie::set(key, "", -1);
}

inline void
cweb::cookie::pdel(const char *key)
{
	return cweb::cookie::print(key, "", -1);
}

template <typename T>
inline bool cweb::session::set(
		const char *key,
		T value)
{
	T *ptr = new T;
	*ptr = value;
	return cweb::session::set(key, ptr, sizeof(T), 1);
}

inline bool
cweb::session::set(const char *key, const char *value)
{
	return cweb::session::set(key, value,
		sizeof(char), strlen(value)+1);
}

inline bool
cweb::session::set(const char *key, const string& value)
{
	return cweb::session::set(key, value.c_str(),
		sizeof(char), strlen(value.c_str())+1);
}

template <typename T>
inline T cweb::session::get(const char *key)
{
	T* ptr = static_cast<T*>(cweb::session::getv(key));
	if(ptr == NULL) {
		return static_cast<T>(NULL);
	}
	return *ptr;
}

inline char*
cweb::session::get(const char *key)
{
	return static_cast<char*>(
		cweb::session::getv(key));
}

template <typename T>
inline T cweb::session::del(const char *key)
{
	T* ptr = static_cast<T*>(cweb::session::delv(key));
	if(ptr == NULL) {
		return static_cast<T>(NULL);
	}
	return *ptr;
}

inline char*
cweb::session::del(const char *key)
{
	return static_cast<char*>(
		cweb::session::delv(key));
}
#endif // CWEBPP_H

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////


































