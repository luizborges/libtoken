
#include <assert.h>
#include <headers/stackTracer.h>
#include <iostream>
#include <map> 
using namespace std;
#include <cweb.hpp>


#define _ printf("Get here...: %d\n", __LINE__);


void test1();
void test2();

int main(void) {
//	MM_Init();
	StackTracer_Init();
	
//	map<std::string, std::string> m;
	map<char*, char*> m;
	cweb::cookie::init();
	
	m["e1"] = "c1";
	char *cp = NULL;
	printf("m[\"%s\"] = \"%s\"\n", "e1", m["e1"]);
	printf("count(m[\"%s\"]) = %d\n", "e2", m.count("e2"));
	printf("count(m[\"%s\"]) = %d\n", NULL, m.count(NULL));
	printf("m[\"%s\"] = \"%s\"\n", "e2", m["e2"]);
	printf("count(m[\"%s\"]) = %d\n", "e2", m.count("e2"));
	printf("m[\"%s\"] = \"%s\"\n", NULL, m[NULL]);
	printf("m[\"%s\"] = \"%s\"\n", cp, m[cp]);
	
	test1();
	test2();
	
	return 0;
}

void test1() {
	printf("\n*******************************************************\n");
	printf("Func: %s\n", __func__);
	printf("*******************************************************\n");
	
	char *cookie = cweb::cookie::set("sid", 
		"my ful name", 180, "www.google.com",
		"/home/borges", true, true);
	printf("cookie 1::\n\"%s\"\n", cookie);
	time_t t = time(NULL);
	printf("time: %s\n", ctime(&t));

}

void test2() {

	printf("\n*******************************************************\n");
	printf("Func: %s\n", __func__);
	printf("*******************************************************\n");
	
	cweb::cookie::print("sid", 
		"my ful name", 180, "www.google.com",
		"/home/borges", true, true);
	time_t t = time(NULL);
	printf("time: %s\n", ctime(&t));

}






