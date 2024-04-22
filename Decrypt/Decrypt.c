#include <stdio.h>
#include <string.h>

// Given an encrypted message and the encryption key, decrypt the message

#define WIDTH 28
#define LENGTH 4
#define PWORDLENGTH 8

// Formats the integer into binary output, used for debugging.

void printBinary(int n, int length) {
    for(int i = length - 1; i >= 0; i--){
        printf("%i", (n & 1 << i) >> i);
        if ((i % 7) == 0){
            printf(" ");
        }
    }
    printf("\n");
}

// Accepts a string containing the password, and breaks it into two 28-bit values

void get_keys(char *twoKeys, unsigned *key1, unsigned *key2) {

    char keys1[LENGTH+1], keys2[LENGTH+1];

    unsigned pass1 = 0;
    unsigned pass2 = 0;

    for(int i = 0; i < LENGTH; i++) {
        keys1[i] = twoKeys[i];             // break the pword into two 4 letter keys
        keys2[i] = twoKeys[i+LENGTH]; 
    }

    for(int i = 0; i < LENGTH; i++) {
        keys1[i] = (keys1[i] & 0x7F);
        pass1 |= keys1[i];                 // drops the MSB of each octet to reduce key1 to 28-bits
        if(i < LENGTH -1) {
            pass1 = (pass1 << 7); 
        }
    }

    for(int i = 0; i < LENGTH; i++) {
        keys2[i] = (keys2[i] & 0x7F);
        pass2 |= keys2[i];                 // drops the MSB of each octet to reduce key2 to 28-bits
        if(i < LENGTH -1) {
            pass2 = (pass2 << 7); 
        }
    }
   
   *key1 = pass1;
   *key2 = pass2;
}

// Returns the bit sequence in a 32-bit unsigned integer, given the provided
// width and starting index. Used to isolate a given field of bits in an int.

unsigned int get_n_bits (unsigned bits, int width, int index) {
    unsigned int temp = (0x7F << index * width) & bits;

    return (temp >> (width * index));
}


// Given a stream of bits, this rotates the bits three places to the left.

unsigned int rotate_left3(unsigned bits) {

    unsigned int leftBits, rightBits;  // holds the bits to make sure none are lost in rotation
    unsigned int rotated = 0;          // the rotated septets are added in place to this
    unsigned int current;              // the current septet being worked on

    for(int i = 0; i<4; i++) {
        current = get_n_bits(bits, 7, i);     // 1. returns the correct current
        leftBits = ((current & 0xF) <<3);     // 2. captures the left side of the shift
        rightBits = ((current & 0x70) >> 4);  // 3. captures the right side of the shift
        rightBits |= leftBits;                // 4. logical ORs the left bits into the right bits
        rotated |= rightBits << ((i*7));      // 5. adds the shifted septet to the correct place
    }                                      

    return rotated;
}

// Shuffles the seven four bit nibbles of a 28-bit integer.

void shuffle_nibbles(unsigned *bits) {
    //shuffle the 7 nibbles according to pattern

    unsigned int temp = 0; // This  where the bits will be |ed into after each nibble is positioned
    unsigned int tempnib;  // This is the reusable nibble space to & mask and shift the nibble into 
                           // its correct place

    unsigned int masks[] = {0xF, 0xF0, 0xF00, 0xF000, 0xF0000, 0xF00000, 0xF000000}; // mask the correct nib
    unsigned int powershift[] = {24, 16, 8, 12, 8, 16, 12}; // 4*nth position shift of each nib

    for(int i = 0; i < 7; i++) {
        tempnib = *bits & masks[i]; 
        if(i < 3) {
            tempnib = tempnib << powershift[i]; 
        } else {
            tempnib = tempnib >> powershift[i];
        }
        temp |= tempnib;  // Adds the shifted nibble to the new integer being built

    }
    *bits = temp;         // sets the bits to the shuffled version.
}


void decode_28bits(unsigned int cipher, char *plain, unsigned int key1, unsigned int key2) {
    //decode a block of 28 bits

    cipher ^= key2;                   // XOR with Key 2
    shuffle_nibbles(&cipher);        
    cipher ^= key1;                   // XOR with key 1
    cipher = rotate_left3(cipher);    

    for(int i = 0; i < 4; i++) {
        printf("%c", (char) (get_n_bits(cipher, 7, (3-i))));  // prints each character to stdout
    }
}

int main(int argc, char *argv[]) {
    
    if(strlen(argv[1]) != PWORDLENGTH || argc != 2) {
         printf("Please try again using an 8-bit key\n");
         return -1;
    }

    char line[8];
    char *plain;
    unsigned int bits, key1 = 0, key2 = 0;

    get_keys(argv[1], &key1, &key2);

    //  Gets the next bits from the stdin and passes them in hexidecimal to the decoding function

    while(fgets(line, PWORDLENGTH, stdin)) {
        sscanf(line, "%x", &bits);
        decode_28bits(bits, plain, key1, key2);
    }
    printf("\n");

    return 0;
}