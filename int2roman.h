#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

// Constants
#define  ZERO           0
#define  ONE            1
#define  FOUR           4
#define  FIVE           5
#define  TEN           10
#define  MAPPER_LIMIT   8
#define  ROMAN_LIMIT   16

// Mapper structure
typedef struct{
    int     const number;
    char*   const roman;
}Mapper;

typedef enum {
    UB_EQUALS_LB,
    EXACT_NUMBER_EXISTS,
    UB_MINUS_LB_GT_PN,
    UB_MINUS_LB_LT_PN
}RomanCase;

typedef enum {
    FROM_LEFT,
    FROM_RIGHT
}Direction;

// Bound Type Enumeration
typedef enum {
    UPPER_BOUND,
    LOWER_BOUND,
    PREV_LOWER_BOUND,
    EXACT_BOUND
}BoundType;

// Function Pointer to Comparaison functions
typedef bool (*FuncPtr)(int,int);

// Functions Prototype

Mapper*   buildMapper(void);
int       getProcessingNumber(int const number);
char*     getRomanNumber(int const pn, Mapper* ub, Mapper* lb, Mapper* plb, Mapper* exct);
RomanCase getRomanCase(int const pn, Mapper* ub, Mapper* lb, Mapper* plb, Mapper* exct);
void      repeat(int const count, char* dest, char* const src);

Mapper*   getBound(int const pn, BoundType, Mapper* mapper);
bool      greatOrEqual (int fld1, int fld2);
bool      lessOrEqual (int fld1, int fld2); 
bool      lessThan (int fld1, int fld2); 
bool      equal (int, int);
void      setupBoundInfo(BoundType, Direction*, FuncPtr*);
char*     integerToRoman(int const);
char*     integerToRomanHelper(int const, Mapper*, char*);


/// @brief Main driver to convert a Number to a Roman Numeral
/// @param number Input number to be process
/// @param mapper Pointer to a mapping structure
/// @return Array pointer containing the conversion
char* integerToRoman(int const number){

    Mapper* mapper = buildMapper();                                // Create mapping structure

    char* romanNumber = (char*)malloc(ROMAN_LIMIT);                // Define destination array
    memset(romanNumber,ZERO,ROMAN_LIMIT);                          // Initialize array nulls


    char* answer = integerToRomanHelper(number, mapper, romanNumber);      // return the conversion 

    free(mapper);  
    
    return answer;                                                // Release memory
}

/// @brief Tail Recursive function to convert Number to Roman Numeral
/// @param number Input number to be process
/// @param mapper Pointer to a mapping structure
/// @param romanNumber Pointer the array that will hols the number conversion
/// @return Array pointer containing the conversion
char* integerToRomanHelper(int const number, Mapper* mapper, char* romanNumber){

    if (number == ZERO)                                            // Base case and end recursive call
        return romanNumber;

    int const pn = getProcessingNumber(number);                    // Get the Processing Number

    Mapper* ub   = getBound(pn, UPPER_BOUND, mapper);              // Get Upper Bound
    Mapper* lb   = getBound(pn, LOWER_BOUND, mapper);              // Get Lower Bound
    int plbPn    = (lb->number < FIVE) ? FIVE : lb->number;        // Adjust Prev Lower Bound    
    Mapper* plb  = getBound(plbPn, PREV_LOWER_BOUND, mapper);
    int exctPn   = ub->number - pn;
    Mapper* exct = getBound(exctPn, EXACT_BOUND, mapper);          // Get Optional Exact

    char* rst    = getRomanNumber(pn, ub, lb, plb, exct);          // Integer to Roman conversion

    strcat(romanNumber,rst);                                       // Copy result to Roman Number array
    free(rst);                                                     // Release memory

    return integerToRomanHelper(number - pn, mapper, romanNumber); // Call recursively with new arguments
}

/// @brief Repeat the source into the destination nth number of times
/// @param count Number of times to repeat
/// @param dest Pointer to the Destination
/// @param src  Pointer to the Source
void repeat(int const count, char* dest, char* const src) {
    for (size_t i = ZERO; i < count; i++)
        strcat(dest, src);
}

/// @brief Get the Roman Case enumeration
/// @param pn  Processing Number
/// @param ub  Upper Bound
/// @param lb  Lower Bound
/// @param plb Previous Lower Bound
/// @param exct Exact number when Ub minus PN exist in the mapping array
/// @return A Roman Case enumeration
RomanCase getRomanCase(int const pn, Mapper* ub, Mapper* lb, Mapper* plb, Mapper* exct){

    RomanCase romanCase;

    if (ub->number == lb->number) {                                // Case 1: UB equals LB
        romanCase = UB_EQUALS_LB;
    }
    else if (exct != NULL && (ub->number - pn == exct->number)) {  // Case 2: handles IX or XL
        romanCase = EXACT_NUMBER_EXISTS;
    }
    else if (ub->number  -  lb->number > pn) {                     // Case 3: handles 2/3 -> I/II 

        romanCase = UB_MINUS_LB_GT_PN;
    }
    else {                                                         // Case 4: handles 6/7 -> VI/VII
        romanCase = UB_MINUS_LB_LT_PN;
    }

    return romanCase;
}
/// @brief Converts a number to its corresponding roman equivalence
/// @param pn  Processing Number
/// @param ub  Upper Bound
/// @param lb  Lower Bound
/// @param plb Previous Lower Bound
/// @param exct Exact number when Ub minus PN exist in the mapping array
/// @return Converted Roman Number
char*  getRomanNumber(int const pn, Mapper* ub, Mapper* lb, Mapper* plb, Mapper* exct){


    char* rst = malloc(sizeof(char) * FOUR);                       // Allocate memory for string
    memset(rst,ZERO, FOUR);                                        // Initialize array nulls

    RomanCase romanCase = getRomanCase(pn, ub, lb, plb, exct);     // Get the Roman Case

    switch (romanCase)
    {
        case UB_EQUALS_LB:
            strcat(rst, ub->roman);                                // Move Upper Bound
            break;
        case EXACT_NUMBER_EXISTS:
            strcat(rst, exct->roman);                              // Move the exact value
            strcat(rst, ub->roman);                                // Move Upper Bound
            break;
        case UB_MINUS_LB_GT_PN:
            int const lbCount = pn / lb->number;                   // Calculate repeating number
            repeat(lbCount, rst, lb->roman);                       // Lower Bound times count
            break;
        default:
            strcat(rst, lb->roman);
            int const plbCount = (pn - lb->number) / plb->number;  // Calculate repeating number
            repeat(plbCount, rst, plb->roman);                     // Prev Lower Bound times count
            break;
    }

    return rst;                                                    // Send the roman string back
}

/// @brief Give 347 this functions returns 300; given 47 it returns 40
/// @param number Input number
/// @return Returns a procession number
int getProcessingNumber(int const number){

    int const exp  = log10((double)number);                        // Call log given 347 -> 2
    int const base = pow(TEN, (double)exp);                        // Exponent given 2 -> 100
    int const pn   = (number / base) * base;                       // After computation returns 300
    return pn;                                                     // Send processing number
}

//
// Comparison functions
//
bool greatOrEqual (int x, int y){                                  // Check when x >= y
    return (x >= y) ? true : false;
} 
bool lessOrEqual (int x, int y){                                   // Check when x <= y                        
    return (x <= y) ? true : false;
}
bool lessThan (int x, int y){                                      // Check when x < y
    return (x < y) ? true : false;
} 
bool equal (int x, int y){                                         // Check when x == y
    return (x == y) ? true : false;
}   

/// @brief Based on the Bound Type set direction and pointer function
/// @param bt Bound Type Enum
/// @param direction Pointer to Direction Enum
/// @param fn Pointer to a function Pointer
void setupBoundInfo(BoundType bt, Direction* direction, FuncPtr* fn){
    
    switch (bt){
        case UPPER_BOUND:                                          // When Upper Bound
            *direction = FROM_LEFT;
            *fn = &greatOrEqual;
            break;
        case LOWER_BOUND:                                          // When Lower Bound
            *direction = FROM_RIGHT;
            *fn = &lessOrEqual;
            break;
        case PREV_LOWER_BOUND:                                     // When Prev Lower Bound
            *direction = FROM_RIGHT;
            *fn = &lessThan;
            break;
        default:                                                   // When Exact
            *direction = FROM_LEFT;
            *fn = &equal;
            break;
    }   
}

/// @brief Retrieve the proper bound based on the Bound Type
/// @param pn Processing Number
/// @param bt Bound Type
/// @param mapper Mapping between Integer and Roman Number
/// @return 
Mapper*   getBound(int const pn, BoundType bt, Mapper* mapper) {
    
    FuncPtr fn;                                                     // Declare Function Pointer              
    Direction direction;                                            // Declare Direction Enum
    Mapper*  candidate = NULL;                                      // Declare Candidate
     
    setupBoundInfo(bt, &direction, &fn);    

    switch (direction)    {
        case FROM_LEFT:                                             // Retrieve from the Left
            for (int i = ZERO; i < MAPPER_LIMIT; i++){
                if ( fn((mapper+i)->number, pn)) {                  // Get first match
                   candidate = (mapper+i);       
                   break;                                           // Exit Loop
                }      
            } 
            break;                                                  // Exit Switch
        default:                                                    // Retrieve from the Right
             for (int i = MAPPER_LIMIT - ONE; i >= ZERO; i--) {

                if ( fn((mapper+i)->number, pn)) {                  // Get first match
                   candidate = (mapper+i);
                   break;                                           // Exit Loop
                }
             }
             break;                                                 // Exit Switch 
    }
  
    return candidate;                                               // Send candidate back

}

/// @brief Builds the Pair Number:Int, Roman:String for mapping between both systems
/// @param None
/// @return A Pointer the Mapper Structure
Mapper* buildMapper(void) {

    Mapper original[] = {                                          // Declare initialized mapper
        {.number=1, .roman="I"},
        {.number=5, .roman="V"},
        {.number=10, .roman="X"},
        {.number=50, .roman="L"},
        {.number=100, .roman="C"},
        {.number=500, .roman="D"},
        {.number=1000, .roman="M"},
        {.number=5000, .roman="V\u0305"}

    };

    int const mapperSize  = sizeof(Mapper) * MAPPER_LIMIT;          // Get memory to allocate
    Mapper* mapper = (Mapper*)malloc(mapperSize);                   // Reserve memory in the heap

    memcpy(mapper, original, mapperSize);                           // Copy data
    return mapper;                                                  // Send back mapper pointer
}
