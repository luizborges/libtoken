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
#include <vector>

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
// show function and test functions
////////////////////////////////////////////////////////////////////////////////
// std::string args_to_str(const std::ostringstream& os) {
//     return os.str();
// }

// template<typename T, typename ... Args>
// std::string args_to_str(std::ostringstream &os, const T val,  const Args ... args) {
//     if constexpr(std::is_pointer<T>::value)  {
//         if(val == nullptr || val == NULL) {
//             os << "";
//         } else {
//             os << val;
//         }
//     } else {
//         os << val;
//     }
    
//     return args_to_str(os, args ...);
// }

// template<typename ... Args>
// void show(const Args ... args) {    
// 	std::ostringstream os;    
// 	std::cout << args_to_str(os, args ...);
// }

// template<typename STR>
// std::string to_vecstr(const STR& str) {
// 	std::string s;

// 	for(size_t i=0; i < str.size(); ++i) {
// 		s += "[" + std::to_string(i) + ",'" + str.at(i) + "'], ";
// 	}

// 	if(s.empty() == false) {
// 		s.pop_back();
// 		s.pop_back();
// 	}

// 	return s;
// }
////////////////////////////////////////////////////////////////////////////////
// end show function
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
      column_split, column_token, line_break, line_break_end_token_column, ignore
    };
	
	////////////////////////////////////////////////////////////////////////////////
	// Class and structs and interfeces
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
		 * @param strs: string em que se realizará a busca do token.
		 * @param str_init_search: posição inicial em @strs que começará a busca pelo token.
		 * @return tuple<@1, @2>:
		 * @1: posição dentro da string do início do token.
		 * Caso não seja encontrado o token retorna: std::string::npos
		 * @2: tamanho do token - this->str.size()
		 * Caso não seja encontrado o token retorna: -1
		 */
        std::tuple<size_t, long> search(const std::string& strs);
        std::tuple<size_t, long> search(const std::string& strs, const long str_init_search);
		std::tuple<size_t, long> search(const std::string_view& strs);
    };

	/**
	 * Interface para representar um token.
	 * O padrão do nome foi escolhido para conformar-se ao padrão que a classe já tinha posto (final _t).
	 * Esta interface é necessária para unificar o tratamento dado para um token, que pode ser tanto um token no sentido padrão (token_t) como um token que representa uma string C/C++, como tokens diversos (tokens em bloco especiais, etc) nesse caso, não compensa representar um token no sentido padrão (token_t), mas deve-se ter uma classe especial para ele, sob o risco de desvirtuar o código e adicionar complexidade excessiva que impossibilita ou dificulta a manutenção do código.
	 * Para fins de padronização com a C++ std, como a interface toma valores em std::string_view, os tipos e tamanhos foram setados para std::string_view::size_type. -> geralmente é size_t, mas para ficar mais genérico foi adotado o tipo abstrado.
	 */
	struct token_interface_t {
		/**
		 * A escolha do nome foi para conformar com do padrão da standard library C++ std::string::find
		 * @param str: string with the subject to search for
		 * @param str_pos_init: Position of the first character in the string to be considered in the search.
		 * If this is greater than the string length, the function never finds matches.
		 * Note: The first character is denoted by a value of 0 (not 1): A value of 0 means that the entire string is searched.
		 * @param str_count: Length of sequence of characters to match.
		 * @obs: the search string will be the string resulting from the following operation: 
		 * real_str_search = str.substr(str_pos_init, str_len);
		 * @return tuple<@1, @2>:
		 * @1: posição dentro da string do início do token.
		 * Caso não seja encontrado o token retorna: std::string::npos
		 * @2: tamanho do token buscado (token, tamanho da string C/C++, etc...)
		 * Caso não seja encontrado o token retorna: 0
		 */
		virtual std::tuple<size_t, size_t> find(const std::string_view& str, 
			const std::string_view::size_type str_pos_init = 0, 
			const std::string_view::size_type str_count = std::string_view::npos) {
        	std::cerr << "NO IMPLEMENTED YET. - find::("<<str<<str_pos_init<<str_count<<")\n";
        	return std::make_tuple(std::string_view::npos, 0);
    	}
    
		/**
		 * Return the the type of the token.
		 */
    	virtual type_t type() const {
        	std::cerr << "NO IMPLEMENTED YET.\n";
        	return type_t::ignore;
    	}

		/**
		 * Returns the value of the token.
		 * Como o token geralmente é guardado por uma string_view, por questões de eficiência, essa função retorna uma string_view dele.
		 * @obs: Cuidado ao lidar com esta função, já que a string_view é uma referência para uma string/char* de fato.
		 * @obs: Caso deseje alterar o token que foi passado, e utilizá-lo fora da biblioteca token, utilizar a função str().
		 */
		virtual std::string_view str_view() const {
			std::cerr << "NO IMPLEMENTED YET.\n";
        	exit(1);
		}

		/**
		 * Returns the string that represents the token.
		 */
		virtual std::string str() const {
			std::cerr << "NO IMPLEMENTED YET.\n";
        	return "";
		}
	};


	/**
	 * Guarda o token passado pelo usuário e verifica se existe o token na string passada.
	 * Cada token passado pelo usuário é criado uma struct deste tipo.
	 */
	class token2_t : public token_interface_t {
	 private:
        std::string_view _str_;
        type_t _type_ = type_t::ignore;
        bool _never_match_ = false; // é utilizado quando se verifica pela primeira vez que a string não contém mais o token

		/**
		 * Serve para guardar a posição inicial do token na string de busca.
		 * O valor inicial é colocado em std::string::npos para simbolizar que a busca
		 * ainda não foi feita.
		 * Feito para otimizar o processo.
		 */
		size_t _pos_ = std::string::npos; // position of the token when found
	 public:
		/**
		 * Template para ser inicializado tanto como std::string e std::string_view.
		 */
		template<typename STR>
        token2_t(const STR& str, const type_t type) {
            _str_ = str;
            _type_ = type;
        }

		std::tuple<std::string_view::size_type, std::string_view::size_type> 
		find(const std::string_view& str, 
			const std::string_view::size_type str_pos_init = 0, 
			const std::string_view::size_type str_count = std::string_view::npos);
        
        type_t type() const {
        	return _type_;
    	}

		std::string_view str_view() const {
			return _str_;
		}

		std::string str() const {
			return std::string(_str_);
		}
    };

	class token_str_t : public token_interface_t {
	 private:
	 	std::string_view _str_;
		type_t _type_;
        bool _never_match_ = false; // é utilizado quando se verifica pela primeira vez que a string não contém mais o token

		/**
		 * Serve para guardar a posição inicial do token na string de busca.
		 * O valor inicial é colocado em std::string_view::npos para simbolizar que a busca
		 * ainda não foi feita.
		 * Feito para otimizar o processo.
		 */
		std::string_view::size_type _pos_ = std::string_view::npos; // position of the token when found

	 public:
	 	token_str_t(const type_t type) {
			this->_type_ = type;
		}

		type_t type() const {
        	return _type_;
    	}

		std::string_view str_view() const {
			return _str_;
		}

		std::string str() const {
			return std::string(_str_);
		}

		/**
		 * Implementing the token_interface_t::find().
		 * Search for the string C/C++ with the function token::find_str()
		 * Busca se existe uma string C/C++ dentro da string passada.
		 * A busca é feita pela função token::find_str();
		 * @exception caso haja erro, ou seja, a string não está completa, é retornado uma exceção. 
		 * @param STR: deve representar uma string em que será realizada a busca, os tipos de STR podem ser tanto std::string como std::string_view.
		 * @param strs: string em que se realizará a busca da string C/C++.
		 * @param str_init_search: posição inicial em @strs que começará a busca da string C/C++.
		 * @return tuple<@1, @2>:
		 * @1: posição dentro da string do início da string C/C++.
		 * Caso não seja encontrado a string C/C++ retorna: std::string::npos
		 * @2: tamanho da string C/C++ -> como retornado pela função token::find_str()
		 * Caso não seja encontrado a string C/C++ retorna: 0
		 */
		std::tuple<std::string_view::size_type, std::string_view::size_type> 
		find(const std::string_view& str, 
			const std::string_view::size_type str_pos_init = 0, 
			const std::string_view::size_type str_count = std::string_view::npos);
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
        				std::vector<token_t> token, 
						T<std::string> result);
    
	/**
	 * @param str_init_search: posição inicial da qual será analisada a string @str.
	 * @return: true: se a busca ainda não finalizou - deve-se chamar a função novamente.
	 * false: se a busca terminou - ou seja, toda a string já foi coberta pela função.
	 */
	template<template<typename> typename T>
    bool search(const std::string& str, 
				size_t& str_init_search,
				std::vector<token_t>& token, 
				T<std::string>& result);
	
	/**
	 * Nova versão da função search.
	 * Utiliza uma string_view para maior eficiência.
	 * Retira o número excessivo de cópias de result.
	*/
	template<template<typename> typename T>
	bool search(std::string_view& str,
				std::vector<token_t>& token,
				T<std::string> &result);

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
	// Funções que realmente devem ser chamadas pelo usuário.
	////////////////////////////////////////////////////////////////////////////////
	/**
	 * Função realmente que deve ser chamada pelo usuário.
	 * @param str: string em que será realizado a busca pelo token.
	 * @param column_split: vector que contém os tokens que "quebram", "partem", a string em diversas colunas
	 * os tokens deste vetor não entram no resultado final da string.
	 * @param column_token: vector que contém os tokens que "quebram", "partem", a string em diversas colunas
	 * os tokens deste vector entram no resultado final da string, cada token deste se torna uma posição
	 * do vetor. A quantidade deste token no vector final será o número de vezes que ele aparecerá na string @str
	 * @param begin: posição de início das buscas.
	 * @param count: Número de characteres que será buscado após o índice passado em @param(begin).
	 * Segue o padrão da função std::string::substr().
	 * O parâmetro count significa quantos characteres serão buscados, considerados desde a posição begin
	 * @return vector que contém a string dividida segundo os tokens passados na função. Cada token produz uma nova
	 * posição no vector final, embora o token possa entrar no vector final (@column_token) ou não (@column_split)
	 */
    template<template<typename> typename T>
    T<std::string> get(const std::string& str, 
        const std::vector<std::string>& column_split, 
        const std::vector<std::string>& column_token = {},
		const size_t begin = 0,
		const size_t count = std::string::npos);
	
	template<template<typename> typename T>
    T<std::string> get(const std::string_view& str, 
        const std::vector<std::string>& column_split, 
        const std::vector<std::string>& column_token = {},
		const size_t begin = 0,
		const size_t count = std::string::npos);
	
	/**
	 * Função realmente que deve ser chamada pelo usuário.
	 * Exemplo de como usar essa função:
	 * auto excel = token::get_matrix<std::deque, std::vector>(excel_untreated, {"\t"}, {}, {"\n", "\r"});
	 * excel será do tipo std::deque<std::vector<std::string>>
	 * @param str: string em que será realizado a busca pelo token.
	 * @param column_split: vector que contém os tokens que "quebram", "partem", a string em diversas colunas
	 * os tokens deste vetor não entram no resultado final da string.
	 * @param column_token: vector que contém os tokens que "quebram", "partem", a string em diversas colunas
	 * os tokens deste vector entram no resultado final da string, cada token deste se torna uma posição
	 * do vetor. A quantidade deste token no vector final será o número de vezes que ele aparecerá na string @str
	 * @param line_split: funciona identico ao @column_split, com a diferença que os próximos resultados
	 * serão colocados na linha subsequente da matrix, ou seja, se os resultados eram colocados na linha i
	 * os próximos serão colocados na linha i+1
	 * Este token não integra o resultado.
	 * @param line_token: funciona identico ao @column_token, com a diferença que os próximos resultados
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
			   const std::vector<std::string>& line_token = {},
			   const size_t begin = 0,
			   const size_t count = std::string::npos);
	
	template<template<typename> typename T, template<typename> typename U>
	T<U<std::string>> get_matrix(const std::string_view& str, 
			   const std::vector<std::string>& column_split, 
			   const std::vector<std::string>& column_token = {}, 
			   const std::vector<std::string>& line_split = {}, 
			   const std::vector<std::string>& line_token = {},
			   const size_t begin = 0,
			   const size_t count = std::string::npos);
	
	/**
	 * ADAPTAR O TEXTO PARA A NOVA FUNÇÃO!
	 * Função realmente que deve ser chamada pelo usuário.
	 * Exemplo de como usar essa função:
	 * auto excel = token::get_matrix<std::deque, std::vector>(excel_untreated, {"\t"}, {}, {"\n", "\r"});
	 * excel será do tipo std::deque<std::vector<std::string>>
	 * @param STR: pode ser do tipo std::string ou std::string_view ou tipo equivalente.
	 * @param str: string em que será realizado a busca pelo token.
	 * @param column_split: vector que contém os tokens que "quebram", "partem", a string em diversas colunas
	 * os tokens deste vetor não entram no resultado final da string.
	 * @param column_token: vector que contém os tokens que "quebram", "partem", a string em diversas colunas
	 * os tokens deste vector entram no resultado final da string, cada token deste se torna uma posição
	 * do vetor. A quantidade deste token no vector final será o número de vezes que ele aparecerá na string @str
	 * @param line_split: funciona identico ao @column_split, com a diferença que os próximos resultados
	 * serão colocados na linha subsequente da matrix, ou seja, se os resultados eram colocados na linha i
	 * os próximos serão colocados na linha i+1
	 * Este token não integra o resultado.
	 * @param line_token: funciona identico ao @column_token, com a diferença que os próximos resultados
	 * serão colocados na linha subsequente da matrix, ou seja, se os resultados eram colocados na linha i
	 * os próximos serão colocados na linha i+1
	 * Este token integra o resultado, e é sempre o último token da linha, ou seja, da linha i -> linha nova i+1
	 * que este token produz ao final.
	 * @param escstr_type: define como será tratado as strings do tipo C/C++ encontradas. As strings C/C++ podem ser tratadas como um dos tipos token::type_t. Elas na função serão tratadas como um tipo especial de token, tanto para busca como no modo de compor o resultado final da função.
	 * @return matrix que contém a string dividida segundo os tokens passados na função. 
	 * Cada token produz uma nova posição no matrix final:
	 * (@line_split e @line_token) produzem uma nova linha na matrix e 
	 * (@column_split e @column_token) não produzem uma nova linha na matrix, apenas afetam a linha atual da matrix
	 * Embora o token possa entrar na matrix (@column_token e @line_token) ou não (@column_split e @line_split)
	 */
	template<typename MATRIX_STR, typename STR>
	MATRIX_STR get_matrix_escstr(const STR& str, 
			   const std::vector<std::string>& column_split, 
			   const std::vector<std::string>& column_token = {}, 
			   const std::vector<std::string>& line_split = {}, 
			   const std::vector<std::string>& line_token = {},
			   const type_t escstr_type = type_t::column_token,
			   const size_t begin = 0,
			   const size_t count = std::string_view::npos);
	
	template<typename MATRIX_STR>
	bool
	search_matrix_escstr(
		std::string_view& str,
		std::string_view::size_type& pos_init_search,
		std::vector<std::unique_ptr<token::token_interface_t>>& token,
		MATRIX_STR& result);

	/**
	 * @brief Busca se dentro de uma string contém uma uma string do tipo C/C++
	 * As strings procuradas são as que começam com os characteres: '"'.
	 * É válido os characteres de escape. \\ \" para escapar tanto o começo, quanto o fim da string.
	 * Ao final, retorna se encontrou ou não uma string, e a posição de começo e de fim da string.
	 * @obs: a string em que será buscada será @param(str).substr(@param(begin), @param(count))
	 * ou seja, uma substring definida pelos parâmetros begin e count.
	 * @obs: os valores dentro da @param(str) fora dos parâmetros passados, são totalmente ignorados pelo programa.
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
	 * str: "01\"4\""" -> begin = 3 -> count = 8
	 * return: tuple<true, 3, 5>
	 * @obs: se str.empty() == true -> return tuple<false, std::string::npos, std::string::npos>
	 * @throw u::error: se count = 0, nenhuma busca será feita.
	 * @throw:u::error: se begin >= str.size().
	 * @param STR: pode ser do tipo std::string ou std::string_view ou tipo equivalente.
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
	template<typename STR>
	std::tuple<bool, size_t, size_t>
	find_str(const STR& str, 
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
						  const std::vector<std::string>& column_token,
						  const size_t begin,
						  const size_t count) {
 try {
    const std::string_view strv = str;
	return get<T>(strv, column_split, column_token, begin, count);
 } catch (const std::exception &e) { throw erx(e.what()); }
}

template<template<typename> typename T>
T<std::string> token::get(const std::string_view& str, 
						  const std::vector<std::string>& column_split, 
						  const std::vector<std::string>& column_token,
						  const size_t begin,
						  const size_t count) {
 try {
	////////////////////////////////////////////////////////////////////////////////
	// Check input
	////////////////////////////////////////////////////////////////////////////////
	if(str.empty()) return {};
	if(begin >= str.size()) throw erx("Begin position of search in string is greater or equal than string size. Begin: ",begin,", String size: ",str.size(),", String: '",str,"'");
	if(count <= 0) return {}; // garante que a saída terá ao menos 1 character

	////////////////////////////////////////////////////////////////////////////////
	// init
	////////////////////////////////////////////////////////////////////////////////
    std::vector<token_t> token;
    for(const auto& s : column_split) { 
		if(!s.empty()) token.push_back({s, type_t::column_split});
	}
    for(const auto& s : column_token) { 
		if(!s.empty()) token.push_back({s, type_t::column_token});
	}
        
    T<std::string> result = {};
	std::string_view strv = str; // linha é necessário, caso contrário, é criado uma nova string pelo str.substr().
	strv = strv.substr(begin, count); // apenas trabalha dentro do range dado pelo usuário.
	
	/**
	 * a função é recursiva.
	 * Em entradadas relativamente grandes, a função estava estourando a pilha e a recursão de cauda não estava
	 * funcinonando, esta foi a maneira de fazer com que a função rode com entradas grandes.
	 */
	while(search(strv, token, result));
	return result;
 } catch (const std::exception &e) { throw erx(e.what()); }
}


template<template<typename> typename T, template<typename> typename U>
T<U<std::string>> token::get_matrix(const std::string& str, 
									const std::vector<std::string>& column_split,
									const std::vector<std::string>& column_token, 
									const std::vector<std::string>& line_split,
									const std::vector<std::string>& line_token,
									const size_t begin,
			   						const size_t count) {
 try {
    const std::string_view strv = str;
	return get_matrix<T, U>(strv, column_split, column_token, line_split, line_token, begin, count);
 } catch (const std::exception &e) { throw erx(e.what()); }
}

template<template<typename> typename T, template<typename> typename U>
T<U<std::string>> token::get_matrix(const std::string_view& str, 
									const std::vector<std::string>& column_split,
									const std::vector<std::string>& column_token, 
									const std::vector<std::string>& line_split,
									const std::vector<std::string>& line_token,
									const size_t begin,
			   						const size_t count) {
 try {
	////////////////////////////////////////////////////////////////////////////////
	// Check input
	////////////////////////////////////////////////////////////////////////////////
	if(str.empty()) return {};
	if(begin >= str.size()) throw erx("Begin position of search in string is greater or equal than string size. Begin: ",begin,", String size: ",str.size(),", String: '",str,"'");
	if(count <= 0) return {}; // garante que a saída terá ao menos 1 character

	////////////////////////////////////////////////////////////////////////////////
	// init
	////////////////////////////////////////////////////////////////////////////////
    std::vector<token_t> token;
    for(const auto& s : column_split) { if(!s.empty()) token.push_back({s, type_t::column_split}); }
    for(const auto& s : column_token) { if(!s.empty()) token.push_back({s, type_t::column_token}); }
    for(const auto& s : line_split)   { if(!s.empty()) token.push_back({s, type_t::line_break}); }
    for(const auto& s : line_token)   { if(!s.empty()) token.push_back({s, type_t::line_break_end_token_column}); }
        
    T<U<std::string>> result = {};
	result.push_back({}); // inicializa a primeira linha da matrix
	std::string_view strv = str; // linha é necessário, caso contrário, é criado uma nova string pelo str.substr().
	strv = strv.substr(begin, count); // apenas trabalha dentro do range dado pelo usuário.
	
	/**
	 * a função é recursiva.
	 * Em entradadas relativamente grandes, a função estava estourando a pilha e a recursão de cauda não estava
	 * funcinonando, esta foi a maneira de fazer com que a função rode com entradas grandes.
	 */
	while(search_matrix(strv, token, result));

	// verifica se a última linha da matrix contém algum valor e é válida
	// isto se deve pelo algoritmo que cria sempre uma nova linha após o token de criação de nova linha.
	// assim é necessário verificar se a última linha é válida
	// por questões de eficiência e simplificação do código é melhor deixar essa verificação para o final
	if(result.back().empty()) result.pop_back(); // removes the last element of a vector or similar
	
	return result;
 } catch (const std::exception &e) { throw err(e.what()); }
}

template<template<typename> typename T>
T<std::string> 
token::search(const std::string& str,
			  std::vector<token_t> token, 
			  T<std::string> result) {
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
token::search(std::string_view& str,
			  std::vector<token_t>& token, 
			  T<std::string>& result) {
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
			auto prev_str = str.substr(0, token_str_begin);
			result.push_back(std::string(prev_str));
		//printf("prev_str: \"%s\" | token_str_begin: %ld | \n", prev_str.c_str(), token_str_begin);
		}
		
		if(token[token_idx].type == type_t::column_token) {
			result.push_back(token[token_idx].str);
		}
		
		if(token_str_begin+token_size >= str.size()) { // no more string to search
			//return result; // nothing to do more
			return false; // nothing to do more
		} else {
			str = str.substr(token_str_begin+token_size, std::string::npos);
			return true;
		}
	} else {
		result.push_back(std::string(str));
		return false; // nothing to do more
	}
 } catch (const std::exception &e) { throw erx(e.what()); }
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
token::search_matrix(std::string_view& str,
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
		auto [str_begin, size] = token[i].search(str);
		if(str_begin < token_str_begin ||
		  (str_begin == token_str_begin && size > token_size)) {
			token_idx = i;
			token_size = size; // size of the token that is first find in str
			token_str_begin = str_begin; // begin of position of token in str		
		}
	}
	
	//printf("[%d] str.size(): %ld | str_init_search: %d | token_str_begin: %ld | result.size(): %ld | result.back().size(): %ld | token_idx: %d | token.size(): %ld | token_size: %ld\n", i++, str.size(), str_init_search, token_str_begin, result.size(), result.back().size(), token_idx, token.size(), token_size);
	if(token_idx != -1) { // encontrou algum token str_init_search, token_str_begin, result.back().size());
		if(token_str_begin > 0) { // insere a substring antes do token encontrado
			const std::string prev_str = std::string(str.substr(0, token_str_begin));
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
		const auto new_str_init_search = token_str_begin+token_size;
		if(new_str_init_search >= str.size()) { // no more string to search
			return false;
		} else {
			//return search_matrix(str, new_str_init_search, token, result); // continue the search
			str = str.substr(new_str_init_search); // atualiza a string de busca
			return true;
		}
		
	} else { // não encontrou token na busca
		result.back().push_back(std::string(str));
		return false;
	}
	
 } catch (const std::exception &e) { throw erx(e.what()); }
}


template<typename MATRIX_STR, typename STR>
MATRIX_STR token::get_matrix_escstr(const STR& str, 
	const std::vector<std::string>& column_split, 
	const std::vector<std::string>& column_token,
	const std::vector<std::string>& line_split,
	const std::vector<std::string>& line_token,
	const token::type_t escstr_type,
	const size_t begin,
	const size_t count)
{
 try {
	//////////////////////////////////////////////////////////////////////////////
	// Check input
	////////////////////////////////////////////////////////////////////////////////
	if(str.empty()) return {};
	if(begin >= str.size()) { throw erx("Begin position of search in string is greater or equal than string size. Begin: ",begin,", String size: ",str.size(),", String: '",str,"'"); }
	if(count <= 0) return {}; // garante que a saída terá ao menos 1 character

	//////////////////////////////////////////////////////////////////////////////
	// init parameters
	////////////////////////////////////////////////////////////////////////////////
	// if constexpr (std::is_same<STR, std::string>::value) {
	// 	// do something
	// }
	// else if constexpr (std::is_same<STR, std::string_view>::value) {
	// 	// do something
	// }
	// else {
	// 	// error
	// }

	/**
	 * Necessário pois o tipo token_interface_t será uma interface, logo para agir como uma interface
	 * deve ser pointer or reference.
	 */
    std::vector<std::unique_ptr<token_interface_t>> token;
    for(const auto& s : column_split) { 
		if(!s.empty()) token.push_back(std::make_unique<token2_t>(s, type_t::column_split)); 
	}
    for(const auto& s : column_token) { 
		if(!s.empty()) token.push_back(std::make_unique<token2_t>(s, type_t::column_token));
	}
    for(const auto& s : line_split) { 
		if(!s.empty()) token.push_back(std::make_unique<token2_t>(s, type_t::line_break));
	}
    for(const auto& s : line_token) {
		if(!s.empty()) token.push_back(std::make_unique<token2_t>(s, type_t::line_break_end_token_column));
	}

	token.push_back(std::make_unique<token_str_t>(escstr_type)); // insert the action to be performed with C/C++ string format
        
    MATRIX_STR result = {};
	result.push_back({}); // inicializa a primeira linha da matrix
	std::string_view strv = str; // linha é necessário, caso contrário, é criado uma nova string pelo str.substr().
	strv = strv.substr(begin, count); // apenas trabalha dentro do range dado pelo usuário.
	std::string_view::size_type pos_init_search = 0; // posição inicial que será usada para procurar os tokens. -> é necessário para otimização, para que em cada iteração não seja feito uma nova busca no std::string::find e funções correlatas
	
	/**
	 * a função é recursiva.
	 * Em entradadas relativamente grandes, a função estava estourando a pilha e a recursão de cauda não estava
	 * funcinonando, esta foi a maneira de fazer com que a função rode com entradas grandes.
	 */
	while(search_matrix_escstr(strv, pos_init_search, token, result));

	// verifica se a última linha da matrix contém algum valor e é válida
	// isto se deve pelo algoritmo que cria sempre uma nova linha após o token de criação de nova linha.
	// assim é necessário verificar se a última linha é válida
	// por questões de eficiência e simplificação do código é melhor deixar essa verificação para o final
	if(result.back().empty()) result.pop_back(); // removes the last element of a vector or similar
	
	return result;
 } catch (const std::exception &e) { throw erx(e.what()); }
}


template<typename MATRIX_STR>
bool
token::search_matrix_escstr(
	std::string_view& str,
	std::string_view::size_type& pos_init_search,
	std::vector<std::unique_ptr<token::token_interface_t>>& token,
	MATRIX_STR& result)
{
 try {
	////////////////////////////////////////////////////////////////////////////////
	// busca o primeiro token dentro da substring de busca
	////////////////////////////////////////////////////////////////////////////////
	// printf("%d of %d\n", str_init_search, str.size());
	// static int ii = 1;
	// printf("[%d] str.size(): %ld | str_init_search: %d | token_str_begin: %ld | result.size(): %ld | result.back().size(): %ld | token_idx: %d | token.size(): %ld | token_size: %ld\n", i, str.size(), str_init_search, token_str_begin, result.size(), result.back().size(), token_idx, token.size(), token_size);
	std::string_view::size_type token_idx = std::string_view::npos;
	std::string_view::size_type token_size = 0; 
	std::string_view::size_type token_str_begin = std::string::npos;
	for(std::string_view::size_type i=0; i < token.size(); ++i) { // search for substring
		auto [str_begin, size] = token.at(i)->find(str, pos_init_search);
		// show("i: ",i," | str_begin: ",str_begin," | size: ",size," | token: '",token.at(i)->str_view(),"'\n");
		if(str_begin < token_str_begin ||
		  (str_begin == token_str_begin && size > token_size)) {
			token_idx = i;
			token_size = size; // size of the token that is first find in str
			token_str_begin = str_begin; // begin of position of token in str		
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// realiza o partimento da substring
	////////////////////////////////////////////////////////////////////////////////
	
	// printf("[%d] str.size(): %ld | str_init_search: %d | token_str_begin: %ld | result.size(): %ld | result.back().size(): %ld | token_idx: %d | token.size(): %ld | token_size: %ld\n", i++, str.size(), str_init_search, token_str_begin, result.size(), result.back().size(), token_idx, token.size(), token_size);

	// show("[",++ii,"]: str: ",str," | pos_init_search: ",pos_init_search," | token_idx: ",token_idx," | token_size: ",token_size," | token_str_begin: ",token_str_begin," | token.size(): ",token.size(),"\n");

	if(token_idx != std::string_view::npos) { // encontrou algum token str_init_search, token_str_begin, result.back().size());
		if(token_str_begin > pos_init_search) { // insere a substring antes do token encontrado
			const auto prev_strv = str.substr(pos_init_search, token_str_begin - pos_init_search);
			const auto prev_str = std::string(prev_strv);
			// show(__func__,"::",__LINE__,":: prev_str: '",prev_str,"'\n");
			result.back().push_back(prev_str);
			//printf("prev_str: \"%s\" | str_init_search: %d | token_str_begin: %ld | \n", prev_str.c_str(), str_init_search, token_str_begin);
		}
		
		////////////////////////////////////////////////////////////////////////////////
		// trata o resultado pelo tipo de token
		////////////////////////////////////////////////////////////////////////////////
		const auto token_type = token.at(token_idx)->type();
		const auto token_strv = token.at(token_idx)->str_view();
		if(token_type == type_t::column_token) {
			result.back().push_back(std::string(token_strv));
		} else if(token_type == type_t::line_break) {
			result.push_back({}); // insere uma nova linha
		} else if(token_type == type_t::line_break_end_token_column) {
			result.back().push_back(std::string(token_strv));
			result.push_back({}); // insere uma nova linha
		} else if(token_type == type_t::ignore) {
			auto& prev_str = result.back().back();
			prev_str = prev_str + std::string(token_strv);
		}
		//else { printf("type_t::column_split\n");}// else => token[token_idx].type == type_t::column_split
		
		////////////////////////////////////////////////////////////////////////////////
		// acerta a nova string que será buscada & recomeça a busca
		////////////////////////////////////////////////////////////////////////////////
		//size_t new_str_init_search = token_str_begin+token_size;
		//if(new_str_init_search >= str.size()) { // no more string to search
		const auto new_str_init_search = token_str_begin+token_size;
		if(new_str_init_search >= str.size()) { // no more string to search
			return false;
		} else {
			//return search_matrix(str, new_str_init_search, token, result); // continue the search
			// str = str.substr(new_str_init_search); // atualiza a string de busca
			pos_init_search = new_str_init_search; // atualiza o novo começo de buscas para a string
			return true;
		}
		
	} else { // não encontrou token na busca
		result.back().push_back(std::string(str.substr(pos_init_search)));
		return false;
	}
	
 } catch (const std::exception &e) { throw erx(e.what()); }
}

template<typename STR>
std::tuple<bool, size_t, size_t>
token::find_str(const STR& str, const size_t begin, const size_t count)
{ try {
	////////////////////////////////////////////////////////////////////////////////
	// check template type argument
	////////////////////////////////////////////////////////////////////////////////
	// if constexpr (std::is_same<STR, std::string>::value) {
	// 	// ok
	// }
	// else if constexpr(std::is_same<STR, std::string_view>::value) {
	// 	// ok
	// } else {
	// 	// error ?
	// }

	////////////////////////////////////////////////////////////////////////////////
	// Check input
	////////////////////////////////////////////////////////////////////////////////
	if(str.empty()) return { false, std::string::npos, 0 };
	// if(begin >= str.size()) throw erx("Begin position of search in string is greater or equal than string size. Begin: %zu, String size: %zu", begin, str.size());
	if(begin >= str.size()) throw erx("Begin position of search in string is greater or equal than string size. Begin: ",begin,", String size: ",str.size(),", String: '",str,"'");
	// if(count > 2) throw erx("Count is less than 2. Number of characters to be searched must be greater than 2. Count: %zu", count);

	std::string_view strv { str };
	strv = strv.substr(begin ,count); // deixa o código mais legível para fazer os cálculos de retorno

	////////////////////////////////////////////////////////////////////////////////
	// procura o início de uma string '"'
	////////////////////////////////////////////////////////////////////////////////
	bool searching = true;
	size_t begin_search = 0;
	size_t begin_str = 0;
	while(searching) {
		begin_str = strv.find("\"", begin_search);
		if(begin_str == std::string::npos) return { false, std::string::npos, 0 };// não foi encontrado
		if(begin_str == begin_search) break; // não precisa verificar pois é o primeiro character da string para buscar

		bool is_str = true;
		for(auto check_begin_str = begin_str -1; check_begin_str >= begin_search && check_begin_str < begin_str; --check_begin_str)
		{
			// std::cout << "check_begin_str: " << check_begin_str << ", begin_str: " << begin_str << ", begin_search: " << begin_search << "\n";
			if(strv.at(check_begin_str) != '\\') {
				if(is_str) {
					searching = false;
					break;
				} else break;
			} else {
				is_str = !is_str; // verifica recursivamente se os characteres transformam a string em uma string real ou não.
			}
		}

		if(is_str) searching = false; // para o caso em que todos os characteres antecessores buscados são '\\'
		else begin_search = begin_str +1; // atualiza o valor da busca

		if(begin_search >= strv.size()) return { false, std::string::npos, 0 }; // verifica se ainda tem string para buscar
	}

	// show("strv: '",strv,"' | begin_str: ",begin_str,"\n");
	////////////////////////////////////////////////////////////////////////////////
	// procura o fim de uma string '"'
	////////////////////////////////////////////////////////////////////////////////
	searching = true;
	begin_search = begin_str +1;
	auto end_str = std::string::npos;
	while(searching) {
		end_str = strv.find("\"", begin_search);
		// show("end_str: ",end_str,"\n");
		if(end_str == std::string::npos) return { false, begin_str + begin, strv.size()-begin_str }; // não foi encontrado

		bool is_str = true;
		for(auto check_end_str = end_str -1; check_end_str >= begin_search && check_end_str < end_str; --check_end_str)
		{
			if(strv.at(check_end_str) != '\\') {
				if(is_str) {
					searching = false;
					break;
				} else break;
			} else {
				is_str = !is_str; // verifica recursivamente se os characteres transformam a string em uma string real ou não.
			}
		}
		// std::cout << "strv: \"" << strv << "\", size: " << strv.size() << ", begin_search: " << begin_search << ", end_str: " << end_str << ", is_str: "<< borges::util::to_str(is_str) << ", searching: " << borges::util::to_str(searching) << "\n";
		if(is_str) searching = false; // para o caso em que todos os characteres antecessores buscados são '\\'
		else begin_search = end_str +1; // atualiza o índice de início das buscas

		// std::cout << "strv: \"" << strv << "\", size: " << strv.size() << ", begin_search: " << begin_search << ", end_str: " << end_str << ", is_str: "<< borges::util::to_str(is_str) << ", searching: " << borges::util::to_str(searching) << "\n";

		if(begin_search >= strv.size()) return { false, begin_str + begin, strv.size()-begin_str }; // não foi encontrado o final
	}
	
	return { true, begin_str + begin, end_str-begin_str+1 }; // end_str-begin_str+begin+1 = o tamanho da string encontrada, contados os characteres '"' de início e fim da string
 } catch (const std::exception &e) { throw erx(e.what()); }
}


#endif // TOKEN_HPP

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// SQL create database security
////////////////////////////////////////////////////////////////////////////////





























