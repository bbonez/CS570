/**
 * Program Two and Four
 * Written by Stephanie Hernandez
 * For SDSU CS570 Fall 2017 
 * Instructor John Carroll
 *
 */
#include "getword.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <stdlib.h>
#include<string.h>

#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#define MAX_ARGS 100
#define MAX_PIPES 10
#define CD_ARGS 2
#define CWD "."
#define RIN "<"
#define ROUT ">"
#define AMP "&"
#define PIPE "|"
#define OW_FLAG ">!"


/*
 * Program exits on pipe, dup2, execvp failure, or EOF
 * Also exits when "exit" string is found in input line
 *
 * Program uses a structure to hold all the processes for piping.
 * Each array index holds information about a process.
 * The structure consists of two members, an array of pointers that holds the 
 * arguments of the process, and the number of arguments of that process, 
 * including the name of the process.
 *
 * Program uses single array of pointers to hold command arguments.
 */

/**
 * Flag set by getword
 * On double, valid quotes, value is zero
 * On single quotes, value is 1
 */
int quote_flag; 

/**
 * Prints values of all buffers
 * Just for debugging
 *
 */
void print();


/**
 * @return integer
 * On success, integer >0 is returned. On error or empty line, zero is returned.
 * On EOF, -1 is returned.
 *
 * Reads in line from stdin into buffer
 * A line is terminated by semicolon or a newline.
 * 
*/
int set_buffer();


/**
 * @return integer
 * On success zero is returned. On error, -1.
 *
 * Processes built-in commands, without forking:
 * For cd p2 complains about too many arguments, ends iteration and 
 * issues prompt again. 
 * 
 * For ls-F p2 complains about too many arguments but does not end iteration, 
 * instead p2 processes the first argument and ignores the rest.
 *
 * All other commands are forked.
 */
int process();



/**
 * @return integer
 * On success, zero is returned. On error, -1 is returned.
 *
 * Goes through buffer and does compares to determine input file, output
 * file, and check for errors such as invalid null commands, too many 
 * redirects, missing name for redirects and invalid lines.
 *
 * As is goes through the buffer, it sets values for proc[].
 */
int parse();


/**
 * @return integer
 * On success, zero is returned. On error, -1 is returned.
 *
 * Redirects stdin and/or stdout to given files.
 *
 */
int redirect();



/**
 * @return integer
 * @param Path to directory
 * On success, zero is returned. On error, -1 is returned.
 *
 * Called by ls-F. 
 * Prints directory contents
 * 
 * Copy input parameter, need local variable to overwrite in case of ".." .
 * Save working directory, change to new path, then open the directory for reading.
 * Change back to saved working directory.
 */
int dirwalk (const char *);



/**
 * @return integer
 * On error, -1 is returned.
 *
 * Vertical piping. Child forks new child, this child forks a new child and so on.
 *
 * For background jobs, it prints PID of the first child, the name of 
 * the child process, and doesn't wait for child to complete.
 *
 * Everything takes place inside the first child.
 * The for loop runs number of pipes + 1.
 * First check if this is the last child, this child only WR to the end of 
 * inherited pipe and executes the command.
 * Else, execute pipe() and fork(). In the parent segment, check if its the 
 * first child, this child only RDs and all others RD and WR. Then it executes 
 * a process. Increment i and repeat.
 */
int cfork(); 


/**
 *
 * Resets global variables
 */
void reset();

