/**
 * Kaleidoscope LLVM tutorial.
 */

#include <cstdlib>
#include <string>
#include "parser.cpp"

void HandleTopLevelExpression() {
  if (FunctionNode *Fn = ParseTopLevelExpr()) {
    if (Function *F = Fn->Codegen()) {
      printf("Parsed a top-level expr\n");
      F->dump();
    }
  } else {
    getNextToken();
  }
}

void HandleFunction() {
  if (FunctionNode *Fn = ParseFunction()) {
    if (Function *F = Fn->Codegen()) {
      printf("Parsed a function definition\n");
      F->dump();
    }
  } else {
    getNextToken();
  }
}

void HandleExtern() {
  if (PrototypeNode *Proto = ParseExtern()) {
    if (Function *F = Proto->Codegen()) {
      printf("Parsed an extern expr\n");
      F->dump();
    }
  } else {
    getNextToken();
  }
}

/**
 * Simple example RunLoop for debugging in a REPL.
 */
void RunLoop() {
  TheModule = new Module("Kaleidoscope", getGlobalContext());

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
