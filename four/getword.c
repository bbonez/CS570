/**
 * Program Two and Four
 * Written by Stephanie Hernandez
 * For SDSU CS570 Fall 2017 
 * Instructor John Carroll
 */

#include "getword.h"
#include "p2.h"

#define QUOTE 39
#define NULL_T '\0'
#define TAB '\t'
#define BLANK ' '
#define BACKSLASH '\\'
#define NEWL '\n'
#define SEMIC ';'

/**
 * Function gets one word from input stream. A word is maximal string
 * consisting of anything except BLANKS, newline, metacharacters, and EOF.
 *
 * NOTE: a way to allow spaces and almost anything else inside a word, 
 * is to use pairs of single quotes. A string starting with the 
 * metacharacter "'" will terminate if an unexpected newline (or EOF) 
 * is encountered. 
 *
 * Metacharacters are "<", ">", ">!", "|", "'", and "&" .
 *
 * @parameter A pointer to the beginning of a character array.
 * @return -1, 0 or number of characters in the word.
 * -1 is returned if encountered EOF and word size is zero
 * 0 is returned if encountered a newline and word size is zero.

 * Needed to check if char is meta before fetching, otherwise it overwrites 
 * what 'word' holds. Another method for doing this??
 *
 * */
int getword(char *word) {
	int state = 0; //0=outside a word, 1=inside a word
	int c;
	int wlen = 0;
	int qcount = 0;
	quote_flag = 0;

	while ( wlen < STORAGE -1 && ((c = getchar() ) != EOF )) { //order matters here
		if (c == QUOTE)	{ //track single quotes
			qcount++;
			if (qcount == 2) qcount = 0; //signal end of quoted string
		}
		else if ( (c == TAB || c == BLANK) && qcount == 0) //ignore spaces
			state = 0; 
	
		else if (c == NEWL || c == SEMIC) { //terminating conditions
			if (qcount > 0 && qcount != 2) { //if no terminating quote
				quote_flag = 1; //set flag for p2.c
				return 0;
			}
			if (wlen == 0) { //no previous word found
				word[wlen] = NULL_T;;
				return 0;
			} 
			state = 0; 
			ungetc(c, stdin); //previous word, unget '\n', ';', '&' -> not part of word
		}
		else if (ismeta(c) && qcount == 0) { //metachars, only process when no previous quote
			state = 0;
			if (wlen != 0) ungetc(c, stdin); //previous word, unget meta, not part of word
			else wlen = getmetas(c, word); //no prev word, get metas 
		}
		else { //inside word
			state = 1;

			if (c == BACKSLASH && qcount > 0) { //if backslash and previous quote
				word[wlen] = c; //store backslash	
				c = getchar(); //get next character

				if (c == NEWL) ungetc(c, stdin); //if char is a newline unget c, not part of word
				else if (c == QUOTE) word[wlen++] = c; //if char is a quote, include quote only overwrite backslash
				else {
					word[++wlen] = c; //add char to word
					wlen++;
				}
			}
			else if (c == BACKSLASH && qcount == 0) { //no previous quote 
				c = getchar(); //get next characters
				if (c == NEWL) ungetc(c, stdin); //unget \n, not part of word
				else word[wlen++] = c; //add char to word
			}
			else word[wlen++] = c; //add char to word
		}

		//word read, return its length
		if (state == 0 && wlen != 0) { 
			word[wlen] = '\0';	
			if (strcmp(word, "exit") == 0) 
				return -1;
			return wlen;
		}
	}
	word[wlen] = NULL_T;

	//at EOF
	if (wlen == 0) return -1; //no previous word		
	else return wlen;  //previous word
}

/**
 * @parameter An integer representing a character
 * @return An integer value.
 *
 * Determines whether int c is a valid metacharacter. 
 * Returns 1 if true, 0 if false.
 */
int ismeta(int c) {
	if ( c == '<' || c == '>' || c == '|' || c == '&')
		return 1;
	return 0;
}

/**
 * @parameter An integer representing a character
 * @parameter A pointer to a character array
 * @return The length of the metacharacter
 *
 * Repeated calls to getchar() are made in order to determine 
 * the length of the metacharacer. Stores metacharacters in character array
 */
int getmetas(int c, char *w) {
	int mlen = 0;

	if ( c == '<' || c == '>' || c == '|' || c == '&') 
		w[mlen++] = c; //store 
		if ( c == '>' ) { //special meta
			c = getchar(); //get 2nd char
			if ( c == '!') w[mlen++] = c; //store 2nd	
			else ungetc(c, stdin); //unget 2nd char, not part of word
		}

	return mlen;
}
