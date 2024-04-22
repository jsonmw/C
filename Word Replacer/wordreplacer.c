/*

Replaces instances of specified word with specified new word in a given text.

*/

#include <stdio.h>
#include <string.h>

#define MAX_LINE    1024

    // function prototypes

int find(char *substr, char *str);
void replace(char *str, int pos, char *substr);  

// function find()
// searches for a substring in a string
// parameters:
//      substr: the substring to be found
//      str: the string to be searched for the substring
// return:
//      position in str where the substring starts, or
//      -1, if the substring was not found in str

int find(char *substr, char *str) {

    // I got rid of the pos and len variables since you can just return i or -1 
    // depending on if the substr is found, and use stlen() itself in the loop. -Jason

   for (int i = 0; i < strlen(str); i++) { 
        if (str[i] == substr[0]) {

            int flag = 1; // flips to 0 if a non-matching char is found
            int j = 0;    // offset to iterate through str from the pos while iterating through substr

            while(j < strlen(substr) && flag == 1) {
                if ((str[i+j] != substr[j])) {
                    flag = 0;
                } else {
                    j++;
                }
            }

            if(flag == 1) {
                // all char in the substr were found and i is the starting pos.
                return i; 
            }
        }
    }
    // if this is reached, the substr was not found
    return -1;
}

// function replace
// replace part of a string by another string
// parameters:
//      str: the string to be modified
//      pos: the index in str where the modified string is to start
//      newstr: the substring to be used to modify str

void replace(char *str, int pos, char *substr) {

    // i is the offset to iterate through the substring from the given starting position, and the index
    // of the replacement substring.

    for(int i = 0; i < strlen(substr); i++) {
        str[pos + i] = substr[i];  
    }

}
 
int main (int argc, char *argv[]) {
    
    // check for 3 command-line arguments
    if (argc < 3) {
        puts("Usage: ./<executable name> word1 word2\n");
        return 1;
    }
    
    // check that word1 and word2 are of equal length
    if (strlen(argv[1]) != strlen(argv[2])) {
        puts("The two words must have the same length\n");
        return 1;
    }
    
    // for each line of input, perform string replacement
    char line[MAX_LINE];
    while (fgets(line, MAX_LINE, stdin)) {
        
        // remove the \n from the end of the line
        line[strlen(line)-1] = 0;
        
        // find word1 in line
        int pos = find(argv[1], line);
        
        // while word1 found in line, replace it
        while (pos >= 0) {
            replace(line, pos, argv[2]);
            pos = find(argv[1], line);
        } 
        puts(line);
    }
}