/**
 * @desc: implementa a leitura do post "enctype='multipart/form-data'"
 */

class FileImpl: public cweb::File
{
	char* name() { return NULL; }
	long  size() { return 0; }
	void* data() { return NULL; }
};


class Mult: public Post
{private:
	struct cmp_str // use this to compare strings (C-style) in map function to find key map
	{
		bool operator()(char const *a, char const *b) const
		{
			return std::strcmp(a, b) < 0;
   		}
	};
	
	map<const char*, FileImpl, cmp_str> _file;

 public:
	virtual bool init(const long max_size)
 	{
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
		
		if(stdin_size > max_size && max_size != 0)
		{
			Error("CWEB::IN - size of input data recieve by HTTP POST METHOD is bigger"
			"bigger than max size allowed for this type of input.\n"
			"getenv(\"CONTENT_LENGTH\") = \"%s\"\nCONTENT_LENGTH = %ld bytes\n"
			"Maximum size for HTTP POST METHOD -\"multipart/form-data\" "
			"= %ld bytes\nYou can change this size.\nSee documentation for more details",
			strctlen, stdin_size, max_size);
		}
	
		char *data = new char[stdin_size +1];
		
		fread(data, 1, stdin_size, stdin);
		data[stdin_size] = '\0';
		Error("data is\n\"%s\"", data);
		
		
		return true; // foi possível inicializar o HTTP REQUEST METHOD POST no mapa de busca
	}
 

	const char* type()
	{
		return "multipart/form-data";
	}
};











