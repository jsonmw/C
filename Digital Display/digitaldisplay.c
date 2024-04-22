/* 

This program prompts takes an integer as a parameter and prints it to the console
in the form of seven segment digital display.

*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Function prototypes

void topZoneDig(char);
void midZoneDig(char);
void botZoneDig(char);
void displayTopRow(char *);
void displayMidRow(char *);
void displayBotRow(char *);
int validateInput(int, char *);

const char INVALID_INPUT[] = "Invalid argument. Please pass a valid number to use this program.\n";

// Prints the correct top segments for the given digit.

void topZoneDig(char digit) {

    if(digit == '1' || digit == '4') {
        printf("   ");
    } else {
        printf(" _ ");
    }

}

// Prints the correct middle segments for the given digit.

void midZoneDig(char digit) {

    if(digit == '0') {
        printf("| |");
    } else if(digit == '1' || digit == '7') {
        printf("  |");
    } else if(digit == '5' || digit == '6') {
        printf("|_ ");
    } else if(digit == '2' || digit == '3') {
        printf(" _|");
    } else {
        printf("|_|");
    } 

}

// Prints the correct bottom segments for the given digit.

void botZoneDig(char digit) {

    if(digit == '0' || digit == '6' || digit == '8') {
        printf("|_|");
    } else if(digit == '1' || digit == '4' || digit == '7') {
        printf("  |");
    } else if(digit == '2') {
        printf("|_ ");
    } else if(digit == '3' || digit == '5' || digit == '9') {
        printf(" _|");
    } 

}

// Handles the logic to print the complete top row of segments for the given number.

void displayTopRow(char *input) {

    int i = 0; 

    if(input[0] == '-') {
        printf(" ");       // check for negative
        i++;
    }

    for(i; i < strlen(input); i++) {
        topZoneDig(input[i]);
    }

    printf("\n");
}

// Handles the logic to print the complete middle row of segments for the given number.

void displayMidRow(char *input) {

    int i = 0; 

    if(input[0] == '-') {
        printf("_");        // check for negative
        i++;
    }

    for(i; i < strlen(input); i++) {
        midZoneDig(input[i]);
    }

    printf("\n");
}

// Handles the logic to print the complete bottom row of segments for the given number.

void displayBotRow(char *input) {

    int i = 0; 

    if(input[0] == '-') {
        printf(" ");        // check for negative
        i++;
    }

    for(i; i < strlen(input); i++) {
        botZoneDig(input[i]);
    }

    printf("\n");
}

// Given the number of arguments and the input String, this returns
// whether or not the String contains a valid number.

int validateInput(int argc, char *input) {

    if(argc !=2) {
        return 0;
    } 

    int i = 0;

    if(input[i] == '-' && strlen(input) > 1) {
        i++;            // check for negative
    }

    for(i; i < strlen(input); i++) {
        if(!isdigit(input[i])) {
            return 0;  
        }
    }
    return 1;
}

// MAIN METHOD

int main(int argc, char *argv[]) {   

    if(!validateInput(argc, argv[1])){
        printf("\n%s\n", INVALID_INPUT);
        return -1;
    }

    displayTopRow(argv[1]);
    displayMidRow(argv[1]);
    displayBotRow(argv[1]);

    printf("\n");
}