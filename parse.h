/*
 * parse.h
 *
 *  Created on: 2010-12-8
 *      Author: leiming
 */

#ifndef PARSE_H_
#define PARSE_H_

#include <ds/binary_tree.h>

typedef binary_tree dom_tree;

extern unsigned int use_css;	/*是否使用CSS*/

extern dom_tree *parse(char *file_name);	/*解析一个html文件并由此生成一棵孩子兄弟类型的二叉树dom_tree*/
extern int scan_dom(dom_tree *this);	/*根据第一步生成的dom_tree并且根据属性的设置加入CSS样式为dom_tree之元素盒子成员赋值*/
extern int destruct(dom_tree *this);

#endif /* PARSE_H_ */
