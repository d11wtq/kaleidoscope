/**
 * Kaleidoscope LLVM tutorial.
 */

#include <cstdlib>
#include <string>
#include "parser.cpp"

void HandleTopLevelExpression() {
  if (ParseTopLevelExpr()) {
    printf("Parsed a top-level expr\n");
  } else {
    getNextToken();
  }
}

void HandleFunction() {
  if (ParseFunction()) {
    printf("Parsed a function definition\n");
  } else {
    getNextToken();
  }
}

void HandleExtern() {
  if (ParseExtern()) {
    printf("Parsed an extern expr\n");
  } else {
    getNextToken();
  }
}

/**
 * Simple example RunLoop for debugging in a REPL.
 */
void RunLoop() {
  InitParser();

  printf("ready> ");
  getNextToken();

  while (true) {
    printf("ready> ");
    switch (CurTok) {
    case tok_eof:
      return;

    case ';':
      getNextToken();
      break;

    case tok_def:
      HandleFunction();
      break;

    case tok_extern:
      HandleExtern();
      break;

    default:
      HandleTopLevelExpression();
      break;
    }
  }
}
