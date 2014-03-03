/**
 * Lexical scanning functions for Kaleidoscope.
 */

#include <cstdlib>
#include <string>

// Lexical token types
enum Token {
  tok_eof = -1,

  // commands
  tok_def    = -2,
  tok_extern = -3,

  // primary
  tok_identifier = -4,
  tok_number     = -5
};

// Set for tok_identifier
static std::string IdentifierStr;

// Set for tok_number
static double NumVal;

/**
 * Lexical analysis routine.
 */
static int gettok() {
  static int LastChar = ' ';

  // Skip whitespace
  while (isspace(LastChar))
    LastChar = getchar();

  // Scan complete identifier
  if (isalpha(LastChar)) {
    IdentifierStr = LastChar;

    while (isalnum((LastChar = getchar())))
      IdentifierStr += LastChar;

    if      (IdentifierStr == "def")    return tok_def;
    else if (IdentifierStr == "extern") return tok_extern;

    return tok_identifier;
  }

  // Scan 64 bit number
  if (isdigit(LastChar) || LastChar == '.') {
    std::string NumStr;

    do {
      NumStr += LastChar;
      LastChar = getchar();
    } while (isdigit(LastChar) || LastChar == '.');

    NumVal = strtod(NumStr.c_str(), 0);
    return tok_number;
  }

  // Skip over comments
  if (LastChar == '#') {
    do {
      LastChar = getchar();
    } while (LastChar != '\n' && LastChar != EOF);

    return gettok();
  }

  if (LastChar == EOF)
    return tok_eof;

  int AsciiChar = LastChar;
  LastChar = getchar();

  return AsciiChar;
}
