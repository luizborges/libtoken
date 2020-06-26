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
		void* value = nullptr;
		int size = 0;
		int numElem = 0;
	};
	
	////////////////////////////////////////////////////////////////////////////////
	// inner variables
	////////////////////////////////////////////////////////////////////////////////
	char *dir   = nullptr;
	char *fname = nullptr;
	char *_sid   = nullptr;
	bool toSave = false;
	time_t life = 5*60; // 5 minutos tempo default de duração da sessão
	map<const char*, Content*, cmp_str> _map;
	
	std::unique_ptr<char[]> fname_old;
 public:
 	Session()
 	{
 		dir = new char[1];
 		strncpy(dir, "", 1); // inicializa como um string vazia.
 	}
 
	void config(const char *dirFileSession = "",
				const time_t lifeSession = 5*60, // valor default de 5 min
				const time_t del = 7*60) // valor default de 7 min);
	{ try {
		setDir(dirFileSession);
		life = lifeSession;
		remove_old_files(del);
	 } catch(u::error& e) { throw err(); }
	}
	
	bool load()
	{ try {
		FILE* fs = getFile();
	   	if(fs == nullptr) {
   			Warn("CWEB::SESSION - File Session is \"%s\"\nerro is %d\nstr erro is \"%s\"",
			fname, errno, strerror(errno));
			return false;
   		}
   	
   		if(is_expired(fs) == true) {
   			return false;
   		}
   	
   		load_map(fs);
   		u::fclose(fs);
   		////////////////////////////////////////////////////////////////////////////////
		// copy the fname to fname_old to remove it when cretates a new one
		////////////////////////////////////////////////////////////////////////////////
		auto size = std::strlen(fname) +1;
		fname_old.reset(new char[size]); // +1 to \0
		std::strcpy(fname_old.get(), fname);
		
   		////////////////////////////////////////////////////////////////////////////////
		// force call save() or sid()
		////////////////////////////////////////////////////////////////////////////////
		_sid = nullptr;
		toSave = false;
   		
   		return true;
   	 } catch(u::error& e) { throw err();
	 } catch(std::bad_alloc& e) { throw err("std::bad_alloc : \"%s\"", e.what()); }
	}
	
	char* sid()
	{ try {
		char *nsec = nullptr; // nano seconds
		bool swap  = false;
		#if defined(unix) || defined(__unix) || defined(__unix__) || (defined (__APPLE__) && defined (__MACH__)) // Unix (Linux, *BSD, Mac OS X)
		
		#ifdef _SC_MONOTONIC_CLOCK
		struct timespec ts;
		if (sysconf (_SC_MONOTONIC_CLOCK) > 0) {
	    	if (clock_gettime (CLOCK_MONOTONIC, &ts) == 0)
	    	{
	    		int size = sizeof(ts.tv_nsec) +1;
	  _  		nsec = new char[size];
	    		snprintf(nsec, size, "%ld", ts.tv_nsec);
	    		swap = ((ts.tv_nsec%2) == 0) ? false : true;
        	}
		}
		
		#endif
		#endif// for windows and olther system
		if(nsec == nullptr)
		{
	 _		nsec = new char[1];
			*nsec = '\0';
		}
		srand(time(nullptr));
		int al = rand();
		time_t current = time(nullptr);
		int sl = 5*sizeof(int)+sizeof(time_t)+strlen(nsec)+1;
	 _	_sid = new char[sl];
		
		// insere outra camada de aleatoriedade na criação do sid
		int _pid = getpid();
		bool spid = ((_pid%2) == 0) ? false : true;
		if(swap && spid) {
			snprintf(_sid, sl, "%li%s%d%d%d", current, nsec, al, _pid, getppid());
		} else if(swap && !spid) {
			snprintf(_sid, sl, "%li%d%s%d%d", current, al, nsec, getppid(), _pid);
		} else if(!swap && spid) {
			snprintf(_sid, sl, "%li%d%s%d%d", current, al, nsec, _pid, getppid());
		} else {
			snprintf(_sid, sl, "%li%d%s%d%d", current, al, nsec, getppid(), _pid);
		}
		
	 	int size = strlen(dir) + strlen(_sid) + strlen(".session") +1;
		/*Warn("++SIZE IS %d\ndir is \"%s\"\nstrlen(dir) = %d\nsid is \"%s\"\nstrlen(sid) is %d\nnsec is \"%s\"\nstrlen(nsec) = %d\nsid size is %d",
		size, dir, strlen(dir), sid, strlen(sid), nsec, strlen(nsec), sl);
		*/
	 _	fname = new char[size];
		snprintf(fname, size, "%s%s.session", dir, _sid);
		
		////////////////////////////////////////////////////////////////////////////////
		// set to save the session
		////////////////////////////////////////////////////////////////////////////////
		toSave = true;
		
		return _sid;
	 } catch(u::error& e) { throw errl(u::line);
	 } catch(std::bad_alloc& e) {
 		throw errl( u::line, "std::bad_alloc : \"%s\"", e.what());
 	 }
	}
	
	char* save(const bool _toSave = false)
	{ try {
		toSave = (toSave || _toSave) ? true:false;
		////////////////////////////////////////////////////////////////////////////////
		// remove the current file session name
		////////////////////////////////////////////////////////////////////////////////
		/*if(fname != nullptr && toSave != true)
		{
   			u::remove(fname);
			fname = nullptr;
		}*/
		////////////////////////////////////////////////////////////////////////////////
		// create file session
		////////////////////////////////////////////////////////////////////////////////
		if(toSave == false) return nullptr;
		
		if(_sid == nullptr) {
			sid();
		}
		
 		FILE *f = u::fopen(fname, "wb");
		
 		bool hasBody = save_header(f);
 		if(hasBody == true) {
 			save_body(f);
 		}
    	
    	u::fclose(f);
    	
    	////////////////////////////////////////////////////////////////////////////////
		// remove the old current file session name - if new was created
		////////////////////////////////////////////////////////////////////////////////
    	if(fname_old.get() != nullptr)
    	{
    		//u::remove(fname_old.get());
    		fname_old.reset();
    	}
    	
 		return _sid;
 	 } catch(u::error& e) { throw err(); }
	}
	
	bool set(
		const char *key = nullptr,
		const void *value = nullptr,
		const int size = 0,
		const int numElem = 0)
	{ try {
		if(key == nullptr)   return false;
		if(value == nullptr) return false;
		if(size < 1)      return false;
		if(numElem < 1)   return false;
		
		Content* cont;
		auto it = _map.find(key);
		if (it != _map.end()) {
			cont = it->second;
		} else {
			cont = new Content;
		}
				
		cont->value = const_cast<void*>(value);
		cont->size  = size;
		cont->numElem = numElem;
		_map[key] = cont;
		
	  /*	auto elem = _map.find(key);
		fprintf(stderr, "\n%s\n", __PRETTY_FUNCTION__);
   			fprintf(stderr, "key=\"%s\"\n", elem->first);
   			fprintf(stderr, "len=\"%d\"\n", strlen(elem->first)+1);
   			fprintf(stderr, "size=\"%d\"\n", elem->second->size);
   			fprintf(stderr, "numElem=\"%d\"\n", elem->second->numElem);
   			fprintf(stderr, "val=\"%s\"\n", (char*)elem->second->value);
   			fprintf(stderr, "val=\"%s\"\n", (char*)value);
   			fprintf(stderr, "val=\"%p\"\n", value);
	  */	
	  //	log_print_no_key_map("");
		return true;
	 } catch(u::error& e) { throw err();
	 } catch(std::bad_alloc& e) { throw err("std::bad_alloc : \"%s\"", e.what()); }
	}
	
	void* get(
		const char *key = nullptr,
		int *numElem = nullptr,
		int *size = nullptr)
	{ try {
		if(key == nullptr)
		{
			log_print_no_key_map(key);
			return nullptr;
		} 
		
		auto it = _map.find(key);
		if (it == _map.end()) // key not found
		{
			log_print_no_key_map(key);
			return nullptr;
		}
		
		if(numElem != nullptr) {
			*numElem = it->second->numElem;
		}
		if(size != nullptr) {
			*size = it->second->size;
		}
		
		return it->second->value;
	 } catch(u::error& e) { throw err(); }
	}
	
	void* del(
		const char *key = nullptr,
		int *numElem = nullptr,
		int *size = nullptr)
	{ try {
		if(key == nullptr)
		{
			log_print_no_key_map(key);
			return nullptr;
		} 
		
		auto it = _map.find(key);
		if (it == _map.end()) // key not found
		{
			log_print_no_key_map(key);
			return nullptr;
		}
		
		if(numElem != nullptr) {
			*numElem = it->second->numElem;
		}
		if(size != nullptr) {
			*size = it->second->size;
		}
		
		const char *_key = it->first;
		Content* c = it->second;
		void *value = it->second->value;
		_map.erase(it);
		delete[] _key; // libera a memória
		delete   c;
		return value;
	 } catch(u::error& e) { throw err(); }
	}
	
	int clean()
	{ try {
		auto numKeys = _map.size();
		_map.clear();
		return numKeys;
	 } catch(...) { throw err(); }
	}
 
 private:
  	void setDir(const char *dirFileSession)
	{
		if(dirFileSession == nullptr) return;
		
		int size = strlen(dirFileSession);
		dir = new char[size+1];
		if(dir == nullptr) {
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
			if(dir == nullptr) {
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

	 try {	
	 	DIR *dir = opendir (this->dir); // open de directory
		if(dir == nullptr) {
			throw err("CWEB::SESSION - Couldn't open the directory where Session files is.\n"
			"Directory name passed is \"%s\"\nerro is %d\nstr erro is \"%s\"",
			this->dir, errno, strerror(errno));
		}
	
		// create array to keep file session name
		int dirLen = strlen(this->dir);
		int maxName = 29;
	 _	char *fname = u::p(new(std::nothrow) char[dirLen + maxName +1]);
	
		// read each file in directory to remove the file how match the condition
		struct dirent *ep;
		while ((ep = readdir(dir)) != nullptr)
		{
    	    bool isS = is_session_file(ep->d_name);
    	    if(isS == false) continue;
        
    	    int flen = strlen(ep->d_name);
       		// verifia se a string pode suportar o tamanho do diretorio + do nome do arquivo
        	if(flen > maxName) { 
        		maxName = flen;  
        _		fname = u::p(static_cast<char*>(realloc(fname, dirLen + maxName +1)));
        	}
        
			sprintf(fname, "%s%s", this->dir, ep->d_name);
        	//printf("\n\ndir is \"%s\"\nep->d_name is \"%s\"\nfname is \"%s\"\n", dirName, ep->d_name, fname);
		_	FILE *f = u::fopen(fname, "rb");
   			bool isDel = is_to_delete(del, fname, f);
   		_	u::fclose(f);
   			if(isDel == true) {
   		_		u::remove(fname);
   			}
    	}
    
    	(void) closedir (dir);
     } catch(u::error& e) { throw errl(u::line); }
		#endif
	}
	
	bool is_session_file(const char *fname)
	{
		if(fname == nullptr) return false;
	
		auto len = strlen(fname);
	
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
	{ try {
		time_t ts;
	 _	u::fread(&ts, sizeof(time_t), 1, f);
	
		char strTimeSession[26];
	 _	u::fread(strTimeSession, sizeof(char), 26, f);
	
		time_t current = time(nullptr);
		char strc[26];
		strcpy(strc, ctime(&current)); // it is necessary, sometimes ctime gives errors.

		time_t diff = current - ts;
		//printf("current is %li\nts is %li\ndiff is %li\nDel is %li\n", current, ts, diff, del);
		//printf("current str is %sts str is %s", strc, strTimeSession);
		if(diff > del) {
			return true;
		}
		return false;
	 } catch(u::error& e) { throw errl(u::line); }
	}
	
	FILE* getFile()
	{ try {
		const char *sid = cweb::cookie::get("sid");
		if(sid == nullptr) {
			return nullptr;
		}
	
		if(dir == nullptr) // Session can be not config - so dir is nullptr
		{
			dir = new char[1];
			strncpy(dir, "", 1); // dir is set as the current directory
		}
		
		int size = strlen(dir) + strlen(sid) + 9; // 9 = strlen(".session") + '\0'
		fname = new char[size];
		// recebe o nome do arquivo
		sprintf(fname, "%s%s.session", dir, sid);
	
		return fopen(fname, "rb");
	 } catch(u::error& e) { throw err(); }
	 catch(std::bad_alloc& e) {
 		throw err("std::bad_alloc : \"%s\"", e.what());
 	 }
	}
	
	bool is_expired(FILE *f)
	{ try {
		time_t ts;
	
	 _	u::fread(&ts, sizeof(time_t), 1, f);
	
		char strTimeSession[26];
	
	 _	u::fread(strTimeSession, sizeof(char), 26, f);
	
		time_t current = time(nullptr);
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
	 } catch(u::error& e) { throw errl(u::line); }
	}
	
	void load_map(FILE *f)
	{ try {
		int numKey = -1;
   	 _	u::fread(&numKey, sizeof(int), 1, f);
		//fprintf(stderr, "%s::numKey is %d\n", __func__, numKey);
   		if(numKey < 1) { // check value
   			return; // session file has no content
		}
		
   		for(int i=0; i < numKey; ++i)
		{
   			//fprintf(stderr, "%s::[%d] of %d\n", __func__, i+1, numKey);
	   		int len = -1;
   		 _	u::fread(&len, sizeof(int), 1, f);
		
   		 	if(len < 2) {
		 _		throw err("CWEB::SESSION - "
				"Session size of length of key map is less than 2\n"
				"size of length of key map is is %d\n"
				"Session File is \"%s\"\nFile pointer is %p\nNumber of Key is %d",
				len, fname, f, numKey);
			}
			
		 _	char *key = new char[len];
   		 _	u::fread(key, sizeof(char), len, f);
		
		 _	Content *cont = new Content;
		
   		 _	u::fread(&cont->size, sizeof(cont->size), 1, f);
   			
			//fprintf(stderr, "%s::size is \"%d\"\n", __func__, cont->size);
			if(cont->size < 1) {
				throw err("CWEB::SESSION - "
				"Session size of value of key session is less than 1\n"
				"Size of memory to alloc value is %li\n"
				"Session File is \"%s\"\nFile pointer is %p\nKey is \"%s\"",
				cont->value, fname, f, key);
			}
			
		 _	u::fread(&cont->numElem, sizeof(cont->numElem), 1, f);
			
			//fprintf(stderr, "%s::numElem is \"%d\"\n", __func__, cont->numElem);
			if(cont->numElem < 1) {
				throw err("CWEB::SESSION - Session number of elements in content of value "
				"of key session is less than 1\nNumber of elements in array is %li\n"
				"Size of each element of this array is %li\nSession File is \"%s\"\n"
				"File pointer is %p\nKey is \"%s\""
				"Content in Session File is write as array. Each content is an array\n"
				"For more details see documentation.",
				cont->numElem, cont->value, fname, f, key);
			}
		
		 _	cont->value = u::p(malloc(cont->size * cont->numElem));
			
   		 _	u::fread(cont->value, cont->size, cont->numElem, f);
			//fprintf(stderr, "%s::value is \"%s\"\n", __func__, (char*)cont->value);
   			_map[key] = cont; // insere no map - map keep the reference not the pointer
		}
	 } catch(u::error& e) { throw errl(u::line); }
	 catch(std::bad_alloc& e) { throw errl(u::line, "std::bad_alloc : \"%s\"", e.what()); }
	}
	
	bool save_header(FILE *f)
	{ try{
		time_t current = time(nullptr);
	 _	u::fwrite(&current, sizeof(time_t), 1, f);
	 _	u::fwrite(ctime(&current), sizeof(char), 26, f );
	
		int numKey = static_cast<int>(_map.size());
		//fprintf(stderr, "NUMKEY=%d\n", numKey);
	 _	u::fwrite(&numKey, sizeof(int), 1, f);
		
		if(numKey < 1) {
			Warn("CWEB::SESSION - Nothing to save in Session File.\nNumber of Keys is %d",
			 numKey);
			return false;
		}
	
		return true;
	 } catch(u::error& e) { throw errl(u::line); }
	}
	
	void save_body(FILE *f)
	{ try { //log_print_no_key_map("");
		for(auto elem : _map) // percorre todos os elementos do map para salvar
		{
   			int len = strlen(elem.first) +1;
   			
   		/*	fprintf(stderr, "\n%s\n", __PRETTY_FUNCTION__);
   			fprintf(stderr, "key=\"%s\"\n", elem.first);
   			fprintf(stderr, "len=\"%d\"\n", len);
   			fprintf(stderr, "size=\"%d\"\n", elem.second->size);
   			fprintf(stderr, "numElem=\"%d\"\n", elem.second->numElem);
   			fprintf(stderr, "val=\"%s\"\n", elem.second->value);
  		*/	
		 _	u::fwrite(&len, sizeof(int), 1, f);
		 _	u::fwrite(elem.first, sizeof(char), len, f);
		 _	u::fwrite(&elem.second->size, sizeof(elem.second->size), 1, f);
		 _	u::fwrite(&elem.second->numElem, sizeof(elem.second->numElem), 1, f);
		 _	u::fwrite(elem.second->value, elem.second->size, elem.second->numElem, f);
		}
	 } catch(u::error& e) { throw errl(u::line); }
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
cweb::session::setv(
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











