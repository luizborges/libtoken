/**
 */
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include "../../../cweb.hpp"

////////////////////////////////////////////////////////////////////////////////
// Private Structs
//////////////////////////////////////////////////////////////////////////////
class Post
{ protected:

	struct cmp_str // use this to compare strings (C-style) in map function to find key map
	{
		bool operator()(char const *a, char const *b) const
		{
			return std::strcmp(a, b) < 0;
   		}
	};

	char *_stdin = NULL; // keep all the post encode content - key + value - é o post tal como recebido no stdin
	char *str = NULL; // only the value of the post's decode content
	long size = 0; // ambas as strings tem o mesmo tamanho -> str e value, always
	map<const char*, const char*, cmp_str> _map;
	
 public:
 	Post()
 	{
 		size = 2048;
 		_stdin = new char[size];
		if(_stdin == NULL) {
			Error("CWEB::POST - Allocated Space for _stdin Post Buffer.\n Size is %d\n"
			"POST type is \"application/x-www-form-urlencoded\"\nerro is %d\n"
			"str erro is \"%s\"", size, errno, strerror(errno));
		}
		
		str = new char[size];
		if(str == NULL) {
			Error("CWEB::POST - Allocated Space for Post Undecoded Buffer.\n Size is %d\n"
			"POST type is \"application/x-www-form-urlencoded\"\nerro is %d\n"
			"str erro is \"%s\"", size, errno, strerror(errno));
		}
	}
 	
 	virtual bool init(const long max_size)
 	{
		///////////////////////////////////////////////////////////////////
		// verifica o tamanho do post, verfica o tamanho dos buffers
		///////////////////////////////////////////////////////////////////
		char *strctlen = getenv("CONTENT_LENGTH");
		if(strctlen == NULL) {
			Error("CWEB::IN - NO CONTENT_LENGTH\ngetenv(\"CONTENT_LENGTH\") = NULL\n"
			"POST type is \"application/x-www-form-urlencoded\"");
		}
		long stdin_size = strtol(strctlen, NULL, 0);
		if(stdin_size == 0L) {
			Error("CWEB::IN - strtol()\nstrtol() = 0L\ngetenv(\"CONTENT_LENGTH\") = \"%s\"\n"
			"POST type is \"application/x-www-form-urlencoded\"\nerro is %d\n"
			"str erro is \"%s\"", strctlen, errno, strerror(errno));
		}
		if(stdin_size < 0L) {
			Error("CWEB::IN - CONTENT_LENGTH is lesser than 1\n"
			"getenv(\"CONTENT_LENGTH\") = \"%s\"\nCONTENT_LENGTH = %ld"
			"POST type is \"application/x-www-form-urlencoded\"",
			strctlen, stdin_size);
		}
		
		if(stdin_size > max_size && max_size != 0)
		{
			Error("CWEB::IN - size of input data recieve by HTTP POST METHOD is bigger"
			"bigger than max size allowed for this type of input.\n"
			"getenv(\"CONTENT_LENGTH\") = \"%s\"\nCONTENT_LENGTH = %ld bytes\n"
			"Maximum size for HTTP POST METHOD -\"application/x-www-form-urlencoded\" "
			"= %ld bytes\nYou can change this size.\nSee documentation for more details",
			strctlen, stdin_size, max_size);
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
				"POST type is \"application/x-www-form-urlencoded\"\nerro is %d\n"
				"str erro is \"%s\"", strctlen, size*sizeof(char), errno, strerror(errno));
			}
			
			str = static_cast<char*>(malloc(size*sizeof(char)));
			if(str == NULL) {
				Error("CWEB::IN - Allocated Space for POST - String Default.\n"
				"getenv(\"CONTENT_LENGTH\") = \"%s\"\nTried to allocated %d bytes\n"
				"POST type is \"application/x-www-form-urlencoded\"\nerro is %d\n"
				"str erro is \"%s\"", strctlen, size*sizeof(char), errno, strerror(errno));
			}
		}
	
		// insere o post dentro da string para realiar a leitura
		size_t read = fread(_stdin, sizeof(char), stdin_size, stdin);
		if(read != static_cast<size_t>(stdin_size)) {
			Error("CWEB::IN - Reading post content and putting it into postStr.\n"
			"Read in post content: %ld\nCONTENT_LENGTH string: \"%s\"\nCONTENT_LENGTH: %ld\n"
			"POST type is \"application/x-www-form-urlencoded\"\nerro is %d\n"
			"str erro is \"%s\"", read, strctlen, stdin_size, errno, strerror(errno));
		}
		
		fill_map();
		return true; // foi possível inicializar o HTTP REQUEST METHOD POST no mapa de busca
	}
	
	virtual const char*
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
		"type of the post is \"%s\"\nfectch key = \"%s\"\nnumber of keys is %d\n"
		"List of all keys in HTTP REQUEST METHOD POST that be parsed:",
		type(), key, _map.size());
		
		for(auto elem : _map)
		{
   			fprintf(stderr, "[\"%s\"] = \"%s\"\n", elem.first, elem.second);
		}
		
		return NULL;
	}
	
	virtual const char* type()
	{
		return "application/x-www-form-urlencoded";
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
				"POST type is \"application/x-www-form-urlencoded\"\n"
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
				"content length: %d\nPOST type is \"application/x-www-form-urlencoded\"\n"
				"HTTP POST: \"%s\"\n", content, contentDecode, contentLen, _stdin);
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
// Treat when the getenv("CONTENT_TYPE") = "multipart/form-data"
////////////////////////////////////////////////////////////////////////////////
#include "multipart.cpp"

////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////
static Post _post;
static Mult _mult;
static Post* _running = NULL;
////////////////////////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////////////////////////
/**
 * Insere a string do http request method post in a map format
 */
bool
cweb::in::init_post(const long max_size)
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
		Error("CWEB::IN - NO CONTENT_TYPE\ngetenv(\"CONTENT_TYPE\") = NULL");
	}
	if(strcmp(ct, "application/x-www-form-urlencoded") == 0) {
		_running = &_post;
	} else
	if(strncmp(ct, "multipart/form-data", 19) == 0) {
		_running = &_mult;
	} else {
		Error("CWEB::IN - CONTENT_TYPE NOT IMPLEMENTED.\n"
		"getenv(\"CONTENT_TYPE\") = \"%s\".\n"
		"This library only implemented getenv(\"CONTENT_TYPE\") = "
		"\"application/x-www-form-urlencoded\" or \"multipart/form-data\"\n", ct);
	}
	
	return _running->init(max_size);
}


/**
 * 
 */
const char*
cweb::in::post(const char *key)
{
	if(_running == NULL) return NULL;
	return _running->post(key);
}














