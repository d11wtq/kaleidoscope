/**
 * LALR(1) parsing functions for Kaleidoscope.
 */

#include <cstdlib>
#include <string>
#include <vector>
#include "lexer.cpp"
#include "ast.cpp"

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
