/*
 * parse_css.c
 *
 *  Created on: 2011-1-2
 *      Author: leiming
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ds/dyn_stack.h>
#include <ds/binary_tree.h>
#include <ds/sim_hash_tb.h>

#include "token_types.h"
#include "get_token.h"
#include "dom_node.h"
#include "parse_css.h"
#include "css_attr.h"

#include "render.h"

#define VALUE_LEN	32
#define INPUT_LEN		100

css_tb *id_tb;	/*ID 样式表*/
css_tb *label_tb;	/*标签样式表*/
css_tb *class_tb;	/*类样式表*/

static style_name *make_style_name(char *name);	/*根据一个名字产生一个样式名*/
static style_value *make_style_value(char *value);	/*根据样式产生一个样式值*/
static int s_v2s_n(style_name *s_name , style_value *s_value);	/*将一个样式值赋予一个样式名*/
/*
 * 输入一个css文件。该函数用来解析之。并且生成三个css样式表。实际是hash表。其键均是26个字母
 * 一个ID表
 * 一个LABEL表
 * 一个CLASS表
 */
int parse_css(char *file){
	unsigned long line;

	FILE *fp;
	dyn_stack *stack;	/*用于存储多个标签对应一个样式。将所有样式名字都入栈*/

	char lowchar;	/*小写字符。用于存储所有样式名第一个字符的小写形式从而方便装入样式表*/
	style_name *s_name;
	style_value *s_value;
	int in_brace;	/*当前是否在{}之中*/

	int type;
	char *p;
	char value[VALUE_LEN];
	char	input[INPUT_LEN];

	char name_buff[S_NAME_LEN];
	char value_buff[S_VALUE_LEN];

	in_brace = 0;	/*开始并不在样式中*/
	line = 0;		/*开始行数为0*/

	stack = dyn_stack_new(10);

	id_tb = sim_hash_tb_new(26);
	label_tb = sim_hash_tb_new(26);
	class_tb = sim_hash_tb_new(26);

	fp = fopen(file , "r");
	if(!fp){
		printf("open file %s failed!\n" , file);
		return -1;
	}

	memset(input , 0 , INPUT_LEN);
	memset(value , 0 , VALUE_LEN);
	memset(name_buff , 0 , S_NAME_LEN);
	memset(value_buff , 0 , S_VALUE_LEN);

	printf("ready to parse css\n");

	while(1){
		line++;	/*新的一行*/
		/*
		 * 每到此处读取文件一行。如果到文件末尾则退出
		 */
		if(fgets(input , INPUT_LEN , fp) == NULL){
			printf("fgets failed!\n");
			break;
		}
		if(strlen(input) == 0){
			printf("end of file!\n");
			break;
		}

////////////MAIN///////////////////////////////
		p = input;
		while(1){	/*解析刚才读取的一行*/
			if(strlen(p) == 0){
//				printf("   end parsing one line!\n");
				break;
			}

			p = get_token(value , &type , p , VALUE_LEN);
			switch(type){
			case CTRL:
				continue;

			case COMMA:	/*如果是逗号，表示前面一个样式名结束。准备接收另一个样式名*/
				s_name = make_style_name(name_buff);
				stack->push(stack , s_name);	/*将样式名入栈*/
				memset(name_buff , 0 , S_NAME_LEN);	/*将样式名缓冲区重置，以准备存储新的名字*/
				break;

			case LBRACE:	/*如果是左花括弧表示之后出现的都是样式值。之前的都是样式名字*/
				s_name = make_style_name(name_buff);
				stack->push(stack , s_name);
				memset(name_buff , 0 , S_NAME_LEN);
				in_brace = 1;
				break;

			case RBRACE:	/*如果是右花括弧表示一个样式值接收完成*/
				in_brace = 0;
				s_value = make_style_value(value_buff);
				memset(value_buff , 0 , S_VALUE_LEN);
				if(!s_value){
					printf("warning: no style definition before %d\n" , line);
				}


				while(1){
					if(stack->size(stack) == 0){	/*将所有入栈的样式名与当前的样式值联系起来。直到栈空*/
						break;
					}
					s_name = (style_name *)stack->pop(stack);

					switch(s_name->name[0]){	/*根据样式名的首字母决定将其放入哪个样式表中*/
					case '.':	/*以点开头装入类样式表*/
						lowchar = tolower(s_name->name[1]);
						class_tb->set_line(class_tb , (unsigned long)((lowchar - 'a')%('a')));	/*根据第二个字母与'a'的模放入表中*/
						class_tb->ins_tail(class_tb , s_name);
						s_v2s_n(s_name , s_value);
						break;

					case '#':	/*以＃开头装入ID样式表*/
						lowchar = tolower(s_name->name[1]);
						id_tb->set_line(id_tb , (unsigned long)((lowchar - 'a')%('a')));	/*根据第二个字母与'a'的模放入表中*/
						id_tb->ins_tail(id_tb , s_name);
						s_v2s_n(s_name , s_value);
						break;

					case 'a'...'z':	/*以字母开头装入LABLE样式表*/
					case 'A'...'Z':
						lowchar = tolower(s_name->name[0]);
						label_tb->set_line(label_tb , (unsigned long)((lowchar - 'a')%('a')));	/*根据第一个字母与'a'的模放入表中*/
						label_tb->ins_tail(label_tb , s_name);
						s_v2s_n(s_name , s_value);
						break;

					default:
						printf("Warning:meets illegal label %s beore line %d\n" , s_name->name , line);
						break;
					}


				}
				break;

			default:	/*其他所有的东西都当作是字符串放入name_buff 或者 value_buff之中*/

				if(!in_brace){/*如果不在样式中。那么说明是样式名将其放入样式名缓冲区中*/
					strcat(name_buff , value);
				}else{	/*在样式中就将符号放倒样式值缓冲区中*/
					strcat(value_buff , value);
				}
				break;

			}	/*end switch*/


		}	/*end parsing input*/

next_line:
		;

	}	/*end reading a line*/

	dyn_stack_destroy(stack);
	fclose(fp);
	return 0;

}	/*end file*/


/*
 * 根据传入的ID、CLASS、LABEL名字从相应的CSS样式表中查找对应的样式
 * 成功则返回样式起始地址失败则返回NULL
 */
static int find_style_id(void *data , void *arg){
	style_name *psn = (style_name *)data;
	if(strcasecmp(&(psn->name[1]) , (char *)arg) == 0){
		return 0;
	}
	return -1;
}

static int find_style_class(void *data , void *arg){
	style_name *psn = (style_name *)data;
	if(strcasecmp(&(psn->name[1]) , (char *)arg) == 0){
		return 0;
	}
	return -1;
}

static int find_style_label(void *data , void *arg){
	style_name *psn = (style_name *)data;
	if(strcasecmp(psn->name , (char *)arg) == 0){
		return 0;
	}
	return -1;
}



char *get_css_id(char *id){
	style_name *psn;

	id_tb->set_line(id_tb , (id[0] - 'a')%('a'));
	psn = (style_name *)id_tb->foreach(id_tb , find_style_id , id);

	if(psn){
		return psn->s_value->value;
	}
	return NULL;
}

char *get_css_class(char *class){
	style_name *psn;

	class_tb->set_line(class_tb , (class[0] - 'a')%('a'));
	psn = (style_name *)class_tb->foreach(class_tb , find_style_class , class);

	if(psn){
		return psn->s_value->value;
	}
	return NULL;
}

char *get_css_label(char *label){
	style_name *psn;

	label_tb->set_line(label_tb , (label[0] - 'a')%('a'));
	psn = (style_name *)label_tb->foreach(label_tb , find_style_label , label);

	if(psn){
		return psn->s_value->value;
	}
	return NULL;
}

/*
 * 根据传入的样式值取得样式名与值同时填充用于渲染的标签盒子
 * @param0 填充的标签盒子
 * @param1 传入的样式值集合以分号分隔
 */
static extra_element *get_extra(void){
	extra_element *pee;

	pee = (extra_element *)malloc(sizeof(extra_element));
	memset(pee , 0 , sizeof(extra_element));

	return pee;
}


int fill_rect(element *rect , char *style_value){
	char *start;
	char *next;
	char *css_name;
	char *css_value;
	char *temp;

	int num;
	int end;

	int type;
	char *p;
	char value[VALUE_LEN];
//	char	input[INPUT_LEN];

	start = style_value;

	while(1){	/*分解样式值*/
//		memset()


		if(strlen(start) == 0){
			break;
		}
		next = strchr(start , ';');	/*取得一个样式*/
		next[0] = 0;
		next++;	/*指向下一个样式*/

		css_name = start;	/*分解该样式为样式名与样式值得*/
		css_value = strchr(css_name , ':');
		css_value[0] = 0;
		css_value++;


		/*
		 * 匹配CSS样式名并用CSS样式值填充RECT
		 */
		switch(css_name[0]){
		case 'B':
		case 'b':
			/*分析距离父元素下端 实际上可以理解为margin-bottom*/
			if(strcasecmp(css_name , BOTTOM) == 0){
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				end = strlen(css_value);
				end--;
				if(css_value[end] == '%'){	//若采用百分比表示
					num = atoi(css_value);
					num = 0 - num;
					rect->margin[2] = num;
				}else if(strcasecmp(css_value , "auto") == 0){
					rect->margin[2] = 0;
				}else{	//采用绝对值
					num = atoi(css_value);
					rect->margin[2] = num;
				}
				goto end_matching;
			}


			/*分析背景色*/
			if(strcasecmp(css_name , BG_COLOR) == 0){
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}
				if(strcasecmp(css_value , "white") == 0){
					rect->background = WHITE;
				}else if(strcasecmp(css_value , "black") == 0){
					rect->background = BLACK;
				}else if(strcasecmp(css_value , "red") == 0){
					rect->background = RED;
				}else if(strcasecmp(css_value , "green") == 0){
					rect->background = GREEN;
				}else if(strcasecmp(css_value , "blue") == 0){
					rect->background = BLUE;
				}else if(strcasecmp(css_value , "yellow") == 0){
					rect->background = YELLOW;
				}else{
					printf("Warning: unknown color:%s\n" , css_value);
				}
				goto end_matching;
			}

			/*处理边框样式*/
			if(strcasecmp(css_name , BR_STYLE) == 0){
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}
				if(strcasecmp(css_value , "none") == 0){
					rect->border_style = BRS_NONE;
				}else if(strcasecmp(css_value , "solid") == 0){
					rect->border_style = BRS_SOLID;
				}else if(strcasecmp(css_value , "dash") == 0){
					rect->border_style = BRS_DASH;
				}else if(strcasecmp(css_value , "dotted") == 0){
					rect->border_style = BRS_DOTTED;
				}else{
					printf("Warning: unknown border style:%s\n" , css_value);
				}
				goto end_matching;
			}

			/*处理边框宽度*/
			if(strcasecmp(css_name , BR_WIDTH) == 0){
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				if(strcasecmp(css_value , "medium") == 0){
					rect->border_width = BRW_MEDIUM;
				}else if(strcasecmp(css_value , "thin") == 0){
					rect->border_width = BRW_THIN;
				}else if(strcasecmp(css_value , "thick") == 0){
					rect->border_width = BRW_THICK;
				}else{	//采用绝对值
					num = atoi(css_value);
					rect->border_width = num;
				}
				goto end_matching;
			}

			/*分析边框背景色*/
			if(strcasecmp(css_name , BR_COLOR) == 0){
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}
				if(strcasecmp(css_value , "white") == 0){
					rect->border_color = WHITE;
				}else if(strcasecmp(css_value , "black") == 0){
					rect->border_color = BLACK;
				}else if(strcasecmp(css_value , "red") == 0){
					rect->border_color = RED;
				}else if(strcasecmp(css_value , "green") == 0){
					rect->border_color = GREEN;
				}else if(strcasecmp(css_value , "blue") == 0){
					rect->border_color = BLUE;
				}else if(strcasecmp(css_value , "yellow") == 0){
					rect->border_color = YELLOW;
				}else{
					printf("Warning: unknown color:%s\n" , css_value);
				}
				goto end_matching;
			}

			if(strcasecmp(css_name , BG_IMG) == 0){
				goto end_matching;
			}
			if(strcasecmp(css_name , BG_REPEAT) == 0){
				goto end_matching;
			}
			if(strcasecmp(css_name , BG_POS) == 0){
				goto end_matching;
			}
			if(strcasecmp(css_name , BG_ATTACH) == 0){
				goto end_matching;
			}


			goto unknown_style;
			break;

		case 'C':
		case 'c':
			/*分析前景色*/
			if(strcasecmp(css_name , COLOR) == 0){
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}
				if(strcasecmp(css_value , "white") == 0){
					rect->foreground = WHITE;
				}else if(strcasecmp(css_value , "black") == 0){
					rect->foreground = BLACK;
				}else if(strcasecmp(css_value , "red") == 0){
					rect->foreground = RED;
				}else if(strcasecmp(css_value , "green") == 0){
					rect->foreground = GREEN;
				}else if(strcasecmp(css_value , "blue") == 0){
					rect->foreground = BLUE;
				}else if(strcasecmp(css_value , "yellow") == 0){
					rect->foreground = YELLOW;
				}else{
					printf("Warning: unknown color:%s\n" , css_value);
				}
				goto end_matching;
			}
			/*处理消除*/
			if(strcasecmp(css_name , CLEAR) == 0){
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				if(strcasecmp(css_value , "none") == 0){
					rect->clear = CLE_NONE;
				}else if(strcasecmp(css_value , "left") == 0){
					rect->clear = CLE_LEFT;
				}else if(strcasecmp(css_value , "right") == 0){
					rect->clear = CLE_RIGHT;
				}else if(strcasecmp(css_value , "both") == 0){
					rect->clear = CLE_BOTH;
				}else{
					printf("Warning: unknown clear value:%s\n" , css_value);
				}
				goto end_matching;
			}

			if(strcasecmp(css_name , CURSOR) == 0){
				goto end_matching;
			}
			goto unknown_style;
			break;

		case 'D':
		case 'd':
			if(strcasecmp(css_name , DISPLAY) == 0){
				goto end_matching;
			}
			goto unknown_style;
			break;

		case 'F':
		case 'f':
			/*处理浮动*/
			if(strcasecmp(css_name , FLOAT) == 0){
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				if(strcasecmp(css_value , "none") == 0){
					rect->floating = FLO_NONE;
				}else if(strcasecmp(css_value , "left") == 0){
					rect->floating = FLO_LEFT;
				}else if(strcasecmp(css_value , "right") == 0){
					rect->floating = FLO_RIGHT;
				}else{
					printf("Warning: unknown float value:%s\n" , css_value);
				}
				goto end_matching;
			}
			/*处理字体类型*/
			if(strcasecmp(css_name , FONT_FAMILY) == 0){
				goto end_matching;
			}

			if(strcasecmp(css_name , FONT_SIZE) == 0){
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				num = atoi(css_value);
				rect->font_size = num;
				goto end_matching;
			}
			if(strcasecmp(css_name , FONT_STYLE) == 0){
				goto end_matching;
			}
			if(strcasecmp(css_name , FONT_WEIGHT) == 0){
				goto end_matching;
			}
			if(strcasecmp(css_name , FONT_VARIANT) == 0){
				goto end_matching;
			}
			goto unknown_style;
			break;

		case 'H':
		case 'h':

			if(strcasecmp(css_name , HEIGHT) == 0){	/*处理元素高度*/
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				end = strlen(css_value);
				end--;
				if(css_value[end] == '%'){	//若采用百分比表示
					num = atoi(css_value);
					num = 0 - num;
					rect->height = num;
				}else if(strcasecmp(css_value , "auto") == 0){
					rect->height = HEI_AUTO;
				}else{	//采用绝对值
					num = atoi(css_value);
					rect->height = num;
				}
				goto end_matching;
			}
			goto unknown_style;
			break;

		case 'L':
		case 'l':
			/*分析距离父元素左端 实际上可以理解为margin-left*/
			if(strcasecmp(css_name , LEFT) == 0){
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				end = strlen(css_value);
				end--;
				if(css_value[end] == '%'){	//若采用百分比表示
					num = atoi(css_value);
					num = 0 - num;
					rect->margin[3] = num;
				}else if(strcasecmp(css_value , "auto") == 0){
					rect->margin[3] = 1;
				}else{	//采用绝对值
					num = atoi(css_value);
					rect->margin[3] = num;
				}
				goto end_matching;
			}

			if(strcasecmp(css_name , LIST_S) == 0){
				goto end_matching;
			}
			if(strcasecmp(css_name , LIST_S_TYPE) == 0){
				goto end_matching;
			}
			if(strcasecmp(css_name , LIST_S_IMAGE) == 0){
				goto end_matching;
			}
			if(strcasecmp(css_name , LIST_S_POS) == 0){
				goto end_matching;
			}
			if(strcasecmp(css_name , LINE_HEIGHT) == 0){
				goto end_matching;
			}
			if(strcasecmp(css_name , LETTER_SPACE) == 0){
				goto end_matching;
			}
			goto unknown_style;
			break;

		case 'M':
		case 'm':
			/*处理顶边距*/
			if(strcasecmp(css_name , MARGIN_TOP) == 0){
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				end = strlen(css_value);
				end--;
				if(css_value[end] == '%'){	//若采用百分比表示
					num = atoi(css_value);
					num = 0 - num;
					rect->margin[0] = num;
				}else if(strcasecmp(css_value , "auto") == 0){
					rect->margin[0] = MAR_AUTO;
				}else{	//采用绝对值
					num = atoi(css_value);
					rect->margin[0] = num;
				}
				goto end_matching;
			}

			/*处理右边距*/
			if(strcasecmp(css_name , MARGIN_RIGHT) == 0){
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				end = strlen(css_value);
				end--;
				if(css_value[end] == '%'){	//若采用百分比表示
					num = atoi(css_value);
					num = 0 - num;
					rect->margin[1] = num;
				}else if(strcasecmp(css_value , "auto") == 0){
					rect->margin[1] = MAR_AUTO;
				}else{	//采用绝对值
					num = atoi(css_value);
					rect->margin[1] = num;
				}
				goto end_matching;
			}

			/*处理下边距*/
			if(strcasecmp(css_name , MARGIN_BOTTOM) == 0){
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				end = strlen(css_value);
				end--;
				if(css_value[end] == '%'){	//若采用百分比表示
					num = atoi(css_value);
					num = 0 - num;
					rect->margin[2] = num;
				}else if(strcasecmp(css_value , "auto") == 0){
					rect->margin[2] = MAR_AUTO;
				}else{	//采用绝对值
					num = atoi(css_value);
					rect->margin[2] = num;
				}
				goto end_matching;
			}

			/*处理左边距*/
			if(strcasecmp(css_name , MARGIN_LEFT) == 0){
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				end = strlen(css_value);
				end--;
				if(css_value[end] == '%'){	//若采用百分比表示
					num = atoi(css_value);
					num = 0 - num;
					rect->margin[3] = num;
				}else if(strcasecmp(css_value , "auto") == 0){
					rect->margin[3] = MAR_AUTO;
				}else{	//采用绝对值
					num = atoi(css_value);
					rect->margin[3] = num;
				}
				goto end_matching;
			}
			goto unknown_style;
			break;

		case 'P':
		case 'p':
			/*处理顶内边距*/
			if(strcasecmp(css_name , PAD_TOP) == 0){
				if(!rect->extra){	/*根据需要获取额外CSS属性*/
					rect->extra = get_extra();
				}

				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				end = strlen(css_value);
				end--;
				if(css_value[end] == '%'){	//若采用百分比表示
					num = atoi(css_value);
					num = 0 - num;
					rect->extra->padding[0] = num;
				}else{	//采用绝对值
					num = atoi(css_value);
					rect->extra->padding[0] = num;
				}
				goto end_matching;
			}

			/*处理右内边距*/
			if(strcasecmp(css_name , PAD_RIGHT) == 0){
				if(!rect->extra){	/*根据需要获取额外CSS属性*/
					rect->extra = get_extra();
				}

				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				end = strlen(css_value);
				end--;
				if(css_value[end] == '%'){	//若采用百分比表示
					num = atoi(css_value);
					num = 0 - num;
					rect->extra->padding[1] = num;
				}else{	//采用绝对值
					num = atoi(css_value);
					rect->extra->padding[1] = num;
				}
				goto end_matching;
			}
			/*处理下内边距*/
			if(strcasecmp(css_name , PAD_BOTTOM) == 0){
				if(!rect->extra){	/*根据需要获取额外CSS属性*/
					rect->extra = get_extra();
				}

				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				end = strlen(css_value);
				end--;
				if(css_value[end] == '%'){	//若采用百分比表示
					num = atoi(css_value);
					num = 0 - num;
					rect->extra->padding[2] = num;
				}else{	//采用绝对值
					num = atoi(css_value);
					rect->extra->padding[2] = num;
				}
				goto end_matching;
			}
			/*处理左内边距*/
			if(strcasecmp(css_name , PAD_LEFT) == 0){
				if(!rect->extra){	/*根据需要获取额外CSS属性*/
					rect->extra = get_extra();
				}

				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				end = strlen(css_value);
				end--;
				if(css_value[end] == '%'){	//若采用百分比表示
					num = atoi(css_value);
					num = 0 - num;
					rect->extra->padding[3] = num;
				}else{	//采用绝对值
					num = atoi(css_value);
					rect->extra->padding[3] = num;
				}
				goto end_matching;
			}

			/*位置*/
			if(strcasecmp(css_name , POSITION) == 0){
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				if(strcasecmp(css_value , "static") == 0){
					rect->position = POS_STATIC;
				}else if(strcasecmp(css_value , "relative") == 0){
					rect->position = POS_RELATIVE;
				}else if(strcasecmp(css_value , "absolute") == 0){
					rect->position = POS_ABSOLUTE;
				}else{
					printf("Warning: unknown position:%s\n" , css_value);
				}
				goto end_matching;
			}
			goto unknown_style;
			break;

		case 'R':
		case 'r':
			/*分析距离父元素右端 实际上可以理解为margin-right*/
			if(strcasecmp(css_name , RIGHT) == 0){
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				end = strlen(css_value);
				end--;
				if(css_value[end] == '%'){	//若采用百分比表示
					num = atoi(css_value);
					num = 0 - num;
					rect->margin[1] = num;
				}else if(strcasecmp(css_value , "auto") == 0){
					rect->margin[1] = 0;
				}else{	//采用绝对值
					num = atoi(css_value);
					rect->margin[1] = num;
				}
				goto end_matching;
			}
			goto unknown_style;
			break;

		case 'T':
		case 't':
			/*分析距离父元素上端 实际上可以理解为margin-top*/
			if(strcasecmp(css_name , TOP) == 0){
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				end = strlen(css_value);
				end--;
				if(css_value[end] == '%'){	//若采用百分比表示
					num = atoi(css_value);
					num = 0 - num;
					rect->margin[0] = num;
				}else if(strcasecmp(css_value , "auto") == 0){
					rect->margin[0] = 1;
				}else{	//采用绝对值
					num = atoi(css_value);
					rect->margin[0] = num;
				}
				goto end_matching;
			}
			/*处理文字对齐*/
			if(strcasecmp(css_name , TEXT_ALIGN) == 0){
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				if(strcasecmp(css_value , "left") == 0){
					rect->text_align = TEA_LEFT;
				}else if(strcasecmp(css_value , "center") == 0){
					rect->text_align = TEA_CENTER;
				}else if(strcasecmp(css_value , "right") == 0){
					rect->text_align = TEA_RIGHT;
				}else if(strcasecmp(css_value , "justify") == 0){
					rect->text_align = TEA_JUSTIFY;
				}else{
					printf("Warning: unknown text-align:%s\n" , css_value);
				}
				goto end_matching;
			}
			if(strcasecmp(css_name , TEXT_DEC) == 0){
				goto end_matching;
			}
			if(strcasecmp(css_name , TEXT_TRANS) == 0){
				goto end_matching;
			}
			if(strcasecmp(css_name , TEXT_INDENT) == 0){
				goto end_matching;
			}
			goto unknown_style;
			break;

		case 'V':
		case 'v':
			if(strcasecmp(css_name , VER_ALIGN) == 0){
				goto end_matching;
			}
			goto unknown_style;
			break;

		case 'W':
		case 'w':
			/*处理宽度*/
			if(strcasecmp(css_name , WIDTH) == 0){
				if(css_value[0] == '"'){	//若属性是用双引号包括
					css_value++;
					end = strlen(css_value);
					css_value[--end] = 0;
				}

				end = strlen(css_value);
				end--;
				if(css_value[end] == '%'){	//若采用百分比表示
					num = atoi(css_value);
					num = 0 - num;
					rect->width = num;
				}else if(strcasecmp(css_value , "auto") == 0){
					rect->width = WID_AUTO;
				}else{	//采用绝对值
					num = atoi(css_value);
					rect->width = num;
				}
				goto end_matching;
			}

			if(strcasecmp(css_name , WORD_SPACE) == 0){
				goto end_matching;
			}
			if(strcasecmp(css_name , WHITE_SPACE) == 0){
				goto end_matching;
			}
			goto unknown_style;
			break;

unknown_style:

		default:
			printf("Waring:unknown style: %s!\n" , css_name);
		}



end_matching:
	;
//		printf("#css_name:%s~~~css_value:%s\n" , css_name , css_value);

		start = next;
	}	/*end while*/


	return 0;
}

/*
 * 删除一个css样式表
 */
static int delete_style(void *data , void *arg){
	style_name *s_name;
	s_name = (style_name *)data;

	if(!s_name){	/*如果该结点没有样式那么直接返回*/
		return -1;
	}


	if(s_name->s_value){	/*如果该样式名拥有样式，那么首先将引用的样式减一(因为该样式名将被删除)如果样式引用为0删除之*/
		s_name->s_value->refer--;
		if(s_name->s_value->refer == 0){
			free(s_name->s_value->value);
			free(s_name->s_value);
		}
	}
	free(s_name);
	return -1;
}

int destruct_css_tb(css_tb *this){
	int i;

	for(i=0; i<this->get_nr_key(this); i++){	/*依次遍历nr_key条链表。删除对应的数据*/
		this->set_line(this , i);
		this->foreach(this , delete_style , NULL);
	}

	return sim_hash_tb_destroy(this);	/*最后删除hash表*/
}



//////////////////////////////////////////////////////////////////////////

/*根据一个名字产生一个样式名*/
static style_name *make_style_name(char *name){
	if(!name){
		return NULL;
	}

	style_name *s_name = (style_name *)malloc(sizeof(style_name));
	memset(s_name , 0 , sizeof(style_name));
	strcpy(s_name->name , name);

	return s_name;
}

/*根据样式产生一个样式值*/
static style_value *make_style_value(char *value){
	if(!value){
		return NULL;
	}

	style_value *s_value = (style_value *)malloc(sizeof(style_value));

	s_value->refer = 0;
	s_value->value = (char *)malloc(strlen(value) + 1);	/*根据输入的字符串长度动态生成value起长度为+1*/
	memset(s_value->value , 0 , strlen(value) + 1);
	strcpy(s_value->value , value);

	return  s_value;
}
/*将一个样式值赋予一个样式名*/
static int s_v2s_n(style_name *s_name , style_value *s_value){
	if(!s_name || !s_value){
		return -1;
	}

	s_value->refer++;	/*被引用次数加一*/
	s_name->s_value = s_value;
	return 0;
}
