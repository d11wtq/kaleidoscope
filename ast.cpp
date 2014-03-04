/**
 * AST node classes for Kaleidoscope.
 */

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <cstdlib>
#include <string>
#include <vector>

using namespace llvm;

/**
 * Called when an error is detected generation LLVM IR.
 */
Value *Error(const char *Str) {
  fprintf(stderr, "Error: %s\n", Str);
  return NULL;
}

static Module *TheModule;
static IRBuilder<> Builder(getGlobalContext());
static std::map<std::string, Value *> Symbols;

/**
 * Base class for all AST nodes.
 */
class ExprNode {
public:
  virtual ~ExprNode() {}
  /**
   * Virtual function used to generate LLVM IR.
   */
  virtual Value *Codegen() = 0;
};

/**
 * AST node representing a single numeric value.
 */
class NumberExprNode : public ExprNode {
  double Val;
public:
  NumberExprNode(double val) : Val(val) {}

  virtual Value *Codegen() {
    return ConstantFP::get(getGlobalContext(), APFloat(Val));
  }
};

/**
 * AST node for a variable or function name.
 */
class IdentifierExprNode : public ExprNode {
  std::string Name;
public:
  IdentifierExprNode(const std::string &name) : Name(name) {}

  virtual Value *Codegen() {
    Value *V = Symbols[Name];
    if (!V)
      return Error("Undefined variable");

    return V;
  }
};

/**
 * AST node for a binary expression such as "42 + 5".
 */
class BinaryExprNode : public ExprNode {
  char Op;
  ExprNode *LHS, *RHS;
public:
  BinaryExprNode(char op, ExprNode *lhs, ExprNode *rhs)
    : Op(op), LHS(lhs), RHS(rhs) {}

  virtual Value *Codegen() {
    Value *LValue = LHS->Codegen();
    Value *RValue = RHS->Codegen();

    if (!(LValue && RValue))
      return NULL;

    switch (Op) {
    case '+':
      return Builder.CreateFAdd(LValue, RValue, "addtmp");

    case '-':
      return Builder.CreateFSub(LValue, RValue, "subtmp");

    case '*':
      return Builder.CreateFMul(LValue, RValue, "multmp");

    case '/':
      return Builder.CreateFDiv(LValue, RValue, "divtmp");

    case '<':
      {
        Value *Cmp = Builder.CreateFCmpULT(LValue, RValue, "cmptmp");
        // convert int from cmp operation to double
        return Builder.CreateUIToFP(
          Cmp,
          Type::getDoubleTy(getGlobalContext()),
          "inttmp");
      }

    default:
      return Error("Unspported binary operator");
    }
  }
};

/**
 * AST node for function call.
 */
class CallExprNode : public ExprNode {
  std::string Callee;
  std::vector<ExprNode *> Args;
public:
  CallExprNode(const std::string &callee, std::vector<ExprNode *> &args)
    : Callee(callee), Args(args) {}

  virtual Value *Codegen() {
    Function *CalleeFn = TheModule->getFunction(Callee);

    if (!CalleeFn)
      return Error("Call to undefined function");

    if (CalleeFn->arg_size() != Args.size())
      return Error("Incorrect arg count");

    std::vector<Value *> CallArgs;

    for (int i = 0, e = Args.size(); i < e; ++i) {
      Value *A = Args[i]->Codegen();
      if (!A) return NULL;

      CallArgs.push_back(A);
    }

    return Builder.CreateCall(CalleeFn, CallArgs, "calltmp");
  }
};

/**
 * AST node that describes a function definition (not its body).
 */
class PrototypeNode {
  std::string Name;
  std::vector<std::string> Params;
public:
  PrototypeNode(const std::string &name,
                const std::vector<std::string> &params)
    : Name(name), Params(params) {}
};

/**
 * AST node for a complete function definition & body.
 */
class FunctionNode {
  PrototypeNode *Prototype;
  ExprNode *Body; // Why is this not a vector?
public:
  FunctionNode(PrototypeNode *prototype, ExprNode *body)
    : Prototype(prototype), Body(body) {}
};
