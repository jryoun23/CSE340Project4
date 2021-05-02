/*
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
 #include <string>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "parser.h"
#include "compiler.h"

using namespace std;


void Parser::syntax_error()
{
    cout << "SYNTAX ERROR\n";
    exit(1);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevand
Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
    {
    	t.Print();
        syntax_error();
	}
    	
//    cout << "Consuming ";
//	t.Print();
    return t;
}
//variable functions for getting the location and printing the variable lists

void Parser::printVars()		//just prints the list of variables we have
{
	if(varCounter == 0)
	{
		cout << "Theres nothinh in the vars";
	}
	else
	{
		cout << "[ ";
		for(int i = 0; i< varCounter; i++)
		{
			cout << vars[i] << " ";
			if(i == 49)
			{
				break;
			}
		}
		cout << "] " << endl;
	}
}


int Parser::location(string lookup)
{
	int locationVal = -1;
	if(varCounter == 0)
	{
		cout << "Theres nothing in here";
	}
	else
	{
		for(int i = 0; i< varCounter; i++)
		{
			if(vars[i] == lookup)
			{
				locationVal = i;
			}
		}
	}
	return locationVal;
}

//creating and viewing the input vector
void Parser::printInputList()
{
	if(inputs.size() == 0)
	{
		cout << "The inputList is is empty! \n";
	}
	else
	{
		cout << "< ";
		for(int i = 0; i < inputs.size(); i++)
		{
			cout << inputs[i] <<  " ";
		}
		cout << " >\n";
	}
}


bool Parser::isExistingCosnt(string cosntLex)
{
	for(int i = 0; i < varCounter; i++)
	{
		if(cosntLex == vars[i])
		{
			return true;
		}
	}
	return false;
}

void Parser::printMem()
{
	cout << "[ ";
	for(int i = 0; i< varCounter; i++)
	{
		cout << " " << mem[i] << " ";
	}
	cout << "]" << endl;
}


// Parsing

//This is where we are going to do the parsing

//program -> var_section body inputs

InstructionNode * Parser::parse_program()
{
//	cout << "made it to parse_program" << endl;
	InstructionNode* final = new InstructionNode;
	InstructionNode* current = new InstructionNode;
	parse_var_section();
	final = parse_body();
	parse_inputs();
	current = final;
//	while(current != NULL)
//	{
//		cout << current->type << " ---- ";
//		current = current->next;
//	}

	return final;
}

//var_section -> id_list SEMICOLON
void Parser::parse_var_section()
{
//	cout << "Made it to parse_var_section " << endl;	
	parse_id_list();
	expect(SEMICOLON);
}


//id_list -> ID COMMA id _ist | ID

void Parser::parse_id_list()
{
//	cout << "Made it to parse_id_list " << endl;
	Token newVar = expect(ID);
	vars[varCounter] = newVar.lexeme;
	mem[varCounter] = 0;
	varCounter++;
//	printMem();
	Token t = lexer.peek(1);
	if(t.token_type == COMMA)
	{
		expect(COMMA);
		parse_id_list();	
	}
	else if(t.token_type != SEMICOLON)
	{
		syntax_error();
	}
	else 
	{
		//do nothing 
	}
}

// parse_body() -> LBRACE stmt_list RBRACE
InstructionNode* Parser::parse_body()
{
//	cout << "Made it to parse_body " << endl;
	InstructionNode* fullInstructions;
	
	expect(LBRACE);
	fullInstructions = parse_stmt_list();
	expect(RBRACE);
//	cout << "Made it to out of body " << endl;
	return fullInstructions;
}


//parse_stmt_list() -> stmt stmt_list | stmt
InstructionNode* Parser::parse_stmt_list()
{
//	cout << "Made it to parse_stmt_list " << endl;
	InstructionNode* thisInst = new InstructionNode;
	InstructionNode* remainingInst = new InstructionNode;
	
	thisInst = parse_stmt();

	Token t = lexer.peek(1);
	if(t.token_type == ID || t.token_type == WHILE || t.token_type == IF || t.token_type == SWITCH || t.token_type == FOR || t.token_type == OUTPUT || t.token_type == INPUT)
	{
		InstructionNode* temp = new InstructionNode;
		temp = thisInst;
		remainingInst = parse_stmt_list();
		while(temp->next != NULL)
		{
			temp = temp->next;
		}
		temp->next = remainingInst;
	}
	else if(t.token_type != RBRACE)
	{
		syntax_error();
	}
	else
	{
		
		//do nothing
	}
//	cout << thisInst->type << "\t" << thisInst->next->type << endl;
	return thisInst;
	
}

//stmt -> assign_stmt | while_stmt | if_stmt | switch_stmt | for_stmt | output_stmt | input_stmt

InstructionNode* Parser::parse_stmt()
{
//	cout << "Made it to parse_stmt " << endl;
	InstructionNode* returnInst = new InstructionNode;
	Token t = lexer.peek(1);
	if(t.token_type == ID)
	{
		returnInst = parse_assign_stmt();
//		cout << returnInst->type << " " << returnInst->assign_inst.left_hand_side_index << " = " <<  returnInst->assign_inst.operand1_index << endl;
	}
	else if(t.token_type == WHILE)
	{
		returnInst = parse_while_stmt();
	}
	else if(t.token_type == IF)
	{
		returnInst = parse_if_stmt();
//		cout << returnInst->type << endl;
//		cout << returnInst->next->type << endl;
//		cout << returnInst->next->next->type << endl;
//		cout << returnInst->cjmp_inst.target->type << endl;
	}
	else if(t.token_type == SWITCH)
	{
		parse_switch_stmt();
	}
	else if(t.token_type == FOR)
	{
		parse_for_stmt();
	}
	else if(t.token_type == OUTPUT)
	{
		returnInst = parse_output_stmt();
	}
	else if(t.token_type == INPUT)
	{
		returnInst = parse_input_stmt();
//		cout << returnInst->type << endl;
	}
	else
	{
		syntax_error();
	}
	return returnInst;
	
}

//assign_stmt -> ID EQUAL primary SEMICOLON | ID EQUAL expr SEMICOLON

InstructionNode* Parser::parse_assign_stmt()
{
//	cout << "Made it to parse_assign_stmt " << endl;
	InstructionNode* newInst = new InstructionNode;
	newInst->next = NULL;
	newInst->type = ASSIGN;
	Token leftSide, rightSide1, rightSide2, op;
	leftSide = expect(ID);
	newInst->assign_inst.left_hand_side_index = location(leftSide.lexeme);
	expect(EQUAL);
	Token t = lexer.peek(1);
	if(t.token_type == ID || t.token_type == NUM)
	{
		Token x = lexer.peek(2);
//		x.Print();
		if(x.token_type == PLUS || x.token_type == MINUS || x.token_type == MULT || x.token_type == DIV) //we have an arithmetic expression
		{
			rightSide1 = lexer.peek(1);
			op = lexer.peek(2);
			rightSide2 = lexer.peek(3);
			parse_expr();
			
			newInst->assign_inst.operand1_index = location(rightSide1.lexeme);
			newInst->assign_inst.operand2_index = location(rightSide2.lexeme);
			if(x.token_type == PLUS){
				newInst->assign_inst.op = OPERATOR_PLUS;
			} else if(x.token_type == MINUS){
				newInst->assign_inst.op = OPERATOR_MINUS;
			}else if(x.token_type == MULT){
				newInst->assign_inst.op = OPERATOR_MULT;
			}else if(x.token_type == DIV){
				newInst->assign_inst.op = OPERATOR_DIV;
			}
			
			
		}
		else if(x.token_type == SEMICOLON)
		{
			newInst->assign_inst.op = OPERATOR_NONE;
			rightSide1 = lexer.peek(1);
			parse_primary();
			newInst->assign_inst.operand1_index = location(rightSide1.lexeme);
//			cout << "we are making it in semi";
			//do nothing
		}
		else 
		{
//			cout << "error in not semi";
			syntax_error();
		}
	}
	else 
	{
//		cout << "error in not num or id";
		syntax_error();
	}
	expect(SEMICOLON);
	return newInst;
}

//expr -> primary op primary
void Parser::parse_expr()
{
//	cout << "Made it to parse_expr " << endl;
	parse_primary();
	parse_op();
	parse_primary();
}

//primary -> ID |NUM

void Parser::parse_primary()
{
//	cout << "Made it to parse_primary " << endl;
	Token t = lexer.peek(1);
//	t.Print();
//	cout << "Made it to parse_primary " << endl;
	if(t.token_type == ID)
	{
//		cout << "made it into the ID!" << endl;
		expect(ID);
	}
	else if(t.token_type == NUM)	//here we need to put constants in our memory
	{
		Token newConst = expect(NUM);
		if(isExistingCosnt(newConst.lexeme))
		{
//			cout << "we have an exisiting const!" << endl;
		}
		else 
		{
			vars[varCounter] = newConst.lexeme;
			mem[varCounter] = stoi(newConst.lexeme);
			varCounter++;
//			printMem();
		}
		
//		printVars();
		
	}
	else
	{
		syntax_error();
	}
//	cout << "made it to the end!? " << endl;
}

//op -> PLUS | MINUS | MULT | DIV
void Parser::parse_op()
{
//	cout << "Made it to parse_op " << endl;
	Token t = lexer.peek(1);
	if(t.token_type == PLUS)
	{
		expect(PLUS);
	}
	else if(t.token_type == MINUS)
	{
		expect(MINUS);
	}
	else if(t.token_type == DIV)
	{
		expect(DIV);
	}
	else if(t.token_type == MULT)
	{
		expect(MULT);
	}
	else
	{
		syntax_error();
	}
}

//output_stmt -> input ID SEMICOLON
InstructionNode* Parser::parse_output_stmt()
{
//	cout << "Made it to parse_output_stmt " << endl;
	InstructionNode* outputInst = new InstructionNode;
	outputInst->type = OUT;
	
	expect(OUTPUT);
	Token outVal = expect(ID);
	outputInst->output_inst.var_index = (location(outVal.lexeme));
	outputInst->next = NULL;
	expect(SEMICOLON);
	return outputInst;
}

//input_stmt -> input ID SEMICOLON
InstructionNode* Parser::parse_input_stmt()
{
//	cout << "Made it to parse_input_stmt " << endl;
	InstructionNode* newInst = new InstructionNode;
	expect(INPUT);
	Token inputTo = expect(ID);
//	cout << location(inputTo.lexeme) << endl;
	newInst->type = IN;
	newInst->input_inst.var_index = location(inputTo.lexeme);
	expect(SEMICOLON);
	newInst->next = NULL;
//	cout << "Made it to the end of parse_input_stmt " << endl;
	return newInst;

}

//while_stmt -> WHILE condition body
InstructionNode* Parser::parse_while_stmt()
{
//	cout << "Made it to parse_while_stmt " << endl;
	InstructionNode* newInst = new InstructionNode;
	newInst->type = CJMP;
	expect(WHILE);
	Token p1 = lexer.peek(1);
	Token r1 = lexer.peek(2);
	Token p2 = lexer.peek(3);
	parse_condition();
	newInst->cjmp_inst.operand1_index = location(p1.lexeme);
	newInst->cjmp_inst.operand2_index = location(p2.lexeme);
	if(r1.token_type ==  NOTEQUAL){
		newInst->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
	}else if(r1.token_type == GREATER){
		newInst->cjmp_inst.condition_op = CONDITION_GREATER;
	}else if(r1.token_type == LESS){
		newInst->cjmp_inst.condition_op = CONDITION_LESS;
	}
	newInst->next = parse_body();
	
	InstructionNode* jmpInst = new InstructionNode;
	jmpInst->type = JMP;
	jmpInst->jmp_inst.target = newInst;
	
	InstructionNode* noOperation = new InstructionNode;
	noOperation->type = NOOP;
	noOperation->next = NULL;
	jmpInst->next = noOperation;
	
	InstructionNode *curr = new InstructionNode;
	curr = newInst->next;
//	cout << curr->type << curr->next->type <<endl;
	while(curr->next != NULL)
	{
//		cout << curr->type << endl;
		curr = curr->next;
	}
	curr->next = jmpInst;
	newInst->cjmp_inst.target = noOperation;
	return newInst;
}

//if_stmt -> IF condition body
InstructionNode* Parser::parse_if_stmt() //////////////////////////////////////////////////////////////
{                             
//	cout << "Made it to parse_if_stmt " << endl;
	InstructionNode* newInst = new InstructionNode;
	newInst->type = CJMP;
	expect(IF);
	Token p1 = lexer.peek(1);
	Token r1 = lexer.peek(2);
	Token p2 = lexer.peek(3);
//	cout << p1.lexeme << " " ;
//	r1.Print() ;
//	cout << " " << p2.lexeme << endl;
	parse_condition();
//	cout << "made it past the parsing of the condition ,,,";
	newInst->cjmp_inst.operand1_index = location(p1.lexeme);
	newInst->cjmp_inst.operand2_index = location(p2.lexeme);
	if(r1.token_type ==  NOTEQUAL){
		newInst->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
	}else if(r1.token_type == GREATER){
		newInst->cjmp_inst.condition_op = CONDITION_GREATER;
	}else if(r1.token_type == LESS){
		newInst->cjmp_inst.condition_op = CONDITION_LESS;
	}
	
//	cout << newInst->cjmp_inst.operand1_index << " " << newInst->cjmp_inst.operand2_index;
	newInst->next = parse_body();
//	cout << newInst->next->type << endl;
	
	InstructionNode* noOperation = new InstructionNode;
	noOperation->type = NOOP;
	noOperation->next = NULL;
	
	InstructionNode *curr = new InstructionNode;
	curr = newInst->next;

	
	while(curr->next != NULL)
	{
//		cout << curr->type << endl;
		curr = curr->next;
	}
	curr->next = noOperation;
	newInst->cjmp_inst.target = noOperation;
	return newInst;
}

//condition -> primary relop primary

void Parser::parse_condition()
{
//	cout << "Made it to parse_condition " << endl;
	parse_primary();
	parse_relop();
//	cout << "going in!";
	parse_primary();
//	cout << "made it to the end i guess?!";
}

//relop -> GREATER | LESS | NOTEQUAL
void Parser::parse_relop()
{
//	cout << "Made it to parse_relop " << endl;
	Token t = lexer.peek(1);
//	t.Print();
	if (t.token_type == GREATER)
	{
		expect(GREATER);
	}
	else if (t.token_type == LESS)
	{
		expect(LESS);
	}
	else if(t.token_type == NOTEQUAL)
	{
		expect(NOTEQUAL);
	}
	else
	{
		syntax_error();
	}
}

//switch_stmt -> SWITCH ID LBRACE case_list RBRACE | SWITCH ID LBRACE case_list default_case RBRACE

void Parser::parse_switch_stmt()
{
//	cout << "Made it to parse_switch_stmt " << endl;
	InstructionNode * newInst = new InstructionNode;

	expect(SWITCH);
	expect(ID);
	expect(LBRACE);
	parse_case_list();
	Token t = lexer.peek(1);
	if(t.token_type == DEFAULT)
	{
		parse_default_case();
	}
	else if(t.token_type != RBRACE)
	{
		syntax_error();
	}
	else
	{
		//do nothing
	}
	expect(RBRACE);
}


//for_stmt -> FOR LPAREN assign_stmt condition SEMICOLON assign_stmt RPAREN body

void Parser::parse_for_stmt()
{
//	cout << "Made it to parse_for_stmt " << endl;
	expect(FOR);
	expect(LPAREN);
	parse_assign_stmt();
	parse_condition();
	expect(SEMICOLON);
	parse_assign_stmt();
	expect(RPAREN);
	parse_body();
}

//case_list -> case case_list | case

void Parser::parse_case_list()
{
//	cout << "Made it to parse_case_list " << endl;
	parse_case();
	Token t = lexer.peek(1);
	if(t.token_type == CASE)
	{
		parse_case_list();
	}
	else if(t.token_type != DEFAULT && t.token_type != RBRACE)
	{
		syntax_error();
	}
	else
	{
		//do nothing	
	}
}

// case -> CASE NUM COLON body
void Parser::parse_case()
{
//	cout << "Made it to parse_case " << endl;
	expect(CASE);
	expect(NUM);
	expect(COLON);
	parse_body();
}


//default_case -> DEFAULT COLON body
void Parser::parse_default_case()
{
//	cout << "Made it to parse_default_case " << endl;
	expect(DEFAULT);
	expect(COLON);
	parse_body();
}

//inputs -> num_list
void Parser::parse_inputs()
{
//	cout << "Made it to parse_inputs " << endl;
	parse_num_list();
}

//num_list -> NUM | NUM num_list()
void Parser::parse_num_list()
{
//	cout << "Made it to parse_num_list " << endl;
	Token newInput = expect(NUM);
	inputs.push_back(stoi(newInput.lexeme));
//	printInputList();
	Token t = lexer.peek(1);
	if(t.token_type == NUM)
	{
		parse_num_list();
	}
	else if(t.token_type != END_OF_FILE)
	{
		syntax_error();
	}
	else
	{
		//do nothing
	}
}



