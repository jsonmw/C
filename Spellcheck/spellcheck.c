/* 

This program prompts the user for input, checks it against a given wordlist to confirm its
spelling, and offer suggestions if its incorrectly spelled.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// FUNCTION PROTOTYPES

int loadFile(char *file);
void checkWord(int len);
int containsWord(char *word, int, int);  
int getHam(char *str1, char *str2);
void clearSuggestions();
void updateLittleHams(int ham, int hamdex);
void printSuggestions(char *word);
void offerSuggestions(char *word);
void cleanInput(char *word);

// MACROS

#define MAX_WORD_LEN 40
#define MAX_WORDS 110000
#define SUGGESTIONS 5
#define ALPHABET 26

// GLOBALS

char wordList[MAX_WORDS][MAX_WORD_LEN];
char currentFile[MAX_WORD_LEN];
int firstLetter[ALPHABET][2];     //  [0] = starting index for each letter [1] = number of words per letter
                                  //  used for checking if word is in list before hamming.
int suggestions[SUGGESTIONS][2];  //  [0] = index of suggestion [1] = hamming distance
                                  //   used for storing suggestions


// Given a valid file, loads it into the wordlist and returns the number of words in the list.

int loadFile(char *file) {
    char word[MAX_WORD_LEN];
    int idx = 0;
    
    FILE* wordFile = fopen(file, "r");

    if(wordFile == NULL) {
        printf("\nInvalid file :: now exiting Spell Check. \n\n");
        return 0;
    }

    printf("\nPreparing word list from %s...\n", file);
    int i = 0, num = 0; //iterates through word list and counts how many words of each letter there are.

    // Takes the input from the given file and stores it in the world list.

    while(fgets(word, MAX_WORD_LEN, wordFile)) {    
        
        char startingChar = tolower(word[0]);
        if(startingChar != ('a' + idx)) {

            idx = (int) startingChar - 'a';
            firstLetter[idx][0] = i;
            num = 0;
            
        }
        int len = 0;

        cleanInput(word);
        strcpy(wordList[i], word);
    
        i++;
        num++;
        firstLetter[idx][1] = num;  // stores how many letters start with that character
    }

    fclose(wordFile);
    printf("\n>> %i words loaded.\n\n", i);
    return i;
}

// Takes the length of the word list and handles the logical flow of the spell check operation.

void checkWord(int len) {
    char word[MAX_WORD_LEN];     
    int ham, smallestHam, listIterator, checkedStart, checkedEnd;
    size_t wordLen;                

    while(1) {
        clearSuggestions();
        listIterator = 0;

        printf("\n----------------------------------------------------------------------------");
        printf("\nPlease enter the spelling you wish checked (Enter nothing to return to menu) ");
        printf("\n----------------------------------------------------------------------------\n\n: ");
        fgets(word, MAX_WORD_LEN, stdin);

    
        wordLen = strlen(word);
        if(word[0] == '\n') {
            printf("\n\n");
            break;     // exit if enter pressed with no word.
        } else {
            word[wordLen-1] = '\0'; // remove "\n" character from fgets() input
        }
    
        wordLen -= 1;
        smallestHam = wordLen;
        checkedStart = firstLetter[tolower(word[0]) - 'a'][0];
        checkedEnd = checkedStart + firstLetter[word[0] - 'a'][1]; // to skip words already checked by containsWord()

        if(!containsWord(word, checkedStart, checkedEnd)) {      
            printf("\nExcellent job! %s is spelled correctly!\n", word);
        } else {

            while(listIterator < len && smallestHam) {   //iterate through word list. 

                if(wordLen == strlen(wordList[listIterator]) 
                              // does not recheck words checked by containsWord()
                && (listIterator < checkedStart || listIterator >= checkedEnd)) {  
                    ham = getHam(word, wordList[listIterator]);             // sets the hamming distance
                    smallestHam = (smallestHam < ham) ? smallestHam : ham;  // assignes new smallestHam if applicable
                    updateLittleHams(ham, listIterator);                    // checks the current word against the suggestions array
                } 
                listIterator++;
            }
            offerSuggestions(word);
        } 
    }
}

// Updates the suggestions array based on the given hamming distance and index.

void updateLittleHams (int ham, int hamdex) {
    
    for(int i=0; i <5; i++) {
        // checks if word is a duplicate suggestion
        if(!strcmp(wordList[suggestions[i][0]], wordList[hamdex])) {
            return;
        }
    }
    
    for(int i=0; i < 5; i++) {
        // Checks to fill any empty suggestions
        if(suggestions[i][0] == 0 && suggestions[i][1] == MAX_WORD_LEN) {
            suggestions[i][0] = hamdex;                                    
            suggestions[i][1] = ham;
            i = 5; // exit loop.
            return;
        }
    }

    for(int i=0; i < 5; i++) {
        // Replaces suggestion with current ham if lower
        if(suggestions[i][1] > ham) {
            suggestions[i][0] = hamdex;
            suggestions[i][1] = ham;
            i = 5; // exit loop.
        }
    }
}

// Checks if word exists using the first character index array to speed up search. Returns 0
// if found.

int containsWord(char *word, int current, int end) {
    int found = 1;

    while(found && current < end) {
        if(strlen(word) == strlen(wordList[current])) {
            found = getHam(word, wordList[current]); 
            updateLittleHams(found, current);
        }
        current++;   
    }
    return found;
}

// Calculates hamming distance of the input word and the current test word. Returns as an integer.

int getHam(char *word, char *test) {
    int ham = 0, i = 0;
    while(tolower(word[i]) != '\0' ) {
        if(tolower(word[i]) != tolower(test[i])) {
            ham++;
        }
        i++;
    }
    return ham;
}

// Sorts the suggestions alphabetically by first letter and calls the printSuggestions() function

void offerSuggestions(char *word) {
    int tempInd;
    int lowest, lowIndex, jIndex;

    // sorts array

    for(int i = 0; i < 4; i++ ) {
        lowest = i;

        for(int j = i + 1; j < 5; j++) {
            lowIndex = suggestions[lowest][0];
            jIndex = suggestions[j][0];

            if(tolower(wordList[jIndex][0]) < tolower(wordList[lowIndex][0])) {
                lowest = j;  
            }
        }
        tempInd = suggestions[lowest][0];
        suggestions[lowest][0] = suggestions[i][0];
        suggestions[i][0] = tempInd;
    }
    printSuggestions(word);
}

// Removes newline characters from text file/fgets input.

void cleanInput(char *input) {
    int i = 0;
    while(input[i] != '\0') {
        if(input[i] == '\n' || input[i] == '\r') {  
            input[i] = '\0';
            break;
        }
        i++;
    }
}

// Sets all indexes to 0 and hamming distances to MAX_WORD_LEN to reset suggestions
// in a new spell check.

void clearSuggestions(){
    for(int i = 0; i < 5; i++) {
        suggestions[i][0] = 0;
        suggestions[i][1] = MAX_WORD_LEN;
    }
}

// Prints the current list of suggestions to the console in alphabetical order.

void printSuggestions(char *word) {
    printf("\nCould not find %s in the current dictionary, did you mean: \n\n", word);
    for(int i = 0; i < 5; i++) {
        printf("\t%s", wordList[suggestions[i][0]]);
    }
    printf("\n");
}

// MAIN

int main(int argc, char *argv[]) {
    int len, exec = 1;
    printf("----------------------\n");
    printf("WELCOME TO SPELL CHECK\n");
    printf("----------------------\n");

    char currentFile[MAX_WORD_LEN];

    if(argc == 1 || argc > 2) {
        printf("Invalid argument. Please run program using a text file-based word list.\n");
        return -1;
    } else {
        strcpy(currentFile, argv[1]);
    }
    printf("Loading %s...", currentFile);
    len = loadFile(currentFile);

    char select[1];

    //  MENU
    while(exec && len) {
        printf("----------------------\n");
        printf("What do you wish to do?\n");
        printf("----------------------\n\n");
        printf(" Press 1 to check a word.\n");
        printf(" Press 2 to load a new word list.\n");
        printf(" Press 3 to view the current word list.\n");
        printf(" Press Q to quit.\n\n");
        printf(": ");
        fgets(select, 5, stdin);

        switch(select[0]) {
            case '1' :
                checkWord(len);
                break;
            case '2' :
                printf("\nEnter the relative path of file name for the word list you wish to use: ");
                
                fgets(currentFile, MAX_WORD_LEN, stdin);
                cleanInput(currentFile);
                printf("\n----------------------\n");
                len = loadFile(currentFile); // exits loop if invalid file.
                break;
            case '3' : 
                printf("\nPrinting word list from %s... q\n\n", currentFile);
                for(int i = 0; i < len; i++) {
                    printf("%s\n", wordList[i]);
                }
                printf("\n%i words\n\n", len);
                break;
            case 'q' :
                printf("\n----------------------\n");
                printf("Thank you for supporting spelling.\n");
                exec = 0;
                break;
            default:
                printf("\n----------------------\n");
                printf("PLEASE ENTER A VALID OPTION\n\n");
                break;
        }
    }
    printf("\n");
}