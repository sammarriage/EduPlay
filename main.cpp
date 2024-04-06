/* Ensure RW LCD pin is connected to GND and connect a 10 K potentiometer with wiper connected to V0 and other ends to A and K LCD pins.
 Connect a 100 ohm resistor (ideally 500 Ohm) between A LCD pin and VDD.
 */

#include "mbed.h"
#include "TextLCD.h"
#include <ctime>

TextLCD lcd(D0, D1, D2, D3, D4, D5, TextLCD::LCD20x4); // Connect these nucleo pins to RS, E, D4, D5, D6 and D7 pins of the LCD
AnalogIn seedPin(A0); // Define an AnalogIn object for the floating pin A0
AnalogIn entropyPin(A1); // Additional analog pin for entropy

//Initialising the buttons
DigitalIn answerA(D14, PullDown);
DigitalIn answerB(D15, PullDown);

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
int score = 0;
int elapsedSeconds = 15;
bool menu = true;
const char* operators[4] = {"+", "-", "*", "//"};
int (*operations[4])(int, int) = {add, subtract, multiply, divide};

// Main menu function
void mainMenu(){
    if (menu){ // Displays the menu if the menu variable is true
        lcd.cls(); // Clears the display
        lcd.locate(1,0); // Sets the cursor to column 1 and row 0
        lcd.printf("EduPlay Maths!"); // Displays the text 'EduPlay Maths!'
        lcd.locate(0,1); // Sets the cursor to column 0 and row 1
        lcd.printf("Press any button"); // Displays the text 'Press any button'
        score = 0; // Resets the score 
        while (menu){
            // Continuously checks to see if a button has been pressed to exit the main menu and break the menu loop
            if (answerA || answerB){
                menu = false;
            }
        }
    }
}

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
    
    lcd.locate(0, 0); // Sets the cursor to column 0 and row 0
    lcd.printf("%d %s %d = ?", x, operators[op], y); // Writes out the question by displaying: the value of x, the mathematical operator in the operators array corresponding to the value of op, the value of y, and then '= ?'
    answer1 = operations[op](x, y); // Calculates the real answer by passing x and y to the mathematical operation function in the operations array corresponding to the value of op
    answer2 = varience ? answer1 + 10 : answer1 - 10; // If varience is 1, answer 2 is 10 greater than answer 1. If varience is 0, then it is 10 less
    
    displayAnswer(answer1, answer2, varience); // Display the answers

    Timer timer; // Initialise a timer
    timer.start(); // Start a timer, this will be used as a countdown rather than a timer
    while (elapsedSeconds > 0) { // Starts a countdown loop which ends once it reaches 0
        elapsedSeconds = 15 - (timer.read_ms() / 1000); // As the timer counts up from 0, it gets subtracted from 15, thus creating a countdown from 15
        lcd.locate(14, 0); //Sets the cursor to column 14 and row 0
        lcd.printf("%02d", elapsedSeconds); // Displays the countdown, ensuring it is always 2 digits long (it will be padded with zeros if it is less than 2 digits)
        if (answerA || answerB || elapsedSeconds == 0) { // Checks if the button corresponding to answer A or B is pressed, or if the countdown is at 0
            if (answerA || answerB) { // Checks if the button corresponding to answer A or B is presse
                lcd.cls(); // Clears the display
                bool correct = (answerA && !varience) || (answerB && varience); // Defines that the correct answers are if either answer A is selected when varience is 0, or if answer B is selected when varience is 1
                score += correct ? 1500 - (timer.read_ms() / 10) : 0; // If the user got the answer correct, the milliseconds it took for the user to answer the question divided by 10 and subtracted from 1500 is added to their score. If they got the answer wrong, their score remains the same
                lcd.locate(correct ? 4 : 3, 0); // Determines the location of the cursor in order to centre the text in the next line
                lcd.printf(correct ? "Correct!" : "Incorrect!"); // If the user got the answer correct, the text 'Correct' is displayed, else 'Incorrect!' is displayed
            }
            if (elapsedSeconds == 0) { // Checks if the countdown is at 0
                lcd.cls(); // Clears the display
                lcd.locate(4, 0); // Sets the cursor to column 4 and row 0
                lcd.printf("Time Up!"); // Displays the text 'Time up!'
            }
            lcd.locate(2, 1); // Sets the cursor to column 2 and row 1
            lcd.printf("Score = %04d", score); // Displays the users score, ensuring it is always 4 digits long (it will be padded with zeros if it is less than 4 digits)
            thread_sleep_for(3000); // Waits for 3 seconds before moving onto the next question
            return; // Moves to the next question
        }
    }
}

// Main Program
int main()
{
    while (true){ // Continuosly loops, ensures that the game will always end up displaying the menu
        mainMenu(); // Displays the menu
        // Display 5 questions
        for (int i = 5; i > 0; --i) {
            questionDisplay();
        }
        lcd.cls(); // Clears the display
        lcd.locate(3,0); // Sets the cursor to column 3 and row 0
        lcd.printf("Quiz Over!"); // Displays the text 'Quiz Over!'
        lcd.locate(2,1); // Sets the cursor to column 2 and row 1
        lcd.printf("Score = %04d", score); // Displays the users score, ensuring it is always 4 digits long (it will be padded with zeros if it is less than 4 digits)
        thread_sleep_for(5000); // Waits for 5 seconds before displaying the menu
        menu = true; // Triggers the menu to display
    }
}