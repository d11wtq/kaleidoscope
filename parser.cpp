/**
 * LALR(1) parsing functions for Kaleidoscope.
 */

#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include "lexer.cpp"
#include "ast.cpp"

static ExprNode *ParseExpression();

/**
 * Operator precedences.
 */
static std::map<char, int> BinOpPrecedence;

/**
 * Current lookahead token.
 */
static int CurTok;

/**
 * Read the next token from the input stream.
 */
static int getNextToken() {
  return CurTok = gettok();
};

/**
 * Get the operator precedence to apply to the current token.
 */
static int GetTokenPrecedence() {
  if (!isascii(CurTok))
    return -1;

  return std::max(-1, BinOpPrecedence[CurTok]);
}

/**
 * Initialize static variables.
 */
static void InitParser() {
  BinOpPrecedence['<'] = 10;
  BinOpPrecedence['+'] = 20;
  BinOpPrecedence['-'] = 20;
  BinOpPrecedence['*'] = 40;
  BinOpPrecedence['/'] = 40;
}

/**
 * Generic error handling function (returns a NULL node).
 */
static ExprNode *ExprError(const char *Str) {
  fprintf(stderr, "Error: %s\n", Str);
  return NULL;
}

/**
 * Error handling function call when failing to parse a function prototype.
 */
static PrototypeNode *PrototypeError(const char *Str) {
  ExprError(Str);
  return NULL;
}

/**
 * Error handling function call when failing to parse a function definition.
 */
static FunctionNode *FunctionError(const char *Str) {
  ExprError(Str);
  return NULL;
}

/**
 * Return an ExprNode for a numeric value read by the lexer.
 */
static ExprNode *ParseNumberExpr() {
  ExprNode *Node = new NumberExprNode(NumVal);
  getNextToken();
  return Node;
}

/**
 * Return an ExprNode for a '(' Expr ')' form.
 */
static ExprNode *ParseParenExpr() {
  getNextToken();
  ExprNode *Node = ParseExpression(); // Where does this come from?

  if (!Node)
    return NULL;

  if (CurTok != ')')
    return ExprError("Expected ')'");

  getNextToken();

  return Node;
}

/**
 * Parse an ExprNode for an Identifier, or a Function Call.
 */
static ExprNode *ParseIdentifierExpr() {
  std::string IdentifierName = IdentifierStr;

  getNextToken();

  if (CurTok != '(') {
    return new IdentifierExprNode(IdentifierName);
  } else {
    getNextToken();

    std::vector<ExprNode *> Args;

    if (CurTok != ')') {
      while (true) {
        ExprNode *Arg = ParseExpression();

        if (!Arg)
          return NULL;

        Args.push_back(Arg);

        if (CurTok == ')')
          break;

        if (CurTok != ',')
          return ExprError("Expected ',' or ')' in argument list");

        getNextToken();
      }
    }

    getNextToken();

    return new CallExprNode(IdentifierName, Args);
  }
}

/**
 * Wrapper function to parse a primary token from the grammar.
 */
static ExprNode *ParsePrimary() {
  switch (CurTok) {
  case tok_identifier:
    return ParseIdentifierExpr();

  case tok_number:
    return ParseNumberExpr();

  case '(':
    return ParseParenExpr();

  default:
    return ExprError("Unknown token, expecting expr");
  }
}

static ExprNode *ParseBinOpRHS(int ExprPrec, ExprNode *LHS) {
  while (true) {
    int TokPrec = GetTokenPrecedence();

    // Don't consume anything if precedence lower than current expr
    if (TokPrec < ExprPrec)
      return LHS; // Recursion terminates here

    int BinOp = CurTok;

    getNextToken();

    ExprNode *RHS = ParsePrimary();

    if (!RHS)
      return NULL;

    int NextPrec = GetTokenPrecedence();

    // If next operator is right-associative
    if (TokPrec < NextPrec) {
      // Replace entire RHS, using itself as LHS in a new Expr
      RHS = ParseBinOpRHS(TokPrec+1, RHS);

      if (!RHS)
        return NULL;
    } else {
      // Replace the entire LHS with current Expr
      LHS = new BinaryExprNode(BinOp, LHS, RHS);
    }
  }
}

/**
 * Parse any type of expression from the input stream.
 */
static ExprNode *ParseExpression() {
  ExprNode *LHS = ParsePrimary();

  if (!LHS)
    return NULL;

  return ParseBinOpRHS(0, LHS);
}
