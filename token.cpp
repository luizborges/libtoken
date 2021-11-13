#include "token.hpp"

////////////////////////////////////////////////////////////////////////////////
// Init global variables
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Global Functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Functions of struct token
////////////////////////////////////////////////////////////////////////////////
std::tuple<size_t, long> 
token::token_t::search(const std::string& strs) {
 try {
	if(never_match == true) { return std::make_tuple(std::string::npos, -1); }
			
	size_t pos = strs.find(str);
	if(pos == std::string::npos) {
		never_match = true;
		return std::make_tuple(std::string::npos, -1);
	} else {
		return std::make_tuple(pos, str.size());
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}
		
std::tuple<size_t, long> 
token::token_t::search(const std::string& strs, const long str_init_search) {
 try {
	if(never_match == true) return { std::string::npos, -1 };
	
	size_t pos = strs.find(str, str_init_search);
	if(pos == std::string::npos) {
		never_match = true;
		return {std::string::npos, -1};
	} else {
		return { pos, str.size() };
	}
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::tuple<bool, size_t, size_t>
token::find_str(const std::string& str, const size_t begin, const size_t count)
{ try {
	////////////////////////////////////////////////////////////////////////////////
	// Check input
	////////////////////////////////////////////////////////////////////////////////
	if(str.empty()) return { false, std::string::npos, 0 };
	if(begin >= str.size()) throw err("Begin position of search in string is greater or equal than string size. Begin: %zu, String size: %zu", begin, str.size());
	// if(count > 2) throw err("Count is less than 2. Number of characters to be searched must be greater than 2. Count: %zu", count);

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
			if(str.at(check_begin_str) != '\\') {
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

	////////////////////////////////////////////////////////////////////////////////
	// procura o fim de uma string '"'
	////////////////////////////////////////////////////////////////////////////////
	searching = true;
	begin_search = begin_str +1;
	auto end_str = std::string::npos;
	while(searching) {
		end_str = strv.find("\"", begin_search);
		if(end_str == std::string::npos) return { false, begin_str + begin, strv.size()-begin_str }; // não foi encontrado

		bool is_str = true;
		for(auto check_end_str = end_str -1; check_end_str > begin_search && check_end_str < end_str; --check_end_str)
		{
			if(str.at(check_end_str) != '\\') {
				if(is_str) {
					searching = false;
					break;
				} else break;
			} else {
				is_str = !is_str; // verifica recursivamente se os characteres transformam a string em uma string real ou não.
			}
		}
		// std::cout << "strv: \"" << strv << "\", size: " << strv.size() << ", begin_search: " << begin_search << ", end_str: " << end_str << "\n";
		if(is_str) searching = false; // para o caso em que todos os characteres antecessores buscados são '\\'
		else begin_search = end_str +1; // atualiza o índice de início das buscas

		if(begin_search >= strv.size()) return { false, begin_str + begin, strv.size()-begin_str }; // não foi encontrado o final
	}
	
	return { true, begin_str + begin, end_str-begin_str+1 }; // end_str-begin_str+begin+1 = o tamanho da string encontrada, contados os characteres '"' de início e fim da string
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::tuple<bool, size_t, size_t>
token::find_str(const std::string_view& str, const size_t begin, const size_t count)
{ try {
	////////////////////////////////////////////////////////////////////////////////
	// Check input
	////////////////////////////////////////////////////////////////////////////////
	if(str.empty()) return { false, std::string::npos, 0 };
	if(begin >= str.size()) throw err("Begin position of search in string is greater or equal than string size. Begin: %zu, String size: %zu", begin, str.size());
	// if(count > 2) throw err("Count is less than 2. Number of characters to be searched must be greater than 2. Count: %zu", count);

	std::string_view strv { str };
	strv = strv.substr(begin, count); // deixa o código mais legível para fazer os cálculos de retorno

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
			if(str.at(check_begin_str) != '\\') {
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

	////////////////////////////////////////////////////////////////////////////////
	// procura o fim de uma string '"'
	////////////////////////////////////////////////////////////////////////////////
	searching = true;
	begin_search = begin_str +1;
	auto end_str = std::string::npos;
	while(searching) {
		end_str = strv.find("\"", begin_search);
		if(end_str == std::string::npos) return { false, begin_str + begin, strv.size()-begin_str }; // não foi encontrado

		bool is_str = true;
		for(auto check_end_str = end_str -1; check_end_str > begin_search && check_end_str < end_str; --check_end_str)
		{
			if(str.at(check_end_str) != '\\') {
				if(is_str) {
					searching = false;
					break;
				} else break;
			} else {
				is_str = !is_str; // verifica recursivamente se os characteres transformam a string em uma string real ou não.
			}
		}
		// std::cout << "strv: \"" << strv << "\", size: " << strv.size() << ", begin_search: " << begin_search << ", end_str: " << end_str << "\n";
		if(is_str) searching = false; // para o caso em que todos os characteres antecessores buscados são '\\'
		else begin_search = end_str +1; // atualiza o índice de início das buscas

		if(begin_search >= strv.size()) return { false, begin_str + begin, strv.size()-begin_str }; // não foi encontrado o final
	}
	
	return { true, begin_str + begin, end_str-begin_str+1 }; // end_str-begin_str+begin+1 = o tamanho da string encontrada, contados os characteres '"' de início e fim da string
 } catch (const std::exception &e) { throw err(e.what()); }
}


std::tuple<bool, size_t, size_t>
token::find_block(const std::string& str,
	const std::string& block_begin,	const std::string& block_end,
	const size_t begin,	const size_t count)
{ try {
	////////////////////////////////////////////////////////////////////////////////
	// Check input
	////////////////////////////////////////////////////////////////////////////////
	if(str.empty()) return { false, std::string::npos, 0 };
	if(begin >= str.size()) throw err("Begin position of search in string is greater or equal than string size. Begin: %zu, String size: %zu", begin, str.size());
	// if(count >= block_begin.size() + block_end.size()) throw err("Number of characters to be searched must be greater than block begin size plus block end size. Count: %zu, Block begin size: %zu, Block end size: %zu", count, block_begin.size(), block_end.size());

	std::string_view strv { str };
	strv = strv.substr(begin, count); // deixa o código mais legível para fazer os cálculos de retorno

	////////////////////////////////////////////////////////////////////////////////
	// procura o início do block
	////////////////////////////////////////////////////////////////////////////////
	const auto init = strv.find(block_begin);
	if(init == std::string::npos) return { false, std::string::npos, 0 };// não foi encontrado

	////////////////////////////////////////////////////////////////////////////////
	// procura o fim do block
	////////////////////////////////////////////////////////////////////////////////
	const auto end = strv.find(block_end, init+block_begin.size());
	if(end == std::string::npos) return { false, init + begin, strv.size()-init }; // não foi encontrado
	
	return { true, init + begin, end-init+block_end.size() }; // end_str-begin_str+begin+1 = o tamanho da string encontrada, contados os characteres '"' de início e fim da string
 } catch (const std::exception &e) { throw err(e.what()); }
}

std::tuple<bool, size_t, size_t>
token::find_block(const std::string_view& str,
	const std::string& block_begin,	const std::string& block_end,
	const size_t begin,	const size_t count)
{ try {
	////////////////////////////////////////////////////////////////////////////////
	// Check input
	////////////////////////////////////////////////////////////////////////////////
	if(str.empty()) return { false, std::string::npos, 0 };
	if(begin >= str.size()) throw err("Begin position of search in string is greater or equal than string size. Begin: %zu, String size: %zu", begin, str.size());
	// if(count >= block_begin.size() + block_end.size()) throw err("Number of characters to be searched must be greater than block begin size plus block end size. Count: %zu, Block begin size: %zu, Block end size: %zu", count, block_begin.size(), block_end.size());

	std::string_view strv { str };
	strv = strv.substr(begin, count); // deixa o código mais legível para fazer os cálculos de retorno

	////////////////////////////////////////////////////////////////////////////////
	// procura o início do block
	////////////////////////////////////////////////////////////////////////////////
	const auto init = strv.find(block_begin);
	if(init == std::string::npos) return { false, std::string::npos, 0 };// não foi encontrado

	////////////////////////////////////////////////////////////////////////////////
	// procura o fim do block
	////////////////////////////////////////////////////////////////////////////////
	const auto end = strv.find(block_end, init+block_begin.size());
	if(end == std::string::npos) return { false, init + begin, strv.size()-init }; // não foi encontrado
	
	return { true, init + begin, end-init+block_end.size() }; // end_str-begin_str+begin+1 = o tamanho da string encontrada, contados os characteres '"' de início e fim da string
 } catch (const std::exception &e) { throw err(e.what()); }
}
