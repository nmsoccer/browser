/*
 * get_token.h
 *
 *  Created on: 2010-12-8
 *      Author: leiming
 */

#ifndef GET_TOKEN_H_
#define GET_TOKEN_H_

/*
 * @param0:output buff
 * @param1:type of token
 * @param2:input buff
 * @param3:lenth of output buff
 *
 * return next start point of input
 */
char *get_token(char *value , int *type , char *input , int len_value);

#endif /* GET_TOKEN_H_ */
