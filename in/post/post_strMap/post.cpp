/**
 */
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include "../../../cweb.hpp"

////////////////////////////////////////////////////////////////////////////////
// Private Structs
////////////////////////////////////////////////////////////////////////////////
struct cmp_str // use this to compare strings (C-style) in map function to find key map
{
	bool operator()(char const *a, char const *b) const
	{
		return std::strcmp(a, b) < 0;
   	}
};

class Post
{
 private:
	char *_stdin; // keep all the post encode content - key + value - é o post tal como recebido no stdin
	char *str; // only the value of the post's decode content
	long size; // ambas as strings tem o mesmo tamanho -> str e value, always
	map<const char*, const char*, cmp_str> _map;
	
 public:
 	bool init()
 	{
 		// verifica se é possível inicializar o post
		char *rm = getenv("REQUEST_METHOD");
		if(rm == NULL) {
			Warn("CWEB::IN - NO REQUEST_METHOD.\ngetenv(\"REQUEST_METHOD\") = NULL");
			return false;
		}
		if(strcmp(rm, "POST") != 0) {
			Warn("CWEB::IN - REQUEST_METHOD IS NOT POST.\n"
			"getenv(\"REQUEST_METHOD\") = \"%s\"", rm);
			return false;
		}
		
		char *ct = getenv("CONTENT_TYPE");
		if(ct == NULL) {
			Warn("CWEB::IN - NO CONTENT_TYPE\ngetenv(\"CONTENT_TYPE\") = NULL")
			return false;
		}
		if(strcmp(ct, "application/x-www-form-urlencoded") != 0) {
			Warn("CWEB::IN - CONTENT_TYPE NOT IMPLEMENTED.\n"
			"getenv(\"CONTENT_TYPE\") = \"%s\".\n"
			"This library only implemented getenv(\"CONTENT_TYPE\") = "
			"\"application/x-www-form-urlencoded\"\n", ct);
			return false;
		}
	
		///////////////////////////////////////////////////////////////////
		// verifica o tamanho do post, verfica o tamanho dos buffers
		///////////////////////////////////////////////////////////////////
		char *strctlen = getenv("CONTENT_LENGTH");
		if(strctlen == NULL) {
			Warn("CWEB::IN - NO CONTENT_LENGTH\ngetenv(\"CONTENT_LENGTH\") = NULL")
			return false;
		}
		long stdin_size = strtol(strctlen, NULL, 0);
		if(stdin_size == 0L) {
			Error("CWEB::IN - strtol()\nstrtol() = 0L\ngetenv(\"CONTENT_LENGTH\") = \"%s\"\n"
			"erro is %d\nstr erro is \"%s\"", strctlen, errno, strerror(errno));
		}
		if(stdin_size < 0L) {
			Error("CWEB::IN - CONTENT_LENGTH is lesser than 1\n"
			"getenv(\"CONTENT_LENGTH\") = \"%s\"\nCONTENT_LENGTH = %ld",
			strctlen, stdin_size);
		}
	
		if(stdin_size >= size)
		{
			size = stdin_size +1; // for character `\0`
			free(_stdin);
			free(str);
		
			_stdin = static_cast<char*>(malloc(size*sizeof(char)));
			if(_stdin == NULL) {
				Error("CWEB::IN - Allocated Space for STDIN POST - String Default.\n"
				"getenv(\"CONTENT_LENGTH\") = \"%s\"\nTried to allocated %d bytes\n"
				"erro is %d\nstr erro is \"%s\"",
				strctlen, size*sizeof(char), errno, strerror(errno));
			}
			
			str = static_cast<char*>(malloc(size*sizeof(char)));
			if(str == NULL) {
				Error("CWEB::IN - Allocated Space for POST - String Default.\n"
				"getenv(\"CONTENT_LENGTH\") = \"%s\"\nTried to allocated %d bytes\n"
				"erro is %d\nstr erro is \"%s\"",
				strctlen, size*sizeof(char), errno, strerror(errno));
			}
		}
	
		// insere o post dentro da string para realiar a leitura
		size_t read = fread(_stdin, sizeof(char), stdin_size, stdin);
		if(read != static_cast<size_t>(stdin_size)) {
			Error("CWEB::IN - Reading post content and putting it into postStr.\n"
			"Read in post content: %ld\nCONTENT_LENGTH string: \"%s\"\nCONTENT_LENGTH: %ld\n"
			"erro is %d\nstr erro is \"%s\"",
			read, strctlen, stdin_size, errno, strerror(errno));
		}
		
		fill_map();
		return true; // foi possível inicializar o HTTP REQUEST METHOD POST no mapa de busca
	}
	
	const char*
	post(const char *key)
	{
		if(key != NULL)
		{
			auto it = _map.find(key);
			if (it != _map.end()) {
				return it->second;
			}
		}
		//PRINT_WARNING_IF_NO_KEY_IN_MAP:
		Warn("CWEB::IN - Fetch for a no key of HTTP REQUEST METHOD POST.\n"
		"fectch key = \"%s\"\nnumber of keys is %d\n"
		"List of all keys in HTTP REQUEST METHOD POST that be parsed:", key, _map.size());
		
		for(auto elem : _map)
		{
   			fprintf(stderr, "[\"%s\"] = \"%s\"\n", elem.first, elem.second);
		}
		
		return NULL;
	}

 private:
	void fill_map()
	{
		const long stdin_size = size -1;
		int posInitNextStr = 0;
		for(int i=0; i < stdin_size; ++i)
		{
			///////////////////////////////////////////////////////////////////
			// busca, decodifica chave do post - um par de key e conteúdo
			///////////////////////////////////////////////////////////////////
			const char *key = &_stdin[i];
			
			// descobre o tamanho de key
			int keyLen = 0;
			for(; i < stdin_size && key[keyLen] != '='; ++keyLen) {++i;}
		
		
			char *keyDecode = &str[posInitNextStr];
			if(cweb::decode(keyDecode, key, keyLen) != true) { // decode the key
				Error("CWEB::IN - Decoding key in HTTP REQUEST METHOD POST.\n"
				"key enconding: \"%s\"\nkey decode (wrong value): \"%s\"\nKey length: %d\n"
				"HTTP post: \"%s\"\n", key, keyDecode, keyLen, _stdin);
			}
			posInitNextStr += strlen(keyDecode) +2; // atualiza a nova posição inicial
		
			///////////////////////////////////////////////////////////////////
			// busca, decodifica contéudo do post - um par de key e contéudo
			///////////////////////////////////////////////////////////////////
			const char *content = &_stdin[++i]; // seta o início da string do content
			int contentLen = 0;
			for(; i < stdin_size && content[contentLen] != '&';
			++contentLen) { ++i; } // descobre o tamanho do content
		
			// recebe a posição no array de conteúdo própria para guardar o valor
			char *contentDecode = &str[posInitNextStr];
			// decode o conteúdo do par do post
			if(cweb::decode(contentDecode, content, contentLen) != true) {
				Error("CWEB::IN - Decoding content in HTTP REQUEST METHOD POST.\n"
				"content enconding: \"%s\"\nkey decode (wrong value): \"%s\"\n"
				"content length: %d\nHTTP POST: \"%s\"\n",
				content, contentDecode, contentLen, _stdin);
			}
			
			posInitNextStr += strlen(contentDecode) +2; // atualiza a nova posição inicial
		
			///////////////////////////////////////////////////////////////////
			// insere no map o key e o conteúdo - atualiza a posição na array do post
			///////////////////////////////////////////////////////////////////
			_map[keyDecode] = contentDecode;
		}
	}
};
////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////
static Post _post;
////////////////////////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////////////////////////
/**
 * Insere a string do http request method post in a map format
 */
bool
cweb::in::init_post()
{
	return _post.init();
}


/**
 * 
 */
const char*
cweb::in::post(const char *key)
{
	return _post.post(key);
}














