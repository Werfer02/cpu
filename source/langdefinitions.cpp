#include "langdefinitions.hpp"

std::unordered_map<std::string, operation>  stringToOperation{
	{"PROGEND",       operation::PROG_END},
	{"TEST",              operation::TEST},
	{"NOOP",              operation::NOOP},
	{"ECHO",              operation::ECHO},
	{"JUMP",              operation::JUMP},
	{"WRITEHEAP",    operation::WRITEHEAP},
	{"READHEAP",      operation::READHEAP},
	{"JUMPZERO",      operation::JUMPZERO},
	{"WRITEREG",      operation::WRITEREG},
	{"ADD",                operation::ADD},
	{"SUB",                operation::SUB},
	{"WRITESCREEN",operation::WRITESCREEN},
	{"JUMPREG",        operation::JUMPREG},
	{"JUMPZEROREG",operation::JUMPZEROREG},
	{"LABEL",            operation::LABEL},
	{"ROM",                operation::ROM},
	{"GET",                operation::GET},
};
