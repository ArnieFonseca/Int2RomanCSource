
#include "int2roman.h"


//gcc int2roman.c -lm

/// @brief Entry Point
/// @return Return Code zeros for good and non-zeros for errors
int main(){    

 
    int numbers[] = {1958,};

    int const numbersLen = sizeof(numbers)/sizeof(int); // Calculate array count

    for (int i = ZERO; i < numbersLen; i++){
       
        int const number = numbers[i];                  // Number to process

        char* romanNumber = integerToRoman(number);   // Call main driver

        printf("The number %d is %s in roman\n", number, romanNumber);

        free(romanNumber);                              // Release memory
    }

    printf("\n");
    
    return ZERO;                                        // Go back to OS
}

