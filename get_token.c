/*
 * get_token.c
 *
 *  Created on: 2010-12-8
 *      Author: leiming
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "token_types.h"

/*
 * @param0:output buff
 * @param1:type of token
 * @param2:input buff
 * @param3:lenth of output buff
 *
 * return next start point of input
 */
char *get_token(char *value , int *type , char *input , int len_value){
	int in_iter = 0;
	int out_iter = 0;

	while(1){
		switch(input[in_iter]){
		case ' ':
		case '\t':
			in_iter++;
			break;

		case '<':
			value[0] = input[0];	/*保存符号*/
			value[1] = 0;

			*type = LABEL_START;
			in_iter++;
			return &input[in_iter];

		case '>':
			value[0] = input[0];
			value[1] = 0;

			*type = LABEL_END;
			in_iter++;
			return &input[in_iter];

		case '\\':
			value[0] = input[0];
			value[1] = 0;

			*type = SLASH;
			in_iter++;
			return &input[in_iter];

		case '/':
			value[0] = input[0];
			value[1] = 0;

			*type = BACKSLASH;
			in_iter++;
			return &input[in_iter];

		case '=':
			value[0] = input[0];
			value[1] = 0;

			*type = ASSIGN;
			in_iter++;
			return &input[in_iter];

		case '\n':
			value[0] = input[0];
			value[1] = 0;

			*type = CTRL;
			in_iter++;
			return &input[in_iter];

		case '%':
			value[0] = input[0];
			value[1] = 0;

			*type = PERCENT;
			in_iter++;
			return &input[in_iter];

		case '"':
			value[0] = input[0];
			value[1] = 0;

			*type = DQUOTE;
			in_iter++;
			return &input[in_iter];

		case '\'':
			value[0] = input[0];
			value[1] = 0;

			*type = SQUOTE;
			in_iter++;
			return &input[in_iter];

		case '.':
			value[0] = input[0];
			value[1] = 0;

			*type = POINT;
			in_iter++;
			return &input[in_iter];

		case '(':
			value[0] = input[0];
			value[1] = 0;

			*type = LBRACK;
			in_iter++;
			return &input[in_iter];

		case ')':
			value[0] = input[0];
			value[1] = 0;

			*type = RBRACK;
			in_iter++;
			return &input[in_iter];

		case ';':
			value[0] = input[0];
			value[1] = 0;

			*type = SEMI;
			in_iter++;
			return &input[in_iter];

		case '!':
			value[0] = input[0];
			value[1] = 0;

			*type = SIGN;
			in_iter++;
			return &input[in_iter];

		case '+':
			value[0] = input[0];
			value[1] = 0;

			*type = ADD;
			in_iter++;
			return &input[in_iter];

		case '-':
			value[0] = input[0];
			value[1] = 0;

			*type = MINUS;
			in_iter++;
			return &input[in_iter];

		case '*':
			value[0] = input[0];
			value[1] = 0;

			*type = MUL;
			in_iter++;
			return &input[in_iter];

		case '{':
			value[0] = input[0];
			value[1] = 0;

			*type = LBRACE;
			in_iter++;
			return &input[in_iter];

		case '}':
			value[0] = input[0];
			value[1] = 0;

			*type = RBRACE;
			in_iter++;
			return &input[in_iter];

		case ':':
			value[0] = input[0];
			value[1] = 0;

			*type = TIPS;
			in_iter++;
			return &input[in_iter];

		case '#':
			value[0] = input[0];
			value[1] = 0;

			*type = POUND;
			in_iter++;
			return &input[in_iter];

		case ',':
			value[0] = input[0];
			value[1] = 0;

			*type = COMMA;
			in_iter++;
			return &input[in_iter];


		case 'A'...'Z':
		case 'a'...'z':
			memset(value , 0 , len_value);
			while(1){	/*直到遇见不是a~z字母或者下划线为止*/
				switch(input[in_iter]){
				case '_':
				case 'A'...'Z':
				case 'a'...'z' :
				case '0'...'9':	/*包括数字是可以的。但是不能以数字开头 否则就成了下面的情况*/
					value[out_iter] = input[in_iter];
					out_iter++;
					in_iter++;
					break;
				default:
					*type = STRING;
					return &input[in_iter];
				}
			}

		case '0'...'9':
			memset(value , 0 , len_value);
			while(1){
				switch(input[in_iter]){
				case '0'...'9':
					value[out_iter] = input[in_iter];
					out_iter++;
					in_iter++;
					break;
				default:
					*type = STRING;
					return &input[in_iter];
				}
			}

		default:
			printf("Warning: meets unknow token %c. get token failed!\n" , input[0]);
			printf("input is:%s\n" , input);
			pause();
			break;

		}	/*end switch*/


	}

}
