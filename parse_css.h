/*
 * parse_css.h
 *
 *  Created on: 2011-1-2
 *      Author: leiming
 */

#ifndef PARSE_CSS_H_
#define PARSE_CSS_H_

#include <ds/sim_hash_tb.h>

typedef sim_hash_tb css_tb;

extern css_tb *id_tb;	/*ID 样式表*/
extern css_tb *label_tb;	/*标签样式表*/
extern css_tb *class_tb;	/*类样式表*/

/*
 * 输入一个css文件。该函数用来解析之。并且生成三个css样式表。实际是hash表。其键均是26个字母
 * 一个ID表
 * 一个LABEL表
 * 一个CLASS表
 */
extern int parse_css(char *file);

/*
 * 根据传入的ID、CLASS、LABEL名字从相应的CSS样式表中查找对应的样式
 * 成功则返回样式起始地址失败则返回NULL
 */
extern char *get_css_id(char *id);
extern char *get_css_class(char *class);
extern char *get_css_label(char *label);

/*
 * 根据传入的样式值取得样式名与值同时填充用于渲染的标签盒子
 * @param0 填充的标签盒子
 * @param1 传入的样式值
 */
extern int fill_rect(element *rect , char *style);

/*
 * 删除用来解析CSS文件生成的CSS样式表。在结束时调用
 */
extern int destruct_css_tb(css_tb *this);

#define S_VALUE_LEN 150
#define S_NAME_LEN 16

/*
 * 层叠样式表名和值
 * 多个样式名可能指向同一个样式值
 */
typedef struct _style_value{
	unsigned char refer;	/*该样式值被引用的次数*/
//	char value[S_VALUE_LEN];
	char *value;
}style_value;


typedef struct _style_name{
	char name[S_NAME_LEN];
	style_value *s_value;
}style_name;



#endif /* PARSE_CSS_H_ */
