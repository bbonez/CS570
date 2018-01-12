/**
 * Program Two and Four
 * Written by Stephanie Hernandez
 * For SDSU CS570 Fall 2017
 * Instructor John Carroll
 *
 * All documentation found in p2.h
 */

#include "getword.h"
#include "p2.h"

/************ Globals **************/

//structure
struct Process {
	char *argv[MAX_ARGS] ;
	int argc;
};

struct Process proc[MAX_PIPES] ; 	//buffer for processes
int length ; 				//length of proc structure

//flags
int iflag ; 			//input file flag
int oflag ; 			//output file flag
int bflag ; 			//background job flag
int owflag ;			//overwrite output file flag
int pipes ;				//counter

//buffer
char *buffer[MAX_ARGS] ; 	//buffer for input line
int buflen ; 				//length of buffer

//file descriptors
int fdin, fdout ;

//Files for redirection
char *infile,
	*outfile,
	*errfile ;

/****************** Code Begins Here *********************/
int parse () {
	//set globals
	pipes = 0 ;

	//locals
	int atpipe = 0 ;    //1:pointing to pipe char, 0: other char
	int ioflag = 0 ;
	int argc = 0 ;      //argument count for process
	int index = 0 ;     //index for proc struct
	char str[STORAGE] ; //holds word

	int i ;
	for (i = 0; i < buflen; i++) {  //traverse buffer
		strcpy(str, buffer[i]) ;

		//Error checking
		if ( ( (strcmp(str, ">!") == 0) || (strcmp(str, ROUT) == 0) ) && (outfile != NULL) ) {
			fprintf(stderr, "Ambiguous output redirect.\n") ;
			return -1 ;
		}
		if ( (strcmp(str, RIN) == 0) && (infile != NULL) ) {
			fprintf(stderr, "Ambiguous input redirect.\n") ;
			return -1	;
		}

		//set flags to skip over metacharacters during this run if any are found
		if (strcmp(str, PIPE) == 0) {
			atpipe = 1	;
			pipes++ ;
		}
		else if ( strcmp(str, ROUT) == 0 )		//set locals/globals
			ioflag = oflag = 1 ;
		else if ( strcmp(str, RIN) == 0 )		//set locals/globals
			ioflag = iflag = 1 ;
		else if ( strcmp(str, ">!") == 0 ) 		//set global
			owflag = ioflag = oflag = 1;
		else if ( strcmp(str, AMP) == 0 )		//set global bflag
			bflag = 1 ;

		//set variables
		else if ( (ioflag == 1) && (strcmp(buffer[i-1], RIN) == 0) ) { //input file
			infile = malloc(sizeof(char *) * STORAGE) ;
			strcpy(infile, str)	;
			ioflag = 0 ; 		//reset ioflag, read input file
		}
		else if ( (ioflag == 1) &&
                ( (strcmp(buffer[i-1], ROUT) == 0) ||
                (strcmp(buffer[i-1], ">!") == 0) ) ) { //output file
			outfile = malloc(sizeof(char *) * STORAGE) ;
			strcpy(outfile, str) ;
			ioflag = 0 ; 		//reset ioflag, read output file
		}
		else { //store each process and its arguments
			proc[index].argv[argc] = malloc(sizeof(char *) * STORAGE) ;
			strcpy(proc[index].argv[argc], str) ;
			argc++ ;
			proc[index].argc = argc ;
		}

		//break at pipes, store process in new index
		if ( atpipe ) {
			proc[index].argv[argc] = '\0';
			(i == buflen-1) ? : index++ ; //move index?
			argc = atpipe = 0 ; 		//reset local argc and atpipe
		}
	}//end for loop
	length = index + 1 ; //number of processes, +1 to account for start position 0

	//Error checking
	if (iflag && (infile == NULL) ) {	//redirect symbol but no file
		fprintf(stderr, "Missing name for redirect.\n") ;
		return -1 ;
	}
	if ( (oflag || owflag) && (outfile == NULL) ) {		//redirect symbol but no file
		fprintf(stderr, "Missing name for redirect.\n") ;
		return -1 ;
	}
	if (proc[0].argc == 0 && (iflag || oflag || bflag) ) {	//redirect/bflag and no command
		fprintf(stderr, "Invalid null command.\n") ;
		return -1 ;
	}
	if (pipes > 0 && ( (pipes + 1) != length) ){
		fprintf(stderr, "Missing name for redirect '|'.\n");
		return -1;
	}
	return 0 ; //successfully parsed the line
}


int process() {
	char *cmd = proc[0].argv[0] ;
	char *path = proc[0].argv[1] ;
	int argc = proc[0].argc;
	int i;

	//cd command
	if (strcmp(cmd, "cd") == 0) {
		if (argc > CD_ARGS || infile != NULL || outfile != NULL) {
			fprintf(stderr, "cd: Too many arguments\n") ;
			return -1 ;
		}
		if (argc == 2) {
			if (-1 == chdir(path)) {
				fprintf(stderr, "No such file or directory. \n") ;
				return -1 ;
			}
		}
		if (argc == 1) {
			char *home = getenv("HOME") ;
			chdir(home) ;
		}
	}

	//ls-F command
	else if (strcmp(cmd, "ls-F") == 0) {
		if (argc == 1) dirwalk(CWD) ; //just ls-F
		else {
			for (i = 1; i < argc; i++) {
				printf("\n%s::\n", proc[0].argv[i]);
				dirwalk(proc[0].argv[i]);
			}
		}
	}

	//exec command
	else if (strcmp(cmd, "exec") == 0) {
		execvp(proc[0].argv[0], proc[0].argv+1 ); //execvp(cargv[0], cargv) ;
		perror("execvp ") ;
		exit(EXIT_FAILURE) ;
	}

	//other commands
	else if( cfork() == -1 ) return -1 ; 		//fork child error

	return 0 ; //successfully proccessed the line
}


int cfork() {
	/* where to include the check for last child?? here or after think both ways work
	if testing check at the end, set condition to i < pipes, else i <= pipes.
	if (i == pipes) { 
	if (i == (pipes-1)) {
	if ( dup2(fildes[i*2 -1], STDOUT_FILENO) == -1) { //inherited WR 
	if ( dup2(fildes[(i+1)*2 -1], STDOUT_FILENO) == -1) { //inherited WR
	*/
	pid_t cpid, pid;
	int cstat;
	int fildes[pipes*2];
	int i;

	//flush here??
	fflush(stdout);
	fflush(stderr);

	if ( (cpid=fork()) < 0) { //fork child
		perror("fork() ") ;
		exit(EXIT_FAILURE) ;
	}
	if (cpid > 0) { //parent process
		if (bflag)	// don't wait
			printf("%s [%d]\n", proc[0].argv[0], cpid) ;
		else while (wait(&cstat) != cpid) ; //wait for completion
	}
	else if (cpid == 0) { //first child
		if (pipes == 0) {
			//Redirect stdin to /dev/null if background job
			if (bflag) {
				if (-1 == (fdin = open("/dev/null", O_RDONLY)) ) {
					fprintf(stderr, "/dev/null: No such file or directory\n") ;
					exit(EXIT_FAILURE) ;
				}
				if (dup2(fdin, STDIN_FILENO) == -1) {
					fprintf(stderr, "/dev/null: No such file or directory\n") ;
					exit(EXIT_FAILURE) ;
				}
				close(fdin) ; 	//close fdin, not needed
			}
			//redirect IO, must be after /dev/null
			if (redirect() == -1) return -1 ;
			//execute 
			execvp(proc[0].argv[0], proc[0].argv) ;
			fprintf(stderr, "%s: Command not found.\n", proc[0].argv[0]);
			exit(EXIT_FAILURE) ;
		}
		else { //pipes present
			//redirect IO
			if (redirect() == -1) return -1 ;

			for (i = 0; i < pipes; i++) {
				//last child, only WR
				if ( pipe(fildes + i*2) == -1) { //create pipe
					perror("pipe() ");
					exit(EXIT_FAILURE);
				}
				if ( (pid=fork()) < 0) { //fork fail
					perror("fork() ") ;
					exit(EXIT_FAILURE) ;
				}
				if (pid > 0) { //parent
					//all RD
					if ( dup2(fildes[i*2], STDIN_FILENO) == -1) {
						perror("dup2 ") ;
						exit(EXIT_FAILURE) ;
					}
					//WR, skip first child
					if (i != 0 && dup2(fildes[i*2 -1], STDOUT_FILENO) == -1) { //inherited WR
							perror("dup2 ") ;
							exit(EXIT_FAILURE) ;
					}
					close(fildes[i*2]); //this RD
					close(fildes[i*2 +1]); //this WR

					execvp(proc[pipes -i].argv[0],proc[pipes -i].argv);
					perror("execvp ");
					exit(EXIT_FAILURE);
				}
				if (i == (pipes-1)) {
					if ( dup2(fildes[(i+1)*2 -1], STDOUT_FILENO) == -1) { //inherited WR
						perror("dup2 ") ;
						exit(EXIT_FAILURE) ;
					}
					execvp(proc[0].argv[0],proc[0].argv); //executes command
					perror("execvp ");
					exit(EXIT_FAILURE);
				}
			}// end for loop
		}//end else
	}//end first child
}


int redirect() {
	struct stat sb;

	/*** Redirect stdout ***/
	if (outfile != NULL) {
		//overwrite outfile
		if ( owflag == 1 ) {
			if ( -1 == (fdout = open(outfile, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) ) {
			fprintf(stderr, "%s: Error opening output file.\n", outfile) ;
			return -1 ;
			}
		}
		//try opening output file
		else {
			if (-1 != (fdout = open(outfile, O_RDONLY)) ) {
				fprintf(stderr, "%s: File exists, cannot overwrite.\n", outfile) ;
				return -1 ;
			}
			if (-1 == (fdout = open(outfile, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) ) {
				fprintf(stderr, "%s: Error opening output file.\n", outfile) ;
				return -1 ;
			}
		}
		//stdout redirected to fdout
		if (dup2(fdout, STDOUT_FILENO) == -1) {
			perror("dup2") ;
			exit(EXIT_FAILURE) ;
		}
		close(fdout) ;		//close fdout, not needed
	}

	/*** Redirect stdin ***/
	if (infile != NULL) {
		//try opening input file
		if (-1 == (fdin = open(infile, O_RDONLY)) ) {
			fprintf(stderr, "redirect(): %s: No such file or directory.\n", infile) ;
			return -1 ;

		}
		//stdin redirected from fdin
		if (dup2(fdin, STDIN_FILENO) == -1) {
			perror("dup2") ;
			exit(EXIT_FAILURE) ;
		}
		close(fdin) ;		//close fdin, not needed
	}
	return 0; //success
}


int dirwalk (const char *path) {
	/* char *strncpy(char *dest, const char *source, size_t num)
	*/
	struct dirent *dp ;
	struct stat sb1, sb2 ;
	DIR *dirp ;
	char *filename ;
	char saved_cwd[MAX_ARGS] = "" ;
	char temp_cwd[MAX_ARGS] = "" ;
	char filepath[MAX_ARGS] = "" ;
	int i, charAt, length ;

	//copy path
	strcpy(filepath, path);

	//check if path is a regular file
	if ( (dirp = opendir(filepath)) == NULL && errno == ENOTDIR) {
		printf("%s\n", path) ;
		return 0 ;
	}

	//save the cwd
	getcwd(saved_cwd, sizeof(saved_cwd)) ;

	/* change working directory */

	//handle "..", get upper level directory filepath
	if ( strcmp(filepath, "..") == 0) { 
		length = strlen(saved_cwd) ;
		for (i = 0; i < length; i++)
			if (saved_cwd[i] == '/') //last occurance
				charAt = i ;
		strncpy(filepath, saved_cwd, charAt) ; 
	}
	//others
	chdir(filepath) ;

	//open directory stream
	if ( ( dirp = opendir(filepath) ) == NULL) {
		fprintf(stderr, "dirwalk(%s): No such file or directory. \n", filepath) ;
		return -1 ;
	}

	//print directory contents
	while ( ( dp = readdir(dirp) ) != NULL ) { //while there's another dir entry
		filename = dp->d_name;

		if (strcmp(filename, "..") == 0) ;
		else if (strcmp(filename, ".") == 0) ;
		else {
			printf("%s", filename)	;

			//get file information
			if ( lstat(filename, &sb1) == -1 ) {
				perror("stat") ;
				exit(EXIT_FAILURE) ;
			}

			//file types
			if ( S_ISDIR(sb1.st_mode) ) printf ("/") ;
			else if ( S_ISLNK(sb1.st_mode) ) {
				if ( stat(filename, &sb2) == 0 ) printf("@") ;
				else if ( stat(filename, &sb2) == -1 ) printf("&") ;
			}
			else if ( sb1.st_mode & (S_IXUSR  | S_IXGRP | S_IXOTH) ) printf("*") ;
			printf("\n") ;
		}
	}

	//close directory
	closedir(dirp) ;

	//change working directory to saved_cwd
	chdir(saved_cwd) ;

	return 0 ; //success
}


int set_buffer () {
	char word[STORAGE] ;
	int index = 0 ;
	int gw ;	//returned value of getword

	//read in words from stdin into buffer until EOF
	while ( ( gw=getword(word) ) != -1) {
		if (gw == 0) break ; 		//break on line terminators, ';' or '\n'
		buffer[index] = malloc(sizeof(char *) * STORAGE) ;
		strcpy(buffer[index++], word) ;
		if ( strcmp(word, AMP) == 0 ) break ; //break on '&'
	}
	buffer[index] = NULL ;
	buflen = index ;

	//Check terminating conditions
	if (gw == -1 && buflen == 0) return -1 ; 	//at EOF and empty line
	if (quote_flag) {
		fprintf(stderr, "Missing closing quote.\n") ;
		return 0 ;
	}

	return buflen ; //successfully read line from stdin
}

void print() {
	int i, j ;
	int len ;

	printf("infile=%s\n", infile);
	printf("outfile=%s\n", outfile);
	printf("pipes=%d\n", pipes);

	for (i = 0; i < buflen; i++)
		printf("buffer[%d]= %s\n", i, buffer[i]) ;
	printf("buflen= %d\n\n", buflen) ;

	for (i = 0; i < length; i++) {
		len = proc[i].argc ;
		printf("argc= %d\n", len);
		for (j = 0; j < len; j++)
			printf("proc[%d].argv[%d]= %s\n", i, j, proc[i].argv[j]) ;
	}
	printf("length= %d\n\n", length) ;

}


void reset () {
	iflag = 0 ;
	oflag = 0 ;
	bflag = 0 ;
	owflag = 0 ;
	buflen = 0 ;
	infile = outfile = errfile = NULL ;

	int i, j;
	for (i = 0; i < MAX_ARGS; i++)
		buffer[i] = NULL ;

	for (i = 0; i < length; i++) {
		proc[i].argc = 0;
		for (j = 0; j < MAX_ARGS; j++)
			proc[i].argv[j] = NULL;
	}
	length = 0 ;	//length of proc, reset after each line
}


void main() {
	int c ;
	pid_t pid = getpid() ;

	//Change PGID of p2 and all its children, PGID set to pid of p2
	setpgid(pid, pid) ;

	//Catch SIGTERM signal
	signal(SIGTERM, SIG_IGN) ;

	for (;;) {
		reset() ; //reset globals
		printf("p2: ") ;

		c = set_buffer() ;
		if (c == -1) break ; 	//reached EOF
		if (c == 0) continue ; 	//empty line or errors

		if (parse() == -1) continue ; //parse, on error continue reading
		if (process() == -1) continue ; //process, on error continue reading
	}

	killpg(getpgrp(), SIGTERM) ;
	printf("p2 terminated.\n") ;
	exit(EXIT_SUCCESS) ;
}