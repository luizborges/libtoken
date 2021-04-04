/**
 * @author: luizborges
 * @descr: erase all define that overrides libc functions in fcgi_stdio.h
 * A biblioteca fcgi_stdio.h sobreescreve as funções originais da libC, com uma definição própria, o objetivo deste arquivo é retirar a definição, para a utilização original das funções da libC
 */

#undef 	FILE
 
#undef 	stdin   
 
#undef 	stdout   
 
#undef 	stderr   
 
#undef 	perror   
 
#undef 	fopen   
 
#undef 	fclose   
 
#undef 	fflush   
 
#undef 	freopen   
 
#undef 	setvbuf   
 
#undef 	setbuf   
 
#undef 	fseek   
 
#undef 	ftell   
 
#undef 	rewind   
 
#undef 	fgetpos  
 
#undef 	fsetpos  
 
#undef 	fgetc   
 
#undef 	getc   
 
#undef 	getchar   
 
#undef 	ungetc   
 
#undef 	fgets   
 
#undef 	gets   
 
#undef 	fputc   
 
#undef 	putc   
 
#undef 	putchar   
 
#undef 	fputs   
 
#undef 	puts   
 
#undef 	fprintf   
 
#undef 	printf   
 
#undef 	vfprintf  
 
#undef 	vprintf  
 
#undef 	fread   
 
#undef 	fwrite   
 
#undef 	feof   
 
#undef 	ferror  
 
#undef 	clearerr   
 
#undef 	tmpfile  
 
#undef 	fileno   
 
#undef 	fdopen   
 
#undef 	popen   
 
#undef 	pclose  


