#pragma once

enum ScalarType
{
	UCHAR_T = 0,
	INT_T = 1,
	FLOAT_T = 2,
	DOUBLE_T = 3
};

#define TemplateMacroCase(typeN, type, call) \
	case typeN: { typedef type TT; call; }; break
#define TemplateMacro(call) \
	TemplateMacroCase(UCHAR_T, unsigned char, call); \
	TemplateMacroCase(INT_T, int, call); \
	TemplateMacroCase(FLOAT_T, float, call); \
	TemplateMacroCase(DOUBLE_T, double, call)