/* Ensure RW LCD pin is connected to GND and connect a 10 K potentiometer with wiper connected to V0 and other ends to A and K LCD pins.
 Connect a 100 ohm resistor (ideally 500 Ohm) between A LCD pin and VDD.
 */

#include "mbed.h"
#include "TextLCD.h"
#include <ctime>

TextLCD lcd(D0, D1, D2, D3, D4, D5, TextLCD::LCD20x4); // Connect these nucleo pins to RS, E, D4, D5, D6 and D7 pins of the LCD
AnalogIn seedPin(A0); // Define an AnalogIn object for the floating pin A0
AnalogIn entropyPin(A1); // Additional analog pin for entropy

// Addition function
int add(int x, int y) {
    return x + y;
}

// Subtraction function
int subtract(int x, int y) {
    return x - y;
}

// Multiplication function
int multiply(int x, int y) {
    return x * y;
}

// Division function
int divide(int x, int y) {
    return x / y;
}

// Seed generation ensures total randomness each time the program is run, allowing unique questions
int generateSeed() {
    int seed = 0;
    seed += seedPin.read_u16();
    seed += entropyPin.read_u16();
    seed += time(NULL); 
    return seed;
}

// Generates a random number from min-max
int random(int min, int max) {
    return (rand() % ((max + 1) - min)) + min;
}

// Function to display the answers
void displayAnswer(int x, int y, int z){
    lcd.locate(0,1); // sets the cursor to column 0 and row 1
    // If varience (z) is generated as 0, the correct answer (x) is displayed on the left-hand side
    if (z == 0) {
        lcd.printf("a)%d ", x);
        lcd.locate(8,1); // sets the cursor to column 8 and row 1
        lcd.printf("b)%d ", y);
    }
    // If varience is generated as 1, the correct answer (x) is displayed on the right-hand side
    else {
        lcd.printf("a)%d ", y);
        lcd.locate(8,1); // sets the cursor to column 8 and row 1
        lcd.printf("b)%d ", x);
    }
}

// Initialising variables
int x, y, op, varience, answer1, answer2;
const char* operators[4] = {"+", "-", "*", "//"};
int (*operations[4])(int, int) = {add, subtract, multiply, divide};

// Main Question Function
void questionDisplay() {
    lcd.cls(); // Clears the display at the start of each new question
    srand(generateSeed()); // Generates a new seed at the start of each new question
    op = random(0, 100) / 25; // Generates a random number from 0-3 in order to determine which mathematical operation will be performed
    varience = random(0, 100) / 50; // Generates a random number from 0-1 in order to determine how the wrong answer will be calculated, and also the locations of where the answers are displayed
    /* Note: in the above lines, random(0, 3) and random(0, 1) should do the same job, but for whatever reason, what i have written seems to randomise the number better. 
    If anyone can find a reason and / or solution to this the let me know! But for now random(0, 100) / 25 and random(0, 100) / 50 does a good job*/


    // If the mathmatical operation is addition or subtraction, the numbers will be in the range of 10-999
    if (op < 2) {
        x = random(10, 999);
        y = random(10, 999);
    }
    // If the mathmatical operation is multiplication, one number will be in the range of 10-99 and the other 2-9
    if (op == 2) {
        x = random(10, 99);
        y = random(2, 9); // The reasoning for setting this number as 2-9 and not 0-9 is because multiplying anything by 1 or 0 has an obvious correct answer
    }
    // If the mathmatical operation is division, one number will be in the range of 10-999 and the other 2-9
    if (op == 3) {
        x = random(10, 999);
        y = random(2, 9); // The reasoning for setting this number as 2-9 and not 0-9 is because dividing anything by 1 has an obvious correct answer, and dividing anything by 0 is undefined
    }
    
    lcd.locate(0, 0); // sets the cursor to column 0 and row 0
    lcd.printf("%d %s %d = ?", x, operators[op], y); // Writes out the question by displaying: the value of x, the mathematical operator in the operators array corresponding to the value of op, the value of y, and then '= ?'
    answer1 = operations[op](x, y); // Calculates the real answer by passing x and y to the mathematical operation function in the operations array corresponding to the value of op
    answer2 = varience ? answer1 + 10 : answer1 - 10; // If varience is 1, answer 2 is 10 greater than answer 1. If varience is 0, then it is 10 less
    
    displayAnswer(answer1, answer2, varience); // Display the answers

    // Countdown timer, counts down by 1 from 15 every second until it reaches 0
    for (int i = 15; i >= 0; --i) {
        lcd.locate(14, 0); // sets the cursor to column 14 and row 0
        // If the number is double digits, print as normal
        if (i > 9){
            lcd.printf("%d", i);
        }
        // If the number is a single digit, print as 0(number), this is done to ensure both columns 14 and 15 are always being used by the timer.
        else {
            lcd.printf("0%d", i);
        }
        thread_sleep_for(1000); // Wait for 1 second before displaying the next number
    }
}

// Main Program
int main()
{
    // Display 5 questions
    for (int i = 5; i > 0; --i) {
        questionDisplay();
    }
}
