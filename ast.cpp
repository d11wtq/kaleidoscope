/**
 * AST node classes for Kaleidoscope.
 */

#include <cstdlib>
#include <string>
#include <vector>

/**
 * Base class for all AST nodes.
 */
class ExprNode {
public:
  virtual ~ExprNode() {}
};

/**
 * AST node representing a single numeric value.
 */
class NumberExprNode : public ExprNode {
  double Val;
public:
  NumberExprNode(double val) : Val(val) {}
};

/**
 * AST node for a variable or function name.
 */
class IdentifierExprNode : public ExprNode {
  std::string Name;
public:
  IdentifierExprNode(const std::string &name) : Name(name) {}
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
