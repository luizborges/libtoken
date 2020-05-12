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
	time_t life = 5*60; // 5 minutos tempo default de duração da sessão
	int maxLenMapKey = 0;
	map<const char*, Content*, cmp_str> _map;

 public:
	void config(const char *dirFileSession = "",
				const time_t lifeSession = 5*60, // valor default de 5 min
				const time_t del = 10*60) // valor default de 10 min);
	{
		setDir(dirFileSession);
		life = lifeSession;
		clean_old_files(del);
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
   		
   		return true;
	}
	
	char* save()
	{
	}
 private:
	void setDir(const char *dirFileSession)
	{
		if(dirFileSession == NULL)
		{
			dir = new char[1];
			strncpy(dir, "", 1);
		} else {
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
	}
	
	void clean_old_files(const time_t del)
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
		if(fread(&maxLenMapKey, sizeof(maxLenMapKey), 1, f) != 1) {
   			Error("CWEB::SESSION - Reading session the value of greatest length map key.\n"
			"Session File is \"%s\"\nFile pointer is %p\nerro is %d\nstr erro is \"%s\"",
			fname, f, errno, strerror(errno));
		}
	
   		if(maxLenMapKey < 2) { // check value
			Error("CWEB::SESSION - Session maximum length of key is less than 2.\n"
			"session maximum length of key is %d", maxLenMapKey);
		}
	
   		//fprintf(stderr, "%s::session->maxLenMapKey is %d\n", __func__, session->maxLenMapKey);
   		char *key = new char[maxLenMapKey];
	   	if(key == NULL) {
			Error("CWEB::SESSION - In allocation memory for reading key of session.\n"
			"The value of greatest length map key of session is %d.\n"
			"Session File is \"%s\"\nFile pointer is %p\nerro is %d\nstr erro is \"%s\"",
			maxLenMapKey, fname, f, errno, strerror(errno));
		}
	
		int numKey = -1;
   		if(fread(&numKey, sizeof(int), 1, f) != 1) {
   			Error("CWEB::SESSION - Reading the number  map key.\n"
			"Session File is \"%s\"\nFile pointer is %p\nerro is %d\nstr erro is \"%s\"",
			fname, f, errno, strerror(errno));
		}
	
   		if(numKey < 1) { // check value
			Error("number of map key is less than 2.\n"
			"number of map key is %d", numKey);
		}
	
		//fprintf(stderr, "%s::numKey is %d\n", __func__, numKey);
	
   		for(int i=0; i < numKey; ++i)
		{
   			//fprintf(stderr, "%s::[%d] of %d\n", __func__, i+1, numKey);
	   		int len = -1;
   			if(fread(&len, sizeof(int), 1, f) != 1) {
				Error("CWEB::SESSION - Reading the length of map key.\nid of key is %d\n"
				"Session File is \"%s\"\nFile pointer is %p\nerro is %d\nstr erro is \"%s\"",
				i, fname, f, errno, strerror(errno));
			}
		
   			if(len < 2) {
				Error("CWEB::SESSION - Session size of length of key map is less than 2\n"
				"size of length of key map is is %d\n"
				"Session File is \"%s\"\nFile pointer is %p\nNumber of Key is %d",
				len, fname, f, numKey);
			}
		
   			//fprintf(stderr, "%s::len is %d\n", __func__, len);
		
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
		
   			_map[key] = cont; // insere no map - map keep the reference not the pointer
		}
		
		delete[] key;
	}
};


////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
static Session _session;


////////////////////////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////////////////////////
extern void
cweb::session::config(
	const char *dirFileSession,
	const time_t lifeSession,
	const time_t del)
{
	return _session.config(dirFileSession, lifeSession, del);
}

extern bool
cweb::session::load()
{
	return _session.load();
}

extern char*
cweb::session::save()
{
	return _session.save();
}



















