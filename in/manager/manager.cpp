/**
 */
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include "../../cweb.hpp"

////////////////////////////////////////////////////////////////////////////////
// Private Structs
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
// Private Variables
////////////////////////////////////////////////////////////////////////////////
static char* (*_read) (const char*);

////////////////////////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////////////////////////
bool
cweb::in::init(const long max_size)
{
	char *rm = getenv("REQUEST_METHOD");
	if(rm == NULL) {
		MError("CWEB::IN - getenv(\"REQUEST_METHOD\") is NULL");
		return false;
	}
	
	// descobre qual o REQUEST_METHOD passado
	if(strcmp(rm, "GET") == 0) {
		_read = cweb::in::get;
		return cweb::in::init_get();;
	}
	else if(strcmp(rm, "POST") == 0) {
		char *ct = getenv("CONTENT_TYPE");
		if(ct == NULL) {
			Error("CWEB::IN - NO CONTENT_TYPE\ngetenv(\"CONTENT_TYPE\") = NULL");
		}
		if(strcmp(ct, "application/x-www-form-urlencoded") == 0) {
			_read = cweb::in::post;
			return cweb::in::init_post(max_size);
		} else
		if(strncmp(ct, "multipart/form-data", 19) == 0) {
			_read = NULL;
			return cweb::in::init_fpost(max_size);
		} else {
			Error("CWEB::IN - CONTENT_TYPE NOT IMPLEMENTED.\n"
			"getenv(\"CONTENT_TYPE\") = \"%s\".\n"
			"This library only implemented getenv(\"CONTENT_TYPE\") = "
			"\"application/x-www-form-urlencoded\" or \"multipart/form-data\"\n", ct);
		}
	}
	else {
		Error("CWEB::IN - REQUEST_METHOD do not recognize.\n"
			"REQUEST_METHOD = \"%s\"", rm);
	}
	
	return false;
}

char*
cweb::in::read(const char *key)
{
	return _read(key);
}







