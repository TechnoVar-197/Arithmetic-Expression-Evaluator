# Arithmetic Expression Evaluator

This C program evaluates arithmetic expressions provided by the user. It supports basic arithmetic operations such as addition, subtraction, multiplication, and division, as well as the use of parentheses to dictate operation precedence.

## Features

- **Token Parsing:** Converts user input into tokens, which represent numbers, operators, and parentheses.
- **Expression Parsing:** Parses and evaluates expressions using a recursive descent parsing technique, which supports:
  - Basic arithmetic (`+`, `-`, `*`, `/`)
  - Proper handling of operation precedence
  - Use of parentheses to override default precedence
- **Interactive Input:** Users can input expressions directly when prompted by the program.
- **Real-time Debug Output:** The program outputs debug information to the console, showing the internal process of tokenization and expression parsing.

## How It Works

1. **Tokenization:** The program first reads the input string and breaks it down into a series of tokens. Each token is either a number, an operator (`+`, `-`, `*`, `/`), or a parenthesis (`(`, `)`).
   
2. **Parsing:** Using the tokens generated from the input, the program parses the expression:
   - **Factors:** Numbers or any expressions inside parentheses.
   - **Terms:** Handles multiplication and division, as these have higher precedence than addition and subtraction.
   - **Expressions:** Handles addition and subtraction, building on the results from terms to compute the final result.

3. **Evaluation:** The parsed expression is evaluated recursively, where multiplication and division are processed before addition and subtraction due to their higher precedence rules.

## Usage

To use this program, compile it with a C compiler such as `gcc` and run the executable. You will be prompted to enter an arithmetic expression. After entering the expression and pressing Enter, the program evaluates the expression and prints the result.

```bash
gcc -o expr_evaluator expression_evaluator.c
./expr_evaluator
```

Enter your expression at the prompt, for example:

```
Enter an expression and press enter: 1 + 2 * (3 + 4)
```

The program will output the steps it takes to evaluate the expression and will display the result:

```
Result: 15.000000
```

## Technologies

- **C Programming Language:** The entire program is written in standard C, making use of dynamic memory allocation, loops, conditionals, and standard I/O.
- **Standard Libraries:** Uses standard libraries like `stdio.h`, `stdlib.h`, and `ctype.h` for input/output operations, memory management, and character handling respectively.

## Developer Notes

- Ensure that the input does not exceed the buffer size (1024 characters in this implementation).
- The program currently handles basic arithmetic operations and can be extended to include more complex mathematical functions and error handling improvements.
