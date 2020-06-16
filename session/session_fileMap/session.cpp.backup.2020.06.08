/**
 */
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
//#include "session_fileMap.h"
#include "../../cweb.hpp"

////////////////////////////////////////////////////////////////////////////////
// Private Structs
////////////////////////////////////////////////////////////////////////////////
class Session
{ private:
	////////////////////////////////////////////////////////////////////////////////
	// inner structs
	////////////////////////////////////////////////////////////////////////////////
	struct cmp_str // use this to compare strings (C-style) in map function to find key map
	{
		bool operator()(char const *a, char const *b) const
		{
			return std::strcmp(a, b) < 0;
   		}
	};
	
	struct Content // use this to keep the array/variale that will be write in session file
	{
		void* value = NULL;
		int size = 0;
		int numElem = 0;
	};
	
	////////////////////////////////////////////////////////////////////////////////
	// inner variables
	////////////////////////////////////////////////////////////////////////////////
	char *dir   = NULL;
	char *fname = NULL;
	char *_sid   = NULL;
	bool toSave = false;
	time_t life = 5*60; // 5 minutos tempo default de duração da sessão
	map<const char*, Content*, cmp_str> _map;

 public:
 	Session()
 	{
 		dir = new char[1];
 		strncpy(dir, "", 1); // inicializa como um string vazia.
 	}
 
	void config(const char *dirFileSession = "",
				const time_t lifeSession = 5*60, // valor default de 5 min
				const time_t del = 10*60) // valor default de 10 min);
	{
		setDir(dirFileSession);
		life = lifeSession;
		remove_old_files(del);
	}
	
	bool load()
	{
		FILE* fs = getFile();
	   	if(fs == NULL) {
   			Warn("CWEB::SESSION - File Session is \"%s\"\nerro is %d\nstr erro is \"%s\"",
			fname, errno, strerror(errno));
			return false;
   		}
   	
   		if(is_expired(fs) == true) {
   			return false;
   		}
   	
   		load_map(fs);
   		int errn = fclose(fs);
   		if(errn != 0) {
   			Error("CWEB::SESSION - In close file session name.\n"
   			"File session name is \"%s\"\nerro is %d\nstr erro is \"%s\"",
   			fname, errno, strerror(errno));
   		}
   		
   		////////////////////////////////////////////////////////////////////////////////
		// force call save() or sid()
		////////////////////////////////////////////////////////////////////////////////
		_sid = NULL;
		toSave = false;
   		
   		return true;
	}
	
	char* sid()
	{
		char *nsec = NULL; // nano seconds
		bool swap  = false;
		#if defined(unix) || defined(__unix) || defined(__unix__) || (defined (__APPLE__) && defined (__MACH__)) // Unix (Linux, *BSD, Mac OS X)
		
		#ifdef _SC_MONOTONIC_CLOCK
		struct timespec ts;
		if (sysconf (_SC_MONOTONIC_CLOCK) > 0) {
	    	if (clock_gettime (CLOCK_MONOTONIC, &ts) == 0)
	    	{
	    		int size = sizeof(ts.tv_nsec) +1;
	    		nsec = new char[size];
	    		if(nsec == NULL) {
	    			Error("CWEB::SESSION - In allocation memory for nano seconds to compose "
	    			"the name of file session.\nsize is %li\nerro is %d\nstr erro is \"%s\"",
					size, errno, strerror(errno));
	    		}
	    		snprintf(nsec, size, "%ld", ts.tv_nsec);
	    		swap = ((ts.tv_nsec%2) == 0) ? false : true;
        	}
		}
		
		#endif
		#endif// for windows and olther system
		if(nsec == NULL)
		{
			nsec = new char[1];
			*nsec = '\0';
		}
		srand(time(NULL));
		int al = rand();
		time_t current = time(NULL);
		int sl = sizeof(int)+sizeof(time_t)+strlen(nsec)+1;
		_sid = new char[sl];
		if(_sid == NULL) {
			Error("CWEB::SESSION - In allocation memory for name of file session.\n"
			"Size is %ld\nerro is %d\nstr erro is \"%s\"",
			sl, errno, strerror(errno));
		}
		
		// insere outra camada de aleatoriedade na criação do sid
		if(swap) {
			snprintf(_sid, sl, "%li%s%d", current, nsec, al);
		} else {
			snprintf(_sid, sl, "%li%d%s", current, al, nsec);
		}
		
		int size = strlen(dir) + strlen(_sid) + strlen(".session") +1;
		/*Warn("++SIZE IS %d\ndir is \"%s\"\nstrlen(dir) = %d\nsid is \"%s\"\nstrlen(sid) is %d\nnsec is \"%s\"\nstrlen(nsec) = %d\nsid size is %d",
		size, dir, strlen(dir), sid, strlen(sid), nsec, strlen(nsec), sl);
		*/
		fname = new char[size];
		if(fname == NULL) {
			Error("CWEB::SESSION - In allocation memory for name of file session.\n"
			"Size is %ld\nerro is %d\nstr erro is \"%s\"", size, errno, strerror(errno));
		}
		snprintf(fname, size, "%s%s.session", dir, _sid);
		
		////////////////////////////////////////////////////////////////////////////////
		// set to save the session
		////////////////////////////////////////////////////////////////////////////////
		toSave = true;
		
		return _sid;
	}
	
	char* save(const bool _toSave = false)
	{
		toSave = (toSave || _toSave) ? true:false;
		////////////////////////////////////////////////////////////////////////////////
		// remove the current file session name
		////////////////////////////////////////////////////////////////////////////////
		if(fname != NULL && toSave == false)
		{
   			if(remove(fname) != 0) {
   				MError("CWEB::SESSION - In remove file session name.\n"
   				"File session name is \"%s\"\nerro is %d\nstr erro is \"%s\"",
				fname, errno, strerror(errno));
   			}
			fname = NULL;
		}
		
		////////////////////////////////////////////////////////////////////////////////
		// create file session
		////////////////////////////////////////////////////////////////////////////////
		if(toSave == false) return NULL;
		
		if(_sid == NULL) {
			sid();
		}
		
 		FILE *f = fopen(fname, "wb");
 		if(f == NULL) {
			Error("CWEB::SESSION - Creating session file.\nfile name is \"%s\"\n"
			"Variable errno is %d\nDefault msg error is \"%s\"\nerro is %d\n"
			"str erro is \"%s\"",
			fname, errno, strerror(errno), errno, strerror(errno));
		}
		
 		bool hasBody = save_header(f);
 		if(hasBody == true) {
 			save_body(f);
 		}
    		
    	if(fclose(f) != 0) {
   			Error("CWEB::SESSION - In close file session name.\n"
   			"File session name is \"%s\"\nerro is %d\nstr erro is \"%s\"",
   			fname, errno, strerror(errno));
   		}
		
 		return _sid;
	}
	
	bool set(
		const char *key = NULL,
		const void *value = NULL,
		const int size = 0,
		const int numElem = 0)
	{
		if(key == NULL)   return false;
		if(value == NULL) return false;
		if(size < 1)      return false;
		if(numElem < 1)   return false;
		
		Content* cont;
		auto it = _map.find(key);
		if (it != _map.end()) {
			cont = it->second;
		} else {
			cont = new Content;
			if(cont == NULL) {
				Error("CWEB::SESSION - In allocation memory for content of session.\n"
	    		"key of content is \"%s\"\nsize of content is %li\nerro is %d\n"
	    		"str erro is \"%s\"", key, sizeof(Content), errno, strerror(errno));
			}
		}
				
		cont->value = const_cast<void*>(value);
		cont->size  = size;
		cont->numElem = numElem;
		_map[key] = cont;
		return true;
	}
	
	void* get(
		const char *key = NULL,
		int *numElem = NULL,
		int *size = NULL)
	{
		if(key == NULL)
		{
			log_print_no_key_map(key);
			return NULL;
		} 
		
		auto it = _map.find(key);
		if (it == _map.end()) // key not found
		{
			log_print_no_key_map(key);
			return NULL;
		}
		
		if(numElem != NULL) {
			*numElem = it->second->numElem;
		}
		if(size != NULL) {
			*size = it->second->size;
		}
		
		return it->second->value;
	}
	
	void* del(
		const char *key = NULL,
		int *numElem = NULL,
		int *size = NULL)
	{
		if(key == NULL)
		{
			log_print_no_key_map(key);
			return NULL;
		} 
		
		auto it = _map.find(key);
		if (it == _map.end()) // key not found
		{
			log_print_no_key_map(key);
			return NULL;
		}
		
		if(numElem != NULL) {
			*numElem = it->second->numElem;
		}
		if(size != NULL) {
			*size = it->second->size;
		}
		
		const char *_key = it->first;
		Content* c = it->second;
		void *value = it->second->value;
		_map.erase(it);
		delete[] _key; // libera a memória
		delete   c;
		return value;
	}
	
	int clean()
	{
		int numKeys = _map.size();
		_map.clear();
		return numKeys;
	}
 private:
 
	void setDir(const char *dirFileSession)
	{
		if(dirFileSession == NULL) return;
		
		int size = strlen(dirFileSession);
		dir = new char[size+1];
		if(dir == NULL) {
			Error("CWEB::SESSION - In allocation memory for directory.\nSize is %ld\n"
				"Directory is \"%s\"\nerro is %d\nstr erro is \"%s\"",
				size+1, dirFileSession, errno, strerror(errno));
		}
		strcpy(dir, dirFileSession);
					
		#if defined(_WIN32) || defined(_WIN64)
		Warn("CWEB::SESSION - Check the end of directory name is not implemented "
		"yet for Windows System.\n"
		"The user must check himself to use cweb::session::config()");
			
		#elif defined(unix) || defined(__unix) || defined(__unix__) || (defined (__APPLE__) && defined (__MACH__)) // Unix (Linux, *BSD, Mac OS X)
		// insere o character '/' ao final da str
		if(dir[size-1] != '/' && strlen(dir) > 1) {
			dir = (char*)realloc(dir, (size+2)*sizeof(char));
			if(dir == NULL) {
				Error("CWEB::SESSION - In reallocation memory for directory.\n"
				"Size is %ld\nDirectory is \"%s\"\nerro is %d\nstr erro is \"%s\"",
				size+2, dirFileSession, errno, strerror(errno));
			}
			
			dir[size] = '/';
			dir[size+1] = '\0';
		}
		#endif
	}
	
	void remove_old_files(const time_t del)
	{
		if(del < 1) { // não faz nada
			return;
		}
		if (del <= life) {
			Warn("CWEB::SESSION - Time to delete old files session is lesser than "
			"life time to a session file.\nNothing will be done by this function.\n"
			"NO file session delete by this funciton.");
			return;
		}
	
		#if defined(_WIN32) || defined(_WIN64)
		Warn("CWEB::SESSION - Session Clean Old File Sessions is not implemented "
		"yet to Windows System.");
	
		#elif defined(unix) || defined(__unix) || defined(__unix__) || (defined (__APPLE__) && defined (__MACH__)) // Unix (Linux, *BSD, Mac OS X)

		DIR *dir = opendir (this->dir); // open de directory
		if(dir == NULL) {
			Error("CWEB::SESSION - Couldn't open the directory where Session files is.\n"
			"Directory name passed is \"%s\"\nerro is %d\nstr erro is \"%s\"",
			this->dir, errno, strerror(errno));
		}
	
		// create array to keep file session name
		int dirLen = strlen(this->dir);
		int maxName = 29;
		char *fname = new char[dirLen + maxName +1];
		if(fname == NULL) {
			Error("CWEB::SESSION - In allocation memory for reading file session name.\n"
			"The default length of file session name is %d.\n"
			"Directory of Session File is \"%s\"\nLength of directory name is %d\n"
			"erro is %d\nstr erro is \"%s\"",
			dirLen+maxName+1, this->dir, dirLen, errno, strerror(errno));
		}
	
		// read each file in directory to remove the file how match the condition
		struct dirent *ep;
		while ((ep = readdir(dir)) != NULL)
		{
    	    bool isS = is_session_file(ep->d_name);
    	    if(isS == false) continue;
        
    	    int flen = strlen(ep->d_name);
       		// verifia se a string pode suportar o tamanho do diretorio + do nome do arquivo
        	if(flen > maxName) { 
        		maxName = flen;  
        		fname = (char*)realloc(fname, dirLen + maxName +1);
        		if(fname == NULL) {
        			Error("CWEB::SESSION - In reallocation memory "
        			"for reading file session name.\n"
					"The length of file session name is %d.\nFile session name is \"%s\""
					"Directory of Session File is \"%s\"\nLength of directory name is %d\n"
					"The total size requeried is %d\nerro is %d\nstr erro is \"%s\"",
					flen, ep->d_name, this->dir, dirLen, dirLen+maxName+1,
					errno, strerror(errno));
        		}
        	}
        
			sprintf(fname, "%s%s", this->dir, ep->d_name);
        	//printf("\n\ndir is \"%s\"\nep->d_name is \"%s\"\nfname is \"%s\"\n", dirName, ep->d_name, fname);
			FILE *f = fopen(fname, "rb");
	        if(f == NULL) {
   				Error("CWEB::SESSION - Could not open file session.\n"
   				"File Session is \"%s\"\nerro is %d\nstr erro is \"%s\"",
   				fname, errno, strerror(errno));
   			}
   		
   			bool isDel = is_to_delete(del, fname, f);
   			int errn = fclose(f);
   			if(errn != 0) {
   				Error("CWEB::SESSION - In close file session.\nFile session name is \"%s\"\n"
				"erro is %d\nstr erro is \"%s\"",
				fname, errno, strerror(errno));
   			}
   			if(isDel == true) {
   				errn = remove(fname);
   				if(errn != 0) {
   					Error("WEB::SESSION - In remove file session .\n"
   					"File session name is \"%s\"\nerro is %d\nstr erro is \"%s\"",
					fname, errno, strerror(errno));
   				}
   			}
    	}
    
    	(void) closedir (dir);
		#endif
	}
	
	bool is_session_file(const char *fname)
	{
		if(fname == NULL) return false;
	
		int len = strlen(fname);
	
		if(len < 9) return false;
	
		// check the extension of file
		if(fname[len -8] != '.') return false;
		if(fname[len -7] != 's') return false;
		if(fname[len -6] != 'e') return false;
		if(fname[len -5] != 's') return false;
		if(fname[len -4] != 's') return false;
		if(fname[len -3] != 'i') return false;
		if(fname[len -2] != 'o') return false;
		if(fname[len -1] != 'n') return false;
	
		return true;
	}
	
	bool is_to_delete(const time_t del,
					  const char *fname,
					  FILE *f)
	{
		time_t ts;
		if(fread(&ts, sizeof(time_t), 1, f) != 1) {
			Error("WEB::SESSION - Reading session time "
			"of delete routine for old file session.\n"
			"Session File is \"%s\"\nFile pointer is %p\nerro is %d\nstr erro is \"%s\"",
			fname, f, errno, strerror(errno));
		}
	
		char strTimeSession[26];
		if(fread(strTimeSession, sizeof(char), 26, f) != 26) {
			Error("WEB::SESSION - Reading session string time of "
			"delete routine for old file session.\nSession File is \"%s\"\n"
			"Session Time is %li\nFile pointer is %p\nerro is %d\nstr erro is \"%s\"",
			ts, fname, f, errno, strerror(errno));
		}
	
		time_t current = time(NULL);
		char strc[26];
		strcpy(strc, ctime(&current)); // it is necessary, sometimes ctime gives errors.

		time_t diff = current - ts;
		//printf("current is %li\nts is %li\ndiff is %li\nDel is %li\n", current, ts, diff, del);
		//printf("current str is %sts str is %s", strc, strTimeSession);
		if(diff > del) {
			return true;
		}
	
		return false;
	}
	
	FILE* getFile()
	{
		const char *sid = cweb::cookie::get("sid");
		if(sid == NULL) {
			return NULL;
		}
	
		if(dir == NULL) // Session can be not config - so dir is NULL
		{
			dir = new char[1];
			strncpy(dir, "", 1); // dir is set as the current directory
		}
		
		int size = strlen(dir) + strlen(sid) + 9; // 9 = strlen(".session") + '\0'
		fname = new char[size];
		if(fname == NULL) {
			Error("CWEB::SESSION - Allocated Space for Sesion File Name.\n Size is %d\n"
			"erro is %d\nstr erro is \"%s\"", size, errno, strerror(errno));
		}
		// recebe o nome do arquivo
		sprintf(fname, "%s%s.session", dir, sid);
	
		return fopen(fname, "rb");
	}
	
	bool is_expired(FILE *f)
	{
		time_t ts;
	
		if(fread(&ts, sizeof(time_t), 1, f) != 1) {
			Error("CWEB::SESSION - Reading session time of creation.\n"
			"Session File is \"%s\"\nFile pointer is %p\nerro is %d\nstr erro is \"%s\"",
			fname, f, errno, strerror(errno));
		}
	
		char strTimeSession[26];
	
		if(fread(strTimeSession, sizeof(char), 26, f) != 26) {
			Error("CWEB::SESSION - Reading session string time of creation.\n"
			"Session File is \"%s\"\nSession Time is %li\nFile pointer is %p\nerro is %d\n"
			"str erro is \"%s\"", ts, fname, f, errno, strerror(errno));
		}
	
		time_t current = time(NULL);
		char strc[26];
		strcpy(strc, ctime(&current)); // it is necessary, sometimes ctime gives errors.

		time_t diff = current - ts;

		if(diff > life) {
			/*Warn("CWEB::SESSION - This section is expired.\nSession life is %li\n"
			"Current is %li\nTime Session is %li\nTime difference is %li",
			session->life, current, ts, diff);*/
			return true;
		}
		
		return false;
	}
	
	void load_map(FILE *f)
	{
		int numKey = -1;
   		if(fread(&numKey, sizeof(int), 1, f) != 1) {
   			Error("CWEB::SESSION - Reading the number  map key.\n"
			"Session File is \"%s\"\nFile pointer is %p\nerro is %d\nstr erro is \"%s\"",
			fname, f, errno, strerror(errno));
		}
		//fprintf(stderr, "%s::numKey is %d\n", __func__, numKey);
   		if(numKey < 1) { // check value
   			return; // session file has no content
			//Error("number of map key is less than 1.\n"
			//"number of map key is %d", numKey);
		}
		
   		for(int i=0; i < numKey; ++i)
		{
   			//fprintf(stderr, "%s::[%d] of %d\n", __func__, i+1, numKey);
	   		int len = -1;
   			if(fread(&len, sizeof(int), 1, f) != 1) {
				Error("CWEB::SESSION - Reading the length of map key.\nid of key is %d\n"
				"Session File is \"%s\"\nFile pointer is %p\nerro is %d\nstr erro is \"%s\"",
				i, fname, f, errno, strerror(errno));
			}
			//fprintf(stderr, "%s::len is %d\n", __func__, len);
		
   			if(len < 2) {
				Error("CWEB::SESSION - Session size of length of key map is less than 2\n"
				"size of length of key map is is %d\n"
				"Session File is \"%s\"\nFile pointer is %p\nNumber of Key is %d",
				len, fname, f, numKey);
			}
			
			char *key = new char[len];
			if(key == NULL) {
				Error("CWEB::SESSION - In allocation memory for session content key.\n"
				"Key size is %d bytes\nSession File is \"%s\"\nFile pointer is %p\n"
				"erro is %d\nstr erro is \"%s\"", len, fname, f, errno, strerror(errno));
			}
   			if(fread(key, sizeof(char), len, f) != static_cast<size_t>(len)) {
				Error("CWEB::SESSION - Reading map key.\nid of key is %d\n"
				"Length of key is %dSession File is \"%s\"\nFile pointer is %p\n"
				"erro is %d\nstr erro is \"%s\"", i, len, fname, f, errno, strerror(errno));
			}
		
   			//fprintf(stderr, "%s::key is \"%s\"\n", __func__, key);
		
			Content *cont = new Content;
			if(cont == NULL) {
				Error("CWEB::SESSION - In allocation memory for session content.\n"
				"Key is \"%s\"\nSession File is \"%s\"\nFile pointer is %p\n"
				"erro is %d\nstr erro is \"%s\"", key, fname, f, errno, strerror(errno));
			}
		
   			if(fread(&cont->size, sizeof(cont->size), 1, f) != 1) {
				Error("CWEB::SESSION - Reading session data content size on session file.\n"
				"key is \"%s\"\nSession File is \"%s\"\nFile pointer is %p\n"
				"erro is %d\nstr erro is \"%s\"", key, fname, f, errno, strerror(errno));
			}
			//fprintf(stderr, "%s::size is \"%d\"\n", __func__, cont->size);
			if(cont->size < 1) {
				Error("CWEB::SESSION - Session size of value of key session is less than 1\n"
				"Size of memory to alloc value is %li\n"
				"Session File is \"%s\"\nFile pointer is %p\nKey is \"%s\"",
				cont->value, fname, f, key);
			}
			
			if(fread(&cont->numElem, sizeof(cont->numElem), 1, f) != 1) {
				Error("CWEB::SESSION - Reading session data content number of elements "
				"on session file.\nkey is \"%s\"\nSize of each element of this content "
				"is %li\nSession File is \"%s\"\nFile pointer is %p\nerro is %d\n"
				"str erro is \"%s\"\nContent in Session File is write as array. "
				"Each content is an array\nFor more details see documentation.", 
				key, cont->size, fname, f, errno, strerror(errno));
			}
			
			//fprintf(stderr, "%s::numElem is \"%d\"\n", __func__, cont->numElem);
			if(cont->numElem < 1) {
				Error("CWEB::SESSION - Session number of elements in content of value "
				"of key session is less than 1\nNumber of elements in array is %li\n"
				"Size of each element of this array is %li\nSession File is \"%s\"\n"
				"File pointer is %p\nKey is \"%s\""
				"Content in Session File is write as array. Each content is an array\n"
				"For more details see documentation.",
				cont->numElem, cont->value, fname, f, key);
			}
		
			cont->value = malloc(cont->size * cont->numElem);
	   		if(cont->value == NULL) {
				Error("CWEB::SESSION - In allocation memory "
				"for reading value of key of session.\n"
				"The value of memory to value is %li.\n"
				"Size of each element of content is %li\n"
				"Number of elements of array that represent this content is %li\n"
				"key of this content is \"%s\"\n"
				"Session File is \"%s\"\nFile pointer is %p\n"
				"erro is %d\nstr erro is \"%s\""
				"Content in Session File is write as array. Each content is an array\n"
				"For more details see documentation.",
				cont->size * cont->numElem, cont->size, cont->numElem, key, fname, f,
				errno, strerror(errno));
			}
			
   			if(fread(cont->value, cont->size, cont->numElem, f) 
   				!= static_cast<size_t>(cont->numElem)) {
				Error("CWEB::SESSION - Reading session data content on session file.\n"
				"The value of memory to value is %li.\n"
				"Size of each element of content is %li\n"
				"Number of elements of array that represent this content is %li\n"
				"key of this content is \"%s\"\n"
				"Session File is \"%s\"\nFile pointer is %p\n"
				"erro is %d\nstr erro is \"%s\""
				"Content in Session File is write as array. Each content is an array\n"
				"For more details see documentation.",
				cont->size * cont->numElem, cont->size, cont->numElem, key, fname, f,
				errno, strerror(errno));
			}
			//fprintf(stderr, "%s::value is \"%s\"\n", __func__, (char*)cont->value);
   			_map[key] = cont; // insere no map - map keep the reference not the pointer
		}
	}
	
	bool save_header(FILE *f)
	{
		time_t current = time(NULL);
		if(fwrite(&current, sizeof(time_t), 1, f) != 1) {
			Error("CWEB::SESSION - Writting session time of creation.\nTime is %li\n"
			"Str Time is \"%s\"\nerro is %d\nstr erro is \"%s\"",
			current, ctime(&current), errno, strerror(errno));
		}
	
		if(fwrite(ctime(&current), sizeof(char), 26, f ) != 26) {
			Error("CWEB::SESSION - Writting session name string time of creation\n"
			"Time is %lu\nStr Time is \"%s\"\nerro is %d\nstr erro is \"%s\"",
			current, ctime(&current), errno, strerror(errno));
		}
	
		int numKey = static_cast<int>(_map.size());
		if(fwrite(&numKey, sizeof(int), 1, f) != 1) {
			Error("CWEB::SESSION - Writting the number of session map key.\n"
			"number of key is %lu\nerro is %d\nstr erro is \"%s\"",
			numKey, errno, strerror(errno));
		}
		
		if(numKey < 1) {
			Warn("CWEB::SESSION - Nothing to save in Session File.\nNumber of Keys is %d",
			 numKey);
			return false;
		}
	
		return true;
	}
	
	void save_body(FILE *f)
	{
		for(auto elem : _map) // percorre todos os elementos do map para salvar
		{
   			int len = strlen(elem.first) +1;
   			//fprintf(stderr, "%s::key len is \"%d\"\n", __func__, len);
			if(fwrite(&len, sizeof(int), 1, f) != 1) {
				Error("CWEB::SESSION - Writting the length of session map key.\n"
				"key is \"%s\"\nLength is %d\nerro is %d\nstr erro is \"%s\"",
				elem.first, len, errno, strerror(errno));
			}
			
			//fprintf(stderr, "%s::key is \"%s\"\n", __func__, elem.first);
			if(fwrite(elem.first, sizeof(char), len, f) != static_cast<size_t>(len)) {
				Error("CWEB::SESSION - Writting session name variable on session file.\n"
				"key is \"%s\"\nkey length is %lu\nerro is %d\nstr erro is \"%s\"",
				elem.first, len, errno, strerror(errno));
			}
			
			//fprintf(stderr, "%s::size is \"%d\"\n", __func__, elem.second->size);
			if(fwrite(&elem.second->size, sizeof(elem.second->size), 1, f) != 1) {
				Error("CWEB::SESSION - Writting session data content size on session file.\n"
				"key is \"%s\"\nsize of value is %lu\nerro is %d\nstr erro is \"%s\"",
				elem.first, elem.second->size, errno, strerror(errno));
			}
			
			//fprintf(stderr, "%s::numElem is %d\n", __func__, elem.second->numElem);
			if(fwrite(&elem.second->numElem, sizeof(elem.second->numElem), 1, f) != 1) {
				Error("CWEB::SESSION - Writting session data content number of elements "
				"of this array (this content) on session file.\nNumber of elements is %li\n"
				"size of each element of array (content) is %li\nkey is \"%s\"\nerro is %d\n"
				"str erro is \"%s\"",
				elem.second->numElem, elem.second->size, elem.first, errno, strerror(errno));
			}
			
			//fprintf(stderr, "%s::value is \"%s\"\n", __func__, (char*)elem.second->value);
			if(fwrite(elem.second->value, elem.second->size, elem.second->numElem, f)
				!= static_cast<size_t>(elem.second->numElem)) {
				Error("CWEB::SESSION - Writting session data content on session file.\n"
				"key is \"%s\"\nsize of value is %lu\nNumber of elements is %li\n"
				"erro is %d\nstr erro is \"%s\"",
				elem.first, elem.second->size, elem.second->numElem, errno, strerror(errno));
			}
			
		}
	}
	
	void log_print_no_key_map(const char *key)
	{
		Warn("CWEB::SESSION - Fetch for a no key of Session.\n"
		"fectch key = \"%s\"\nnumber of keys is %d\n"
		"List of all keys in Session that be parsed:", key, _map.size());
		
		for(auto elem : _map)
		{
   			fprintf(stderr, "[\"%s\"] = \"%s\"\n", elem.first, (char*)elem.second->value);
		}
	}
};


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
static Session _session;


////////////////////////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////////////////////////
void
cweb::session::config(
	const char *dirFileSession,
	const time_t lifeSession,
	const time_t del)
{
	return _session.config(dirFileSession, lifeSession, del);
}

bool
cweb::session::load()
{
	return _session.load();
}

char*
cweb::session::sid()
{
	return _session.sid();
}

char*
cweb::session::save(const bool toSave)
{
	return _session.save(toSave);
}

bool
cweb::session::set(
	const char *key,
	const void *value,
	const int size,
	const int numElem)
{
	return _session.set(key, value, size, numElem);
}

void*
cweb::session::getv(
	const char *key,
	int *numElem,
	int *size)
{
	return _session.get(key, numElem, size);
}

void*
cweb::session::delv(
	const char *key,
	int *numElem,
	int *size)
{
	return _session.del(key, numElem, size);
}

int
cweb::session::clean()
{
	return _session.clean();
}











