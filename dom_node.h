/*
 * dom_node.h
 *
 *  Created on: 2010-12-13
 *      Author: leiming
 */

#ifndef DOM_NODE_H_
#define DOM_NODE_H_

#define DN_VALUE_LEN	34

#include "element.h"

typedef struct _dom_node{
	unsigned short type;	/*type:Element Text Attribute*/
	struct _dom_node *parent;		/*如果是标签那么指向父标签；如果是属性或者文本指向其父标签*/
	struct _dom_node *brother;	/*指向该结点的兄结点(即其的上一个兄弟结点)*/
	element rect;	/*对应的元素盒子*/
	char value[DN_VALUE_LEN];
}dom_node;

/*#######TYPE##################*/

#define ROOT			1
#define ELEMENT	2
#define TEXT			3
#define ATTR			4

#endif /* DOM_NODE_H_ */
