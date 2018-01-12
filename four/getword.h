/* getword.h - header file for the getword() function used in
 *  
 * Program Two
 * Written by Stephanie Hernandez
 * For SDSU CS570 Fall 2017 
 * Instructor John Carroll
 *
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define STORAGE 255
      /* This is one more than the max wordsize that getword() can handle */
   
int getmetas(int c, char *w);
int ismeta(int c);

int getword(char *w);

/**
int getword(char *w);
* (Note: the preceding line is an ANSI C prototype statement for getword().
  It will work fine with edoras' gcc or cc compiler.)
* The getword() function gets one "word" from the input stream.
* A "word" is a [maximal] string consisting of anything except TAB, newline,
  and EOF (EOF is actually a signal, not a character)
  (Note: unlike any sane definition, spaces (blanks) do NOT separate words; so
  'house cat' is considered ONE [size-9] word, not two [it would be two words
  if there was a tab between 'house' and 'cat', but a space doesn't cut it.)
* INPUT: a pointer to the beginning of a character array
  (This address is supplied to you in p0.c, as a parameter to getword() .)
* OUTPUT: -1, 0, or the number of characters in the word
  (If we have collected some characters, return the size of the word once you
  encounter a TAB, newline or EOF.  But if EOF is encountered while the
  word size is still zero, then return -1 [and leave an empty string in the
  character array].)  Similarly, if a newline is encountered while the
  word size is still zero, then return 0 [and leave an empty string in the
  character array].)
* SIDE EFFECTS: bytes beginning at address w will be overwritten.
  (Normally, we should guard against writing beyond the end of an array,
  but you do NOT have to worry about that safeguard in p0.)

getword() SKIPS leading TABs, so if getword() is called and there are no more
words on the line, then w points to an empty string. All strings, including the
empty string, will be delimited by a zero-byte (eight 0-bits), as per the normal
C convention (this delimiter is not 'counted' when determining the length of
the string that getword will report as a return value; see the examples below).

The metacharacters are "<", ">", "'", "|", "&", ">!" . The last word on a line may 
be terminated by the newline character OR by EOF. Word collection is "greedy": getword()
always tries to each time to read the largest word that does not violate the rules.
For example, >!foo is parsed as ">!" and then "foo", NOT as ">" and "!foo".

The backslash character "\" is special, and may change the behavir of the character 
that directly foloows it on the input line. When "\" precedes a metacharacter, 
that metacharacter is treated like most other characters.
(That is, the symbol will be lpart of ta word rather than a word delimiter.)

Another way to allow spaces (and almost anything else) inside a word is to use pairs 
of single quotes ("'"). The quotes are not part of the word -- if we really wanted a "'"
in a word, you'd have to specify it with \' . A string starting with "'" will end with 
the next "'" OR when newline OR when EOF is encountered.
Note:
'Null\'void'
returns 9 and the string "Null'void"
In all other cases, a "\" inside a quoted string is an actual "\". For example,
'7\>3'
returns 4 and the string "7\>3" whereas
'7\'3'
returns 3 and the string "7'3" .

*/
