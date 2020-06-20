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
#include <sys/syscall.h>

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
/**#define TRACE_FUNC \
fprintf(stderr, "\n*****************************************\n");\
fprintf(stderr, "TRACE FUNC:: \"%s\" (%d, \"%s\")\n",\
__PRETTY_FUNCTION__, __LINE__, __FILE__);\
fprintf(stderr, "*****************************************\n");
*/
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
 * em caso de erro, retorna nullptr.
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
	 * @arg key: if the value is nullptr, this function return nullptr.
	 * @return: nullptr if the key does not exists in map or the value is nullptr.
	 */
	extern const char* get(const char *key);
	
	
	/**
	 * Retorna uma string contendo todo o conteúdo do cookie.
	 * os valores key e value, não podem ser nullptr e também não podem ser uma
	 * string vazia.
	 * Insert the string: " GMT" ao final do tempo designado.
	 * O tempo é recuperado com a função localtime(time(nullptr) + expires_sec)
	 * Se o argumento "expires_sec" é 0, ele não é inserido.
	 * Se os argumentos "domain" e "path" são nullptrs, eles não são inseridos.
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
	 * os valores key e value, não podem ser nullptr e também não podem ser uma
	 * string vazia.
	 * Insert the string: " GMT" ao final do tempo designado.
	 * O tempo é recuperado com a função localtime(time(nullptr) + expires_sec)
	 * Se o argumento "expires_sec" é 0, ele não é inserido.
	 * Se os argumentos "domain" e "path" são nullptrs, eles não são inseridos.
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
class file
{ public:
	virtual char* name() = 0;
	virtual long  size() = 0;
	virtual char* data() = 0;
	virtual char  type() = 0;
	virtual char* typeStr() = 0;
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
	 * Se o valor for <= 0, essa variável será desconsiderada, que é o valor default
	 */
	extern bool init(const long max_size = 0);
	
	/**
	 * serve tanto para 'get' quanto 'post' ("application/x-www-form-urlencoded").
	 * a função init(); seta automaticamente qual é o method utilizado.
	 */
	extern char* read(const char *key);
	
	/**
	 * apenas para o getenv("REQUEST_METHOD") = "GET".
	 * valores diferentes o comportamento é indefinido.
	 */
	extern char* get(const char *key);
	
	/**
	 * apenas para o getenv("REQUEST_METHOD") = "POST".
	 * getenv("CONTENT_TYPE") = "application/x-www-form-urlencoded"
	 * valores diferentes o comportamento é indefinido.
	 */
	extern char* post(const char *key);
	
	/**
	 * apenas para o getenv("REQUEST_METHOD") = "POST".
	 * getenv("CONTENT_TYPE") = "multipart/form-data"
	 * valores diferentes o comportamento é indefinido.
	 */
	extern file& fpost(const char *key); 
	
	/**
	 * Inicializa a estrutura para armazenagem dos valores se:
	 * getenv("REQUEST_METHOD") = "GET".
	 * OBS: não é necessário chamar, pois já é chamada implicitamente pelo init();
	 */
	extern bool init_get(const long max_size = 0);
	
	/**
	 * Inicializa a estrutura para armazenagem dos valores se:
	 * getenv("REQUEST_METHOD") = "POST".
	 * getenv("CONTENT_TYPE") = "application/x-www-form-urlencoded"
	 * OBS: não é necessário chamar, pois já é chamada implicitamente pelo init();
	 */
	extern bool init_post(const long max_size = 0);
	
	/**
	 * Inicializa a estrutura para armazenagem dos valores se:
	 * getenv("REQUEST_METHOD") = "POST".
	 * getenv("CONTENT_TYPE") = "multipart/form-data"
	 * OBS: não é necessário chamar, pois já é chamada implicitamente pelo init();
	 */
	extern bool init_fpost(const long max_size = 0);
	
	/**
	 * Verifica se o valor de user_method é igual 
	 * ao retornado pela função "getenv("REQUEST_METHOD")";
	 * Também verifica se o valor de type é igual
	 * ao retornado pela função "getenv("CONTENT_TYPE")"
	 * @OBS: se o valor de type = nullptr -> não faz a verificação.
	 * @OBS: o valor de user_method sempre é comparado com getenv("REQUEST_METHOD")
	 * mesmo se ambos forem nullptr
	 * @throw: essa sempre lança uma exceção em caso de erro.
	 * a exceção é uma "string&", cujo conteúdo contem:
	 * 1-> identificação: "cweb::error::inMethod["
	 * 2-> onde ocorreu o erro na verificação.
	 * TODO - implementar uma conferência por meio de uma classe de erro específica.
	 */
	extern void method(const char *user_method = nullptr,
						const char *type = nullptr);
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
	
	
	/**
	 * Carrega a sessão que se encontra no arquivo de sessão definido pelo cookie "sid".
	 * @return: true: caso consiga carregar os valores.
	 * false: otherwise
	 */
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
	 * caso a função 'sid()' não foi chamada anteriormente, 
	 * e se deseja salvar a sessão, deve-se chamar a função como
	 * o parâmetro como true: cweb::session::save(true)
	 */
	extern char* save(const bool toSave = false);
	
	/**
	 * Retorna true em caso de inserção e falso em caso contrário.
	 */
	extern bool setv(
		const char *key = nullptr,
		const void *value = nullptr,
		const int size = sizeof(int),
		const int numElem = 1);

	extern inline bool
	set(const char *key, const char *value);
	
	extern inline bool
	set(const char *key, const int value);

	/**
	 * Retorna nullptr caso a chave key não exista no map.
	 */
	extern void* getv( // get vector
		const char *key,
		int *numElem = nullptr,
		int *size = nullptr);
	
	/**
	 * Retorna nullptr caso a chave key não exista no map.
	 */		
	extern inline char* get(const char *key);
	extern inline int geti(const char *key);
	
	extern void* delv( // del vector
		const char *key,
		int *numElem = nullptr,
		int *size = nullptr);
	
	inline char* del(const char *key);
	inline int deli(const char *key);
	
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

////////////////////////////////////////////////////////////////////////////////
// Includes - Route
////////////////////////////////////////////////////////////////////////////////
/**
 * os parâmetros são os mesmos.
 * @PARAM PATH = string que contém o caminho requisitado, ou seja, deve ser igual ao
 * retornado pela variável PATH_INFO que é preenchida pelo HTTP.
 * @PARAM FUNC = função que deve ser chamada, caso o endereço seja requisitado.
 * para utilizar mais de um código, apenas usar o parênteses ex: page_index(); page_index_2()
 */
#define cweb_route_init(PATH, FUNC) \
	char *cweb_route_path = getenv("PATH_INFO"); \
	if(cweb_route_path == nullptr) {\
		FUNC;\
	} else if(strcmp(cweb_route_path, PATH) == 0) {\
		FUNC;\
	}

#define cweb_route(PATH, FUNC) \
	else if(strcmp(cweb_route_path, PATH) == 0) {\
		FUNC;\
	}

#define cweb_route_error(FUNC) \
	else {\
		FUNC;\
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

inline bool
cweb::session::set(const char *key, const char *value)
{
	return cweb::session::setv(key, value,
		sizeof(char), strlen(value)+1);
}

inline char*
cweb::session::get(const char *key)
{
	return static_cast<char*>(
		cweb::session::getv(key));
}

inline char*
cweb::session::del(const char *key)
{
	return static_cast<char*>(
		cweb::session::delv(key));
}

inline bool
cweb::session::set(const char *key, const int value)
{ try {
	int* _i = new int;
	*_i = value;
	return cweb::session::setv(key, _i,
		sizeof(int), 1);
 } catch(u::error& e) { throw err();
 } catch(std::bad_alloc& e) { throw err("std::bad_alloc - \"%s\"", e.what()); }
}

inline int
cweb::session::geti(const char *key)
{
	int* _i = static_cast<int*>(
		cweb::session::getv(key));
	if(_i == nullptr) throw err("no integer key\nkey = \"%s\"", key);
	return *_i;
}

inline int
cweb::session::deli(const char *key)
{
	int* _i = static_cast<int*>(
		cweb::session::delv(key));
	if(_i == nullptr) throw err("no integer key\nkey = \"%s\"", key);
	return *_i;
}
#endif // CWEBPP_H

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////


































