#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#include "lexer.h"
#include "parser.h"

struct InstructionNode * parse_generate_intermediate_representation()
{
   Parser parser;
   struct InstructionNode * final;
   final = parser.parse_program();
   return final;
}


