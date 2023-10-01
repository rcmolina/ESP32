#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

char	buf[8192];	// I/O buffer
int	ec;		// exit code

// pe(format_string, format_string_argument, exit_code_modifier);
void
pe(	const char	*fmt,
	const char	*arg,
	int		ecm) {
	int	serrno;	// hold area for errno

	serrno = errno;
	snprintf(buf, sizeof(buf), fmt, arg);
	errno = serrno;
	perror(buf);
	ec |= ecm;
}

// NAME		dtrch -- Delete trailing repeated bytes.
//
// SYNOPSIS	dtrch file...
//
// DESCRIPTION	Delete trailing repeated bytes from each file named as an operand.
//		File are updated in place.
//
// OPERANDS
//	file	A pathname of a file to be truncated to have a length that does
//		not include any trailing NUL bytes.
//
// INPUT FILES	The input files must be regular files.
//
// STDERR	The standard error shall be used only for diagnostic messages.
//
// EXIT STATUS
//	0	All input files were successfully processed.
//	>0	An error occurred.
//
// CONSEQUENCES OF ERRORS
//		Default.

int
main(	int	argc,
	char	*argv[]) {

	ssize_t	buflen;	// number of bytes in buf[]
	int	fd,	// file descriptor
		i,	// loop control
		j;	// loop control
	off_t	nsize,	// new file size
		size;	// current file size
	char	lastch = 0;

	for(i = 1; i < argc; i++) {
		//if((fd = open(argv[i], O_RDWR)) == -1) {
		if((fd = open(argv[i], O_RDWR+O_BINARY)) == -1) {	 	 
			pe("Can't open \"%s\":", argv[i], 1);
			continue;
		}
		nsize = size = 0;
		
		lseek(fd,-1,SEEK_END);
		read(fd, buf, 1);
		lastch=buf[0];
		lseek(fd,0,SEEK_SET);
		
		
		while((buflen = read(fd, buf, sizeof(buf))) > 0) {
			for(j = 0; j < buflen; j++) {
				size++;
				if(buf[j] != lastch)							// en cuanto aparece un char distinto, recupera todo el tamaño descartado
					nsize = size;
				//printf("j=%d size=%d nsize=%d\n",j,size,nsize);
			}
		}
		
		nsize++;
		if(buflen) {
			pe("Read error on \"%s\": file will not be truncated:",
			    argv[i], 2);
		} else if(ftruncate(fd, nsize)) {
			pe("Truncation failed on \"%s\":", argv[i], 4);
		}
		close(fd);
	}
	return ec;
}

