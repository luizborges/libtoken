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
 } catch (const std::exception &e) { throw erx(e.what()); }
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
 } catch (const std::exception &e) { throw erx(e.what()); }
}

std::tuple<size_t, long> 
token::token_t::search(const std::string_view& strs) {
 try {
	if(never_match == true) { return std::make_tuple(std::string::npos, -1); }
			
	size_t pos = strs.find(str);
	if(pos == std::string::npos) {
		never_match = true;
		return std::make_tuple(std::string::npos, -1);
	} else {
		return std::make_tuple(pos, str.size());
	}
 } catch (const std::exception &e) { throw erx(e.what()); }
}

// std::tuple<size_t, size_t> 
// token::token_t::search2(const std::string_view& strs, const size_t strs_init_search) {
//  try {
// 	if(never_match == true) { return std::make_tuple(std::string::npos, 0); }

// 	// check if it is necessary to proceed to a new search
// 	if(pos != std::string::npos 
// 		&& pos >= strs_init_search) { // se entrar no if não é necessário nova busca
// 		return std::make_tuple(pos, str.size());	
// 	}

// 	// it is necessary to proceed to a new search - necessário nova busca
// 	size_t new_pos = strs.find(str, strs_init_search);
// 	if(new_pos == std::string::npos) {
// 		never_match = true;
// 		pos = std::string::npos; // apenas para fins de coerencia - não necessário
// 		return std::make_tuple(std::string::npos, 0);
// 	} else {
// 		pos = new_pos;
// 		return std::make_tuple(pos, str.size());
// 	}
//  } catch (const std::exception &e) { throw erx(e.what()); }
// }

////////////////////////////////////////////////////////////////////////////////
// Implementation of functions - class token2_t
////////////////////////////////////////////////////////////////////////////////
std::tuple<std::string_view::size_type, std::string_view::size_type> 
token::token2_t::find(
	const std::string_view& str, 
	const std::string_view::size_type str_pos_init,
	const std::string_view::size_type str_count)
{
 try {
	// show(__PRETTY_FUNCTION__,"::",__LINE__,"\n");
	// show(__func__,"::",__LINE__,"\n");
	if(_never_match_ == true) { 
		return std::make_tuple(std::string_view::npos, 0);
	}
	// show(__func__,"::",__LINE__,"\n");
	// check if it is necessary to proceed to a new search
	if(_pos_ != std::string_view::npos 
		&& _pos_ >= str_pos_init) { // se entrar no if não é necessário nova busca
		return std::make_tuple(_pos_, _str_.size());
	}

	// it is necessary to proceed to a new search - necessário nova busca
	// necessario o str.substr()
	// necessário pois não existe a função std::string_view::find((str_view || str), pos, count)
	// const auto strs = str.substr(0, str_count);
	// show(__func__,"::",__LINE__,"\n");
	// const auto new_pos = str.find(&_str_.at(0), str_pos_init, str_count);
	const auto new_str = str.substr(0, str_count);
	const auto new_pos = new_str.find(_str_, str_pos_init);
	// show("__func__:: str: '",str,"' | _str_:'",&_str_.at(0),"' | str_pos_init: ",str_pos_init," | str_count: ",str_count," | new_pos: ",new_pos,"\n");
	if(new_pos == std::string_view::npos) {
		// show(__func__,"::",__LINE__,"\n");
		_never_match_ = true;
		_pos_ = std::string_view::npos; // apenas para fins de coerencia - não necessário
		return std::make_tuple(std::string_view::npos, 0);
	} else {
		// show(__func__,"::",__LINE__,"\n");
		_pos_ = new_pos;
		return std::make_tuple(_pos_, _str_.size());
	}
 } catch (const std::exception &e) { throw erx(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// Implementation of functions - class token_str_t
///////////////////////////////////////////////////////////////////////////////	
std::tuple<std::string_view::size_type, std::string_view::size_type>
token::token_str_t::find(
	const std::string_view& str, 
	const std::string_view::size_type str_pos_init,
	const std::string_view::size_type str_count)
{
 try {
	if(_never_match_ == true) { 
		return std::make_tuple(std::string_view::npos, 0);
	}

	// check if it is necessary to proceed to a new search
	if(_pos_ != std::string_view::npos 
		&& _pos_ >= str_pos_init) { // se entrar no if não é necessário nova busca
		return std::make_tuple(_pos_, _str_.size());	
	}

	// it is necessary to proceed to a new search - necessário nova busca
	const auto [exists, new_pos, new_size] = find_str(str, str_pos_init, str_count);
	if(exists == false) {
		if(new_pos == std::string::npos) { // não existe uma string C/C++ válida
			_never_match_ = true;
			_pos_ = std::string_view::npos; // apenas para fins de coerencia - não necessário
			return std::make_tuple(std::string_view::npos, 0);
		} else {
			throw erx("String has a init string in C/C++ format but has no valid end. init position in string: ",new_pos);
		}
	} else {
		_pos_ = new_pos;
		_str_ = str.substr(new_pos, new_size);
		return std::make_tuple(new_pos, new_size);
	}
 } catch (const std::exception &e) { throw erx(e.what()); }
}

////////////////////////////////////////////////////////////////////////////////
// Implementation of general functions
////////////////////////////////////////////////////////////////////////////////

std::tuple<bool, size_t, size_t>
token::find_block(const std::string& str,
	const std::string& block_begin,	const std::string& block_end,
	const size_t begin,	const size_t count)
{ try {
	std::string_view strv { str };
	return find_block(strv, block_begin, block_end, begin, count);
 } catch (const std::exception &e) { throw erx(e.what()); }
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
	// if(begin >= str.size()) throw err("Begin position of search in string is greater or equal than string size. Begin: %zu, String size: %zu", begin, str.size());
	if(begin >= str.size()) throw erx("Begin position of search in string is greater or equal than string size. Begin: ",begin,", String size: ",str.size(),", String: '",str,"'");
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
 } catch (const std::exception &e) { throw erx(e.what()); }
}
