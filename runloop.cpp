/**
 * Kaleidoscope LLVM tutorial.
 */

#include <cstdlib>
#include <string>
#include "parser.cpp"
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/TargetSelect.h>

void HandleTopLevelExpression() {
  if (FunctionNode *Fn = ParseTopLevelExpr()) {
    if (Function *F = Fn->Codegen()) {
      void *FPtr = TheExecutionEngine->getPointerToFunction(F);
      double (*FP)() = (double (*)())(intptr_t)FPtr;
      printf("-> %f\n", FP());
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
  InitializeNativeTarget();

  std::string Err;
  TheModule          = new Module("Kaleidoscope", getGlobalContext());
  TheExecutionEngine = EngineBuilder(TheModule).setErrorStr(&Err).create();
  if (!TheExecutionEngine) {
    printf("Failed to initialize JIT: %s\n", Err.c_str());
    exit(1);
  }

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

  TheModule->dump();
}
