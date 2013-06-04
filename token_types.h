/*
 * token_types.h
 *
 *  Created on: 2010-12-8
 *      Author: leiming
 */

#ifndef TOKEN_TYPES_H_
#define TOKEN_TYPES_H_


#define LABEL_START	1	/*<*/
#define LABEL_END		2	/*>*/
#define SLASH				3	/*\*/
#define BACKSLASH		4 	/*/*/
#define ASSIGN				5	/*=*/
#define CTRL					6	/*\n */
#define PERCENT			7	/*%*/
#define DQUOTE				8	/*"*/
#define SQUOTE				9	/*'*/
#define POINT					10	/*.*/
#define LBRACK				11	/*(*/
#define RBRACK				12	/*)*/
#define SEMI					13	/*;*/
#define SIGN					14	/*!*/
#define ADD					15	/*+*/
#define MINUS					16	/*-*/
#define MUL					17	/***/
#define DIVIDE				BACKSLASH
#define LBRACE				18	/*{*/
#define RBRACE				19	/*}*/
#define TIPS					20	/*:*/
#define POUND				21	/*#*/
#define COMMA				22	/*,*/
#define STRING				30	/*string*/
#define NUMBER				31	/*number*/
#define UNKNOWN			32	/*unknown char*/

#endif /* TOKEN_TYPES_H_ */
