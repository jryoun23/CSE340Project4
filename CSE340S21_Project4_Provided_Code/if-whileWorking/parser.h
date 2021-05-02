/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"
#include <vector>
#include <string>
#include "compiler.h"
using namespace std;

class Parser {
  public:
	InstructionNode * parse_program();

  private:	
    LexicalAnalyzer lexer;
    void syntax_error();
    Token expect(TokenType expected_type);
    void parse_var_section();
    void parse_id_list();
    InstructionNode* parse_body();
    InstructionNode* parse_stmt_list();
    InstructionNode* parse_stmt();
    InstructionNode* parse_assign_stmt();
    void parse_expr();
    void parse_primary();
    void parse_op();
    InstructionNode* parse_output_stmt();
    InstructionNode* parse_input_stmt();
    InstructionNode* parse_while_stmt();
    InstructionNode* parse_if_stmt();
    void parse_condition();
    void parse_relop();
    void parse_switch_stmt();
    void parse_for_stmt();
    void parse_fpr_stmt();
    void parse_case_list();
    void parse_case();
    void parse_default_case();
    void parse_inputs();
    void parse_num_list();
    
    //variable stuff
    string vars[50];
    int location(string lookup);
    void printVars();
    void printMem();
    int varCounter = 0;
    bool isExistingCosnt(string cosntLex);
    void printAssingnInst();
    
    
    //inputs stuff
	void printInputList();
};

#endif
