/**
 *
 * @descripion: funciona como um header que contém todas as bibliotecas, é necessário apenas adicionar
 * essa biblioteca para ter acesso a todas a biblioteca.
 */
#ifndef TOKEN_HPP
#define TOKEN_HPP


////////////////////////////////////////////////////////////////////////////////
// Includes - default libraries - C
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes - default libraries - C++
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <ctime>
#include <ctype.h>
#include <map> 
#include <deque>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <streambuf>
#include <exception>
#include <list>
#include <string_view>

////////////////////////////////////////////////////////////////////////////////
// Includes - system dependent libraries
////////////////////////////////////////////////////////////////////////////////
#if defined(unix) || defined(__unix) || defined(__unix__) || (defined (__APPLE__) && defined (__MACH__)) // Unix (Linux, *BSD, Mac OS X)
#include <unistd.h> // unix standard library
#include <sys/syscall.h>
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
namespace token
{
	////////////////////////////////////////////////////////////////////////////////
	// Enum - init in token.cpp
	////////////////////////////////////////////////////////////////////////////////
	enum class type_t {
      column_split, column_token, line_break, line_break_end_token_column
    };
	
	////////////////////////////////////////////////////////////////////////////////
	// Class and structs
	////////////////////////////////////////////////////////////////////////////////
	/**
	 * Guarda o token passado pelo usuário e verifica se existe o token na string passada.
	 * Cada token passado pelo usuário é criado uma struct deste tipo.
	 */
	struct token_t {
        std::string str;
        type_t type;
        bool never_match = false; // é utilizado quando se verifica pela primeira vez que a string não contém mais o token
    
        token_t(const std::string& str, const type_t type) {
            this->str = str;
            this->type = type;
        }
        
        /**
		 * @arg strs: string em que se realizará a busca do token.
		 * @arg str_init_search: posição inicial em @strs que começará a busca pelo token.
		 * @return tuple<@1, @2>:
		 * @1: posição dentro da string do início do token.
		 * Caso não seja encontrado o token retorna: std::string::npos
		 * @2: tamanho do token - this->str.size()
		 * Caso não seja encontrado o token retorna: -1
		 */
        std::tuple<size_t, long> search(const std::string& strs);
        std::tuple<size_t, long> search(const std::string& strs, const long str_init_search);
    };
	
	
	////////////////////////////////////////////////////////////////////////////////
	// Global functions - file: token.cpp
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	// "Private" functions - file: token.cpp
	////////////////////////////////////////////////////////////////////////////////
	/**
	 * Função que realiza a busca realmente de um token
	 * Está função busca apenas um token.
	 * As chamadas são recursivas.
	 * a diferença entre as duas funções é que uma sempre cria uma substring para realizar a busca
	 * enquanto a segunda reusa a string e muda a posição inicial da busca.
	 */
	template<template<typename> typename T>
	T<std::string> search(const std::string& str,
        std::vector<token_t> token, T<std::string> result);
    
	/**
	 * @arg str_init_search: posição inicial da qual será analisada a string @str.
	 * @return: true: se a busca ainda não finalizou - deve-se chamar a função novamente.
	 * false: se a busca terminou - ou seja, toda a string já foi coberta pela função.
	 */
	template<template<typename> typename T>
    bool search(const std::string& str, 
				size_t& str_init_search,
				std::vector<token_t>& token, 
				T<std::string>& result);
	
	/**
	 * Função que realiza a busca realmente de um token.
	 * Está função busca apenas um token.
	 * Trata o resultado que o token produz.
	 * As chamadas são recursivas.
	 * Busca um token que afeta que produz um resultado em uma matrix.
	 * @return: true: se a busca ainda não finalizou - deve-se chamar a função novamente.
	 * false: se a busca terminou - ou seja, toda a string já foi coberta pela função.
	 */
	template<template<typename> typename T, template<typename> typename U>
	bool
	search_matrix(const std::string& str, 
				  size_t& str_init_search,
			      std::vector<token_t>& token,
			      T<U<std::string>>& result);
    
	////////////////////////////////////////////////////////////////////////////////
	// "Public" functions - file: token.cpp
	////////////////////////////////////////////////////////////////////////////////
	/**
	 * Função realmente que deve ser chamada pelo usuário.
	 * @arg str: string em que será realizado a busca pelo token.
	 * @arg column_split: vector que contém os tokens que "quebram", "partem", a string em diversas colunas
	 * os tokens deste vetor não entram no resultado final da string.
	 * @arg column_token: vector que contém os tokens que "quebram", "partem", a string em diversas colunas
	 * os tokens deste vector entram no resultado final da string, cada token deste se torna uma posição
	 * do vetor. A quantidade deste token no vector final será o número de vezes que ele aparecerá na string @str
	 * @return vector que contém a string dividida segundo os tokens passados na função. Cada token produz uma nova
	 * posição no vector final, embora o token possa entrar no vector final (@column_token) ou não (@column_split)
	 */
    template<template<typename> typename T>
    T<std::string> get(const std::string& str, 
        const std::vector<std::string>& column_split, 
        const std::vector<std::string>& column_token = {});
	
	/**
	 * Função realmente que deve ser chamada pelo usuário.
	 * Exemplo de como usar essa função:
	 * auto excel = token::get_matrix<std::deque, std::vector>(excel_untreated, {"\t"}, {}, {"\n", "\r"});
	 * excel será do tipo std::deque<std::vector<std::string>>
	 * @arg str: string em que será realizado a busca pelo token.
	 * @arg column_split: vector que contém os tokens que "quebram", "partem", a string em diversas colunas
	 * os tokens deste vetor não entram no resultado final da string.
	 * @arg column_token: vector que contém os tokens que "quebram", "partem", a string em diversas colunas
	 * os tokens deste vector entram no resultado final da string, cada token deste se torna uma posição
	 * do vetor. A quantidade deste token no vector final será o número de vezes que ele aparecerá na string @str
	 * @arg line_split: funciona identico ao @column_split, com a diferença que os próximos resultados
	 * serão colocados na linha subsequente da matrix, ou seja, se os resultados eram colocados na linha i
	 * os próximos serão colocados na linha i+1
	 * Este token não integra o resultado.
	 * @arg line_token: funciona identico ao @column_token, com a diferença que os próximos resultados
	 * serão colocados na linha subsequente da matrix, ou seja, se os resultados eram colocados na linha i
	 * os próximos serão colocados na linha i+1
	 * Este token integra o resultado, e é sempre o último token da linha, ou seja, da linha i -> linha nova i+1
	 * que este token produz ao final.
	 * @return matrix que contém a string dividida segundo os tokens passados na função. 
	 * Cada token produz uma nova posição no matrix final:
	 * (@line_split e @line_token) produzem uma nova linha na matrix e 
	 * (@column_split e @column_token) não produzem uma nova linha na matrix, apenas afetam a linha atual da matrix
	 * Embora o token possa entrar na matrix (@column_token e @line_token) ou não (@column_split e @line_split)
	 */
	template<template<typename> typename T, template<typename> typename U>
	T<U<std::string>> get_matrix(const std::string& str, 
			   const std::vector<std::string>& column_split, 
			   const std::vector<std::string>& column_token = {}, 
			   const std::vector<std::string>& line_split = {}, 
			   const std::vector<std::string>& line_token = {});
	
	/**
	 * @brief Busca se dentro de uma string contém uma uma string do tipo C/C++
	 * As strings procuradas são as que começam com os characteres: '"'.
	 * É válido os characteres de escape. \\ \" para escapar tanto o começo, quanto o fim da string.
	 * Ao final, retorna se encontrou ou não uma string, e a posição de começo e de fim da string.
	 * @obs: a string em que será buscada será @param(str).substr(@param(begin), @param(count))
	 * ou seja, uma substring definida pelos parâmetros begin e count.
	 * @obs: os valores dentro da @arg(str) fora dos parâmetros passados, são totalmente ignorados pelo programa.
	 * Exemplo:
	 * str: "012"45"7" -> begin = 2 -> count = 8
	 * return: tuple<true, 3, 4>
	 * str: "012"45"7" -> begin = 5 -> count = 8
	 * return: tuple<false, 6, 2>
	 * str: "012"45"7" -> begin = 7 -> count = 8
	 * return: tuple<false, std::string::npos, 0>
	 * str: "01\"45"7" -> begin = 0 -> count = 8
	 * return: tuple<false, 6, 2>
	 * str: "01\"45"7" -> begin = 3 -> count = 8
	 * return: tuple<true, 3, 4>
	 * @obs: se str.empty() == true -> return tuple<false, std::string::npos, std::string::npos>
	 * @throw u::error: se count = 0, nenhuma busca será feita.
	 * @throw:u::error: se begin >= str.size().
	 * @param str: string que será buscada a string C/C++.
	 * @param begin: posição de início das buscas.
	 * @param count: Número de characteres que será buscado após o índice passado em @param(begin).
	 * Segue o padrão da função std::string::substr().
	 * O parâmetro count significa quantos characteres serão buscados, considerados desde a posição begin
	 * @return std::tuple<bool, size_t, size_t>
	 * bool: true: se existe uma string completa (com início e fim) C/C++ dentro da string.
	 * false: caso não exista uma string completa, ou se a string está incompleta, sem fim.
	 * size_t(1): posição de início da string C/C++. Caso não haja início na string, o valor retornado é o std::string::npos.
	 * size_t(2): o tamanho (número de characteres) da string encontrada, contados os characteres '"' de início e fim da string
	 * Caso não haja string retornada, será retornado o valor 0.
	 */
	std::tuple<bool, size_t, size_t>
	find_str(const std::string& str, 
		const size_t begin = 0,
		const size_t count = std::string::npos);
	
	std::tuple<bool, size_t, size_t>
	find_str(const std::string_view& str, 
		const size_t begin = 0,
		const size_t count = std::string::npos);
	
	/**
	 * @brief Find the block in a string.
	 * string de busca = @param (str).substr(@param(begin), @param(count))
	 * @param str: string that will be searched.
	 * @param block_begin: string that marks the begin of block.
	 * @param block_end: string that marks the end of block.
	 * @param begin: initial position of @param(str) that search will beginning
	 * @param count: number of characteres that will be searching after begin.
	 * @return std::tuple<bool, size_t, size_t>
	 * bool: true: se existe um bloco completo.
	 * false: caso não exista um bloco, ou se o bloco está incompleta, sem fim.
	 * size_t(1): posição do bloco. Caso não haja início do bloco, o valor retornado é o std::string::npos.
	 * size_t(2): o tamanho (número de characteres) do bloco encontrado, contados os characteres de @param(block_begin) e @param(block_end).
	 * Caso não haja string retornada, será retornado o valor 0.
	 * Caso seja encontrado apenas o bloco inicial e não o final, é retornado o número de characteres do bloco inicial até o fim da string de busca.
	 */
	std::tuple<bool, size_t, size_t>
	find_block(const std::string& str,
		const std::string& block_begin,
		const std::string& block_end,
		const size_t begin = 0,
		const size_t count = std::string::npos);
	
	std::tuple<bool, size_t, size_t>
	find_block(const std::string_view& str,
		const std::string& block_begin,
		const std::string& block_end,
		const size_t begin = 0,
		const size_t count = std::string::npos);
	

	////////////////////////////////////////////////////////////////////////////////
	// Class error
	////////////////////////////////////////////////////////////////////////////////
	
}
////////////////////////////////////////////////////////////////////////////////
// Implementation of inline functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Implementation of template functions
////////////////////////////////////////////////////////////////////////////////
template<template<typename> typename T>
T<std::string> token::get(const std::string& str, 
						  const std::vector<std::string>& column_split, 
						  const std::vector<std::string>& column_token) {
 try {
    std::vector<token_t> token;
    for(const auto& s : column_split) { 
		if(!s.empty()) token.push_back({s, type_t::column_split});
	}
    for(const auto& s : column_token) { 
		if(!s.empty()) token.push_back({s, type_t::column_token});
	}
        
    T<std::string> result = {};
	size_t str_init_search = 0;
	
	/**
	 * a função é recursiva.
	 * Em entradadas relativamente grandes, a função estava estourando a pilha e a recursão de cauda não estava
	 * funcinonando, esta foi a maneira de fazer com que a função rode com entradas grandes.
	 */
	while(search(str, str_init_search, token, result));
	return result;
 } catch (const std::exception &e) { throw err(e.what()); }
}

template<template<typename> typename T, template<typename> typename U>
T<U<std::string>> token::get_matrix(const std::string& str, 
									const std::vector<std::string>& column_split,
									const std::vector<std::string>& column_token, 
									const std::vector<std::string>& line_split,
									const std::vector<std::string>& line_token) {
 try {
    std::vector<token_t> token;
    for(const auto& s : column_split) { if(!s.empty()) token.push_back({s, type_t::column_split}); }
    for(const auto& s : column_token) { if(!s.empty()) token.push_back({s, type_t::column_token}); }
    for(const auto& s : line_split)   { if(!s.empty()) token.push_back({s, type_t::line_break}); }
    for(const auto& s : line_token)   { if(!s.empty()) token.push_back({s, type_t::line_break_end_token_column}); }
        
    T<U<std::string>> result = {};
	result.push_back({}); // inicializa a primeira linha da matrix
	size_t str_init_search = 0;
	
	/**
	 * a função é recursiva.
	 * Em entradadas relativamente grandes, a função estava estourando a pilha e a recursão de cauda não estava
	 * funcinonando, esta foi a maneira de fazer com que a função rode com entradas grandes.
	 */
	while(search_matrix(str, str_init_search, token, result));
	
	return result;
 } catch (const std::exception &e) { throw err(e.what()); }
}

template<template<typename> typename T>
T<std::string> 
token::search(const std::string& str,
			  std::vector<token_t> token, T<std::string> result) {
 try {
	long token_idx = -1, token_size = -1; size_t token_str_begin = str.size();
	for(size_t i=0; i < token.size(); ++i) { // search for substring
		auto [str_begin, size] = token[i].search(str);
		if(str_begin < token_str_begin) {
			token_idx = i;
			token_size = size; // size of the token that is first find in str
			token_str_begin = str_begin; // begin of position of token in str
		} else if(str_begin == token_str_begin) {
			if(size > token_size) {
				token_idx = i;
				token_size = size; // size of the token that is first find in str
				token_str_begin = str_begin; // begin of position of token in str
			}
		}
	}
		
	if(token_idx != -1) {
		if(token_str_begin > 0) {
			std::string prev_str = str.substr(0, token_str_begin);
			result.push_back(prev_str);
		//printf("prev_str: \"%s\" | token_str_begin: %ld | \n", prev_str.c_str(), token_str_begin);
		}
		
		if(token[token_idx].type == type_t::column_token) {
			result.push_back(token[token_idx].str);
		}
		
		if(token_str_begin+token_size >= str.size()) { // no more string to search
			return result;
		} else {
			std::string new_str = str.substr(token_str_begin+token_size, std::string::npos);
			return search(new_str, token, result); // continue the search
		}
	} else {
		result.push_back(str);
		return result;
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}

template<template<typename> typename T>
bool
token::search(const std::string& str, 
			  size_t& str_init_search,
			  std::vector<token_t>& token, 
			  T<std::string>& result) {
 try {
	long token_idx = -1, token_size = -1; size_t token_str_begin = str.size();
	for(size_t i=0; i < token.size(); ++i) { // search for substring
		auto [str_begin, size] = token[i].search(str, str_init_search);
		if(str_begin < token_str_begin ||
		  (str_begin == token_str_begin && size > token_size)) {
			token_idx = i;
			token_size = size; // size of the token that is first find in str
			token_str_begin = str_begin; // begin of position of token in str		
		}
	}
		
	if(token_idx != -1) {
		if(token_str_begin > str_init_search) {
			std::string prev_str = str.substr(str_init_search, token_str_begin - str_init_search);
			result.push_back(prev_str);
			//printf("prev_str: \"%s\" | str_init_search: %d | token_str_begin: %ld | \n", prev_str.c_str(), str_init_search, token_str_begin);
		}
			
		if(token[token_idx].type == type_t::column_token) {
			result.push_back(token[token_idx].str);
		}
			
		str_init_search = token_str_begin+token_size;
		if(str_init_search >= str.size()) { // no more string to search
			return false;
		} else {
			return true;
		}
		
	} else {
		result.push_back(str.substr(str_init_search));
		return false;
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}

template<template<typename> typename T, template<typename> typename U>
bool
token::search_matrix(const std::string& str, 
					 size_t& str_init_search,
					 std::vector<token_t>& token, 
					 T<U<std::string>>& result) {
 try {
	////////////////////////////////////////////////////////////////////////////////
	// busca o primeiro token dentro da substring de busca
	////////////////////////////////////////////////////////////////////////////////
	//printf("%d of %d\n", str_init_search, str.size());
	//static int i = 1;
	//printf("[%d] str.size(): %ld | str_init_search: %d | token_str_begin: %ld | result.size(): %ld | result.back().size(): %ld | token_idx: %d | token.size(): %ld | token_size: %ld\n", i, str.size(), str_init_search, token_str_begin, result.size(), result.back().size(), token_idx, token.size(), token_size);
	long token_idx = -1, token_size = -1; size_t token_str_begin = str.size();
	for(size_t i=0; i < token.size(); ++i) { // search for substring
		auto [str_begin, size] = token[i].search(str, str_init_search);
		if(str_begin < token_str_begin ||
		  (str_begin == token_str_begin && size > token_size)) {
			token_idx = i;
			token_size = size; // size of the token that is first find in str
			token_str_begin = str_begin; // begin of position of token in str		
		}
	}
	
	//printf("[%d] str.size(): %ld | str_init_search: %d | token_str_begin: %ld | result.size(): %ld | result.back().size(): %ld | token_idx: %d | token.size(): %ld | token_size: %ld\n", i++, str.size(), str_init_search, token_str_begin, result.size(), result.back().size(), token_idx, token.size(), token_size);
	if(token_idx != -1) { // encontrou algum token str_init_search, token_str_begin, result.back().size());
		if(token_str_begin > str_init_search) { // insere a substring antes do token encontrado
			std::string prev_str = str.substr(str_init_search, token_str_begin - str_init_search);
			result.back().push_back(prev_str);
			//printf("prev_str: \"%s\" | str_init_search: %d | token_str_begin: %ld | \n", prev_str.c_str(), str_init_search, token_str_begin);
		}
		
		////////////////////////////////////////////////////////////////////////////////
		// trata o resultado pelo tipo de token
		////////////////////////////////////////////////////////////////////////////////
		if(token[token_idx].type == type_t::column_token) {
			result.back().push_back(token[token_idx].str);
		} else if(token[token_idx].type == type_t::line_break) {
			result.push_back({}); // insere uma nova linha
		} else if(token[token_idx].type == type_t::line_break_end_token_column) {
			result.back().push_back(token[token_idx].str);
			result.push_back({}); // insere uma nova linha
		} //else { printf("type_t::column_split\n");}// else => token[token_idx].type == type_t::column_split
		
		////////////////////////////////////////////////////////////////////////////////
		// acerta a nova string que será buscada & recomeça a busca
		////////////////////////////////////////////////////////////////////////////////
		//size_t new_str_init_search = token_str_begin+token_size;
		//if(new_str_init_search >= str.size()) { // no more string to search
		str_init_search = token_str_begin+token_size;
		if(str_init_search >= str.size()) { // no more string to search
			return false;
		} else {
			//return search_matrix(str, new_str_init_search, token, result); // continue the search
			return true;
		}
		
	} else { // não encontrou token na busca
		result.back().push_back(str.substr(str_init_search));
		return false;
	}
	
 } catch (const std::exception &e) { throw err(e.what()); }
}
#endif // TOKEN_HPP

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// SQL create database security
////////////////////////////////////////////////////////////////////////////////





























