/**
 */
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include "../../cweb.hpp"

struct cmp_str // use this to compare strings (C-style) in map function to find key map
{
	bool operator()(char const *a, char const *b) const
	{
		return std::strcmp(a, b) < 0;
   	}
};

////////////////////////////////////////////////////////////////////////////////
// Private Class
////////////////////////////////////////////////////////////////////////////////
class Cookie
{
	// variables
	char *http_cookie = NULL; // original string = getenv("HTTP_COOKIE");
	int hclen = 0; // size of http_cookie - htlen = http_cookien_len
	char *str = NULL;
	int size = 0; // size of str
	map<const char*, const char*, cmp_str> _map;
	// functions
 public:

	Cookie()
	{
 		size = 2048; // 2 kB is the default size of str to keep cookies
 		str = static_cast<char*>(malloc(2048*sizeof(char)));
 		if(str == NULL) {
			Error("CWEB::COOKIE - Allocated Space for Cookie - String Default.\n"
			"Tried to allocated %d bytes\nerro is %d\nstr erro is \"%s\"",
			size*sizeof(char), errno, strerror(errno));
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// Public Functions
	////////////////////////////////////////////////////////////////////////////////	
	bool init()
	{
		_map.clear(); // limpa o map
		http_cookie = getenv("HTTP_COOKIE");
		if(http_cookie == NULL) {
			Warn("CWEB::COOKIE - NO COOKIE.\ngetenv(\"HTTP_COOKIE\") = NULL");
			return false;
		}
		
		hclen = strlen(http_cookie);
		if(size <= hclen)
		{
			size = hclen +1; // for character `\0`
			free(str);
		
			str = static_cast<char*>(malloc(size*sizeof(char)));
			if(str == NULL) {
				Error("CWEB::COOKIE - Allocated Space for Cookie - String Default.\n"
				"Tried to allocated %d bytes\nerro is %d\nstr erro is \"%s\"",
				size*sizeof(char), errno, strerror(errno));
			}
		}
		
		fill_map();
		/*
		MError("print all elements of map\nmap size: %d", _map.size());
		for(auto elem : _map)
		{
   			fprintf( stderr, "(func: \"%s\", %d)::[\"%s\"] = \"%s\" | "
   			"count() = %ld | str = %p | pid = %ld | ppid = %ld\n",
   			__PRETTY_FUNCTION__, __LINE__, elem.first, elem.second,
   			_map.count(elem.first), str, getpid(), getppid());
		}
		fprintf(stderr, "\n");
		*/
		return true;
	}
	
	const char* get(const char *key)
	{
		if(key != NULL)
		{
			auto it = _map.find(key);
			if (it != _map.end()) {
				return it->second;
			}
		}
		//PRINT_WARNING_IF_NO_KEY_IN_MAP:
		Warn("CWEB::COOKIE - Fetch for a no key of HTTP COOKIE.\nfectch key = \"%s\"\n"
		"number of keys is %d\nList of all keys in HTTP COOKIE that be parsed.\n",
		key, _map.size());
		
		for(auto elem : _map)
		{
   			fprintf(stderr, "[\"%s\"] = \"%s\"\n", elem.first, elem.second);
		}
		
		return NULL;
	}
	
	void set(const char *key, const char *value,
			const long expires_sec,
			const char *domain, const char *path,
			const bool isSecure, const bool isHttpOnly,
			FILE *output)
	{
		///////////////////////////////////////////////////////////////////
		// check the args
		///////////////////////////////////////////////////////////////////
		if(key == NULL) {
			Error("CWEB::COOKIE - Cookie Set Key is NULL.");
		}
		if(strlen(key) < 1) {
			Error("CWEB::COOKIE - Cookie Set Key is a empty string.");
		}
	
		if(value == NULL) {
			Error("CWEB::COOKIE - Cookie Set Value is NULL.\nkey is \"%s\"", key);
		}
		
		if(expires_sec < -1) {
			Error("CWEB::COOKIE - Cookie Set Expires seconds cannot be less than -1.\n"
				"Uses 0 to not set expires time to cookie.\n"
				"Uses -1 to expire a cookie, setting the value = \n"
				"\"; Expires=Thu, 01 Jan 1970 00:00:00 GMT\".\n"
				"Expires seconds passed is %d", expires_sec);
		}
	
		///////////////////////////////////////////////////////////////////
		// cookie set -
		///////////////////////////////////////////////////////////////////
		fprintf(output, "Set-Cookie: %s=%s",
			key, cweb::encode(value, strlen(value)));
		
		///////////////////////////////////////////////////////////////////
		// cookie set - time
		///////////////////////////////////////////////////////////////////
		if(expires_sec > 0)
		{
			time_t expires = time(NULL) + expires_sec;
			struct tm *localTimeExp = localtime(&expires);
			if(localTimeExp == NULL) {
				Error("Local Time is NULL.\nreturned from localtime() C function.\n"
				"erro is %d\nstr erro is \"%s\"", errno, strerror(errno));
			}
		
			char *st = asctime(localTimeExp);
			if(st == NULL) {
				Error("CWEB::COOKIE - String of Local Time is NULL.\n"
				"returned from asctime() C function.\nerro is %d\nstr erro is \"%s\"",
				errno, strerror(errno));
			}
			fprintf(output, "; Expires=");
			// segue o padrão RFC 7231, section 7.1.1.2: 
			//Date 	Hypertext Transfer Protocol (HTTP/1.1): Semantics and Content - 
			//https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Date
			// Date: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
			// Date: Wed, 21 Oct 2015 07:28:00 GMT 
			// DDD MMM dd hh:mm:ss YYYY - C time string
			fprintf(output, "%c%c%c, %c%c %c%c%c %c%c%c%c %c%c:%c%c:%c%c GMT",
				st[0], st[1], st[2], // week day
				st[8], st[9], // day
				st[4], st[5], st[6], // month
				st[20], st[21], st[22], st[23], // year
				st[11], st[12], // hour
				st[14], st[15], // minute
				st[17], st[18]); // second
			}
		else if(expires_sec == -1)
		{
			fprintf(output, "; Expires=Thu, 01 Jan 1970 00:00:00 GMT");
		}
	
		///////////////////////////////////////////////////////////////////
		// seta os outros atributos
		///////////////////////////////////////////////////////////////////
		if(domain != NULL)
		{
			if(strlen(domain) < 1) {
				Error("CWEB::COOKIE - Cookie Set Domain cannot be a empty string.");
			}
		
			fprintf(output, "; Domain=%s", domain);
		}
	
		if(path != NULL)
		{
			if(strlen(path) < 1) {
				Error("CWEB::COOKIE - Cookie Set Path cannot be a empty string.");
			}
		
			fprintf(output, "; Path=%s", path);
		}
	
		if(isSecure == true)
		{
			fprintf(output, "; Secure");
		}
	
		if(isHttpOnly == true)
		{
			fprintf(output, "; HttpOnly");
		}
	
		fprintf(output, "\r\n"); // para indicar o fim do cookie
	}

	////////////////////////////////////////////////////////////////////////////////
	// Private Functions Head
	////////////////////////////////////////////////////////////////////////////////
 private:
 
 	void fill_map()
 	{
 		int posInitNextStr = 0;
		for(int i=0; i < hclen; ++i)
		{
			///////////////////////////////////////////////////////////////////
			// busca, decodifica chave do ENCODE - um par de key e conteúdo
			///////////////////////////////////////////////////////////////////
			const char *key = &http_cookie[i];
			
			// descobre o tamanho da key
			int keyLen = 0;
			for(; i < hclen && key[keyLen] != '='; ++keyLen) {++i;} 
		
		
			char *keyDecode = &str[posInitNextStr];
			if(cweb::decode(keyDecode, key, keyLen) == false) { // decode the key
				Error("CWEB::COOKIE - decoding key in HTTP COOKIE.\nkey enconding: \"%s\"\n"
					"key decode (wrong value): \"%s\"\nKey length: %d\n"
					"HTTP COOKIE: \"%s\"\n", key, keyDecode, keyLen, str);
			}
			posInitNextStr += strlen(keyDecode) +2; // atualiza a nova posição inicial
		
			///////////////////////////////////////////////////////////////////
			// busca, decodifica contéudo da STR ENCODE - um par de key e contéudo
			///////////////////////////////////////////////////////////////////
			const char *content = &http_cookie[++i]; // seta o início da string do content
			int contentLen = 0;
			for(; i < hclen && content[contentLen] != ';';
				++contentLen) { ++i; } // descobre o tamanho do content
		
			// recebe a posição no array de conteúdo própria para guardar o valor
			char *contentDecode = &str[posInitNextStr];
			// decode o conteúdo do par do get
			if(cweb::decode(contentDecode, content, contentLen) == false) { 
				Error("CWEB::COOKIE - decoding content in HTTP COOKIE.\n"
				"content enconding: \"%s\"\nkey decode (wrong value): \"%s\"\n"
				"content length: %d\nHTTP COOKIE: \"%s\"",
				content, contentDecode, contentLen, str);
			}
			posInitNextStr += strlen(contentDecode) +2; // atualiza a nova posição inicial
			// atualiza a posição - final do conteúdo é "; "
			// é necessário para buscar o próximo key se houver
			++i;
		
			///////////////////////////////////////////////////////////////////
			// insere no map o key e o conteúdo - atualiza a posição na array do get
			///////////////////////////////////////////////////////////////////
			_map[keyDecode] = contentDecode;
		}
 	}
};

static Cookie _cookie;

////////////////////////////////////////////////////////////////////////////////
// Private Variables
////////////////////////////////////////////////////////////////////////////////

								
////////////////////////////////////////////////////////////////////////////////
// Private Functions Head
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Private Functions Inline
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Construct Functions
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////////////////////////
bool
cweb::cookie::init()
{
	return _cookie.init();
}

const char*
cweb::cookie::get(const char *key)
{
	return _cookie.get(key);
}


char*
cweb::cookie::set(const char *key, const char *value,
				  const long expires_sec,
				  const bool isSecure,
				  const bool isHttpOnly,
				  const char *domain,
				  const char *path)
{
	///////////////////////////////////////////////////////////////////
	// Create temp file to keep string
	///////////////////////////////////////////////////////////////////
	FILE *tmp = tmpfile();
	if(tmp == NULL) {
		Error("CWEB::COOKIE - Cookie Set Cannot Create a temporary file.\n"
		"erro is %d\nstr erro is \"%s\"",errno, strerror(errno));
	}
	
	///////////////////////////////////////////////////////////////////
	// create cookie
	///////////////////////////////////////////////////////////////////
	_cookie.set(key, value, expires_sec, domain, 
				path, isSecure, isHttpOnly, tmp);
	
	///////////////////////////////////////////////////////////////////
	// return the string
	///////////////////////////////////////////////////////////////////
	rewind(tmp);
	char* _cookie = util::file::toStr(tmp); // já fecha o arquivo
	return _cookie;
}


void
cweb::cookie::print(const char *key, const char *value,
					const long expires_sec,
					const bool isSecure, const bool isHttpOnly,
				  	const char *domain, const char *path)
{
	_cookie.set(key, value, expires_sec, domain, 
				path, isSecure, isHttpOnly, stdout);
}


























