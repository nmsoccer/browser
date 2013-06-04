/*
 * parse.c
 *
 *  Created on: 2010-12-8
 *      Author: leiming
 */

/*
 *输入一个html文件。根据get_token产生的记号分析该文件然后产生相关的结点
 *构造出一棵DOM树
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ds/binary_tree.h>

#include "parse.h"
#include "token_types.h"
#include "get_token.h"
#include "dom_node.h"
#include "html_label.h"

#define VALUE_LEN	20
#define INPUT_LEN		100

unsigned int use_css = 0;	/*是否使用CSS*/


/*
 * 根据当前结点找到其父结点(注意在孩子兄弟结点中作为右孩子关系的不是父子，必须是左孩子关系)
 */
static int find_parent(dom_tree *this);

/*
 * @param:需要解析的文件名
 * 返回一课生成DOM树的根指针
 * 该树是一棵孩子兄弟结点类型的二叉树木
 */

dom_tree *parse(char *file_name){
	dom_tree *dom;
	dom_node *pn;
	FILE *fp;

	int  bool_flag;
	int type;
	char *p;
	char value[VALUE_LEN];
	char input[INPUT_LEN];

	unsigned long line = 0;

	fp = fopen(file_name , "r");
	if(fp == NULL){
		printf("open file failed!\n");
		return NULL;
	}

	dom = binary_tree_new();

	pn = (dom_node *)malloc(sizeof(dom_node));
	memset(pn , 0 , sizeof(dom_node));
	strcpy(pn->value , "root");
	pn->parent = NULL;
	pn->brother = NULL;
	pn->type = ROOT;
	pn->rect.extra = (extra_element *)malloc(sizeof(extra_element));	/*根结点拥有EXTRA结点*/
	memset(pn->rect.extra , 0 , sizeof(extra_element));

	dom->ins_root(dom , pn);
	dom->set_base(dom);		/*设立一个将要发展的父结点*/

	while(1){
		/*
		 * test end inputting file condition
		 */
		if(fgets(input , INPUT_LEN , fp) == NULL){
//			printf("fgets failed!\n");
			break;
		}
		if(strlen(input) == 0){
//			printf("end of file!\n");
			break;
		}
		line++;	/*从文件中每新读取一行。行数加1 列数置0*/

//		printf("line:%d is:%s\n" , line , input);

		/*start parsing*/
		p = input;
		while(1){
			if(strlen(p) == 0){
				break;
			}

			p = get_token(value , &type , p , VALUE_LEN);

			switch(type){
			/*
			 * 树建立的为孩子兄弟二叉树。任一个结点的真正后代是其左孩子的右孩子链
			 *
			 * 树的base指针始终指向当前要扩展的结点
			 * 凡是遇见LABEL<xx>首先将其作为当前扩展结点的后代放入其左子之右子链中
			 * 然后将其设为当前扩展结点。将LABEL之中内容(属性)作为后代放入其左子之右子链中
			 * 遇见TEXT作为当前要发展LABEL结点的子结点
			 * 遇见</>表明一个LABEL结点添加孩子结束 我们沿着父指针链找到该LABEL之上一层需要发展的父结点继续重复一的故事
			 */

///////////////////////////////////////////////////////////////////////////////////////////////////
			case LABEL_START:	/*一个标签开始*/
				bool_flag = 1;	/*表示LABEL之中出现的第一个字符串*/

				while(1){
					p = get_token(value , &type , p , VALUE_LEN);	/*获取标签内容*/

					switch(type){

					case STRING:
						pn = (dom_node *)malloc(sizeof(dom_node));
						memset(pn , 0 , sizeof(dom_node));
						strcpy(pn->value , value);
						pn->parent = (dom_node *)dom->base->data;


						if(bool_flag){		/*标签的第一个是字符串说明是一个新的标签。我们将此标签作为当前扩展LABEL的子结点并设置其为当前
												扩展LABEL结点*/
							pn->type = ELEMENT;
						}else{				/*在标签中的非第一个字符串只能说明这是属性*/
							pn->type = ATTR;
						}

						if(dom->base->left == NULL){	/*若当前扩展LABEL无孩子那么这个孩子是第一个。作为其左孩子*/
							dom->goto_base(dom);
							dom->ins_left(dom , pn);
						}else{	/*当前扩展结点有孩子了则将当前孩子放入其左子的右孩子链*/
							dom->goto_base(dom);	/*找到当前base结点*/
							dom->goto_left(dom);
							while(!dom->goto_right(dom)){	/*搜索其孩子结点之最后一位*/
							}
							dom->ins_right(dom , pn);
						}

						if(bool_flag){	/*如果是标签中出现的第一个字符串那么我们要将该结点设置为当前扩展LABEL结点*/
							dom->set_base(dom);		/*将当前LABEL设置为扩展结点*/
							bool_flag = 0;		/*标签中以后出现的字符串将不作为扩展基本点*/
						}

						if(dom->current->parent->right == dom->current){	/*如果当前结点是其父结点的右孩子说明它们是兄弟关系*/
							pn->brother = (dom_node *)dom->current->parent->data;
						}else{	/*否则说明它是后辈中最长的结点*/
							pn->brother = NULL;
						}


						break;

					case SIGN:
						if(bool_flag){	/*表明是<!-->。以!开始全部是注释。之后全部省略*/
							while(1){
								p = get_token(value , &type , p , VALUE_LEN);	/*剩余标签内容作废不再分析直到遇见>标志结束*/

								if(type == LABEL_END){
									goto _bottom_while;
								}
							}
						}
						break;

					case ASSIGN:	/*表明是为为属性赋值*/
						p = get_token(value , &type , p , VALUE_LEN);

						if(type != DQUOTE){	/*要求属性必须以" "包括住 否则出错*/
							printf("error: line:%d attr lack \" \n" , line);
							goto _err;
						}

						pn = (dom_node *)dom->read(dom);
						strcat(pn->value , ":");

						while(1){	/*持续为当前属性结点赋值直到遇见结束 " */
							p = get_token(value , &type , p , VALUE_LEN);

							switch(type){
							case DQUOTE:
								goto _bottom_while_label;
							case LABEL_END:	/*没有出现下一个" 而是出现>表示缺少属性结束符号*/
								printf("error: line:%d attr lack \" \n" , line);
								goto _err;
							default:
								strcat(pn->value , value);
								break;
							}

						}
						/*结束赋值*/
						break;

					case BACKSLASH:	/*遇见/表明一个当前扩展结点完成*/
						dom->goto_base(dom);	/*回到当前扩展的LABEL结点*/
						find_parent(dom);	/*找到此LABEL结点的父LABEL结点*/
						dom->set_base(dom);	/*在其父结点上准备扩展*/

						while(1){
							p = get_token(value , &type , p , VALUE_LEN);	/*剩余标签内容作废不再分析直到遇见>标志结束*/
							if(type == LABEL_END){
								goto _bottom_while;
							}
						}
						break;

					case LABEL_END:	/*遇见>表明分析完成一个LABEL<>*/
						goto _bottom_while;
					}

_bottom_while_label:
	;
				}
				break;	/*end fo parsing LABEL_START*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			case CTRL:	/*\n忽略之*/
				break;

			default:
				pn = (dom_node *)malloc(sizeof(dom_node));	/*文本区域 此处不调用make_node函数*/
				memset(pn , 0 , sizeof(dom_node));
				pn->type = TEXT;
				strcpy(pn->value , value);
				strcat(pn->value , " ");
				pn->parent = (dom_node *)dom->base->data;

				if(dom->base->left == NULL){	/*若当前扩展LABEL无孩子那么这个孩子是第一个。作为其左孩子*/
					dom->goto_base(dom);
					dom->ins_left(dom , pn);
				}else{	/*当前扩展结点有孩子了则将当前孩子放入其左子的右孩子链*/
					dom->goto_base(dom);	/*找到当前base结点*/
					dom->goto_left(dom);
					while(!dom->goto_right(dom)){	/*搜索其孩子结点之最后一位*/
					}
					dom->ins_right(dom , pn);
				}

				if(dom->current->parent->right == dom->current){	/*如果当前结点是其父结点的右孩子说明它们是兄弟关系*/
					pn->brother = (dom_node *)dom->current->parent->data;
				}else{	/*否则说明它是后辈中最长的结点*/
					pn->brother = NULL;
				}

				break;
			}

_bottom_while:
	;

		}	/*end while*/
	}	/*end reading file*/
	printf("done!\n");
	fclose(fp);

	return dom;

_err:
	;
	destruct(dom);
	return NULL;
} /*end function*/


/*根据第一步生成的dom_tree并且根据属性的设置加入CSS样式
 * 同时为dom_tree之元素盒子成员赋值
*/
static int set;	/*表明标签盒子是否被设置*/
static dom_node *current = NULL;	/*表明正在构建的当前结点*/
static char label_class[32];	/*特殊类名 label.class*/
static char style_value[200];	/*一个标签的样式值得*/


static int do_scan_dom(void *data , void *arg){
	char *attr_name;
	char *attr_value;
	char *style;
	dom_node *pn = (dom_node *)data;

/*
	printf("type is:%d  " , pn->type);
	printf("node is:%s\n" , pn->value);
	return -1;
*/

	switch(pn->type){
/////////////////////////////////////////
	case ELEMENT:	/*标签*/
		if(current){	/*之前的标签样式*/
			if(use_css){
				if(strlen(style_value) == 0){
//					printf("$$$$$$label %s---no style\n" , current->value);
				}else{
//					printf("$$$$$$label %s---style is:%s\n" , current->value , style_value);
					fill_rect(&current->rect , style_value);
//					show_money(current);
				}
			}else{
//				printf("$$$$$$label %s---no style\n" , current->value);
			}
		}

		current = pn;/*当前是标签那么current设置为当前之标签。表示将要构建此标签的盒子*/
		set = 0;	/*新标签没有被设置*/
		memset(style_value , 0 , 200);	/*存储当前标签样式*/
		memset(label_class , 0 , 32);	/*用于处理特殊类*/
		strcpy(label_class , current->value);
		strcat(label_class , ".");

		/*检测LABEL_TABLE查看是否有对应的样式*/
		if(use_css){
			style = (char *)get_css_label(current->value);
			if(style){
				strcat(style_value , style);
			}
		}

		goto end_do_scan;
		break;
//////////////////////////////////////////
	case ATTR:	/*主要是负责解析属性*/
		attr_name = pn->value;	/*属性名*/
		attr_value = strchr(pn->value , ':');	/*属性值*/

		*attr_value = 0;
		attr_value++;

		/*ATTR TYPE*/

		/*CLASS*/
		if(strcasecmp(attr_name , CLASS) == 0){
			strcat(label_class , attr_value);

			style = (char *)get_css_class(attr_value);	/* 普通的类.class*/
			if(style){
				strcat(style_value , style);
			}

			style = (char *)get_css_label(label_class);	/*特殊的类label.class 注意其样式在LABEL表中*/
			if(style){
				strcat(style_value , style);
			}

			goto end_do_scan;
		}

		/*ID*/
		if(strcasecmp(attr_name , ID) == 0){
			style = (char *)get_css_id(attr_value);	/*特殊的类label.class 注意其样式在LABEL表中*/
			if(style){
				strcat(style_value , style);
			}

			goto end_do_scan;
		}

		/*HREF*/
		if(strcasecmp(attr_name , HREF) == 0){
			if(strcasecmp(current->value , A) == 0){	/*当前标签是超链接*/
				goto end_do_scan;
			}
			if(strcasecmp(current->value , LINK) == 0){	/*如果目前标签是LINK*/
				use_css = 1;
				parse_css(attr_value);
				goto end_do_scan;
			}
		}
		break;

////////////////////////////////////
	case TEXT:
//		printf("-->text in %s:%s\n" , current->value , pn->value);
		break;

	case ROOT:
		break;

	default:
		printf("ERR: meets unknown type of dom node when scanning!\n");
		pause();
		break;
	}	/*end switch*/

end_do_scan:
	;

	return -1;
}	/*end func*/



int scan_dom(dom_tree *this){
	this->goto_root(this);
	this->pre_trav(this , do_scan_dom , NULL , CNG);/*最后一个结点没有处理 current指向它*/
	/*处理最后一个current标签*/
	if(current){	/*之前的标签样式*/
		if(use_css){
			if(strlen(style_value) == 0){
//				printf("$$$$$$label %s---no style\n" , current->value);
			}else{
//				printf("$$$$$$label %s---style is:%s\n" , current->value , style_value);
				fill_rect(&current->rect , style_value);
//				show_render(current);
			}
		}else{
//			printf("$$$$$$label %s---no style\n" , current->value);
		}
	}
	return 0;
}

/*
 * 回收MOD树占有的内存
 * @param:树之根结点
 */
static int free_node(void *data , void *arg){
	dom_node *pn;
	pn = (dom_node *)data;

	/*将结点内容所指向结构销毁并持续遍历*/
	if(pn->rect.extra){
		/*如果存在额外CSS内容*/
		free(pn->rect.extra);
	}

	free(pn);
	data = NULL;
	return -1;
}

int destruct(dom_tree *this){
	this->goto_root(this);
	this->hier_trav(this , free_node , NULL , NCNG);	/*通过回调函数销毁树结点的内容*/
	this->goto_root(this);
	binary_tree_destroy(this);	/*销毁树*/

	return 0;
}





/*
 * #########################################
 * private functions
 */

/*
 * 根据输入的内容识别并由此产生相应类型和类别的结点

static dom_node *make_ele_node(char *s){
	dom_node *p;
	p = (dom_node *)malloc(sizeof(dom_node));

	strcpy(p->value , s);
	p->type = ELEMENT;
	return p;
/*
	if(strcmp(s , "root") == 0){
		p->type = ROOT;
		return p;
	}
	if((strcmp(s , "html") == 0) || (strcmp(s , "HTML") == 0)){
		p->type = ELEMENT;
		p->class = HTML;
		return p;
	}
	if((strcmp(s , "head") == 0) || (strcmp(s , "HEAD") == 0)){
		p->type = ELEMENT;
		p->class = HEAD;
		return p;
	}
	if((strcmp(s , "title") == 0) || (strcmp(s , "TITLE") == 0)){
		p->type = ELEMENT;
		p->class = TITLE;
		return p;
	}
	if((strcmp(s , "body") == 0) || (strcmp(s , "BODY") == 0)){
		p->type = ELEMENT;
		p->class = BODY;
		return p;
	}
	if((strcmp(s , "table") == 0) || (strcmp(s , "TABLE") == 0)){
		p->type = ELEMENT;
		p->class = TABLE;
		return p;
	}
	if((strcmp(s , "tr") == 0) || (strcmp(s , "TR") == 0)){
		p->type = ELEMENT;
		p->class = TR;
		return p;
	}
	if((strcmp(s , "td") == 0) || (strcmp(s , "TD") == 0)){
		p->type = ELEMENT;
		p->class = TD;
		return p;
	}

//////////////////////////
	if((strcmp(s , "id") == 0) || (strcmp(s , "ID") == 0)){
		p->type = ATTR;
		p->class = ID;
		return p;
	}
	if((strcmp(s , "name") == 0) || (strcmp(s , "NAME") == 0)){
		p->type = ATTR;
		p->class = NAME;
		return p;
	}
	if((strcmp(s , "class") == 0) || (strcmp(s , "CLASS") == 0)){
		p->type = ATTR;
		p->class = CLASS;
		return p;
	}
	if((strcmp(s , "style") == 0) || (strcmp(s , "STYLE") == 0)){
		p->type = ATTR;
		p->class = STYLE;
		return p;
	}
	if((strcmp(s , "lang") == 0) || (strcmp(s , "LANG") == 0)){
		p->type = ATTR;
		p->class = LANG;
		return p;
	}


	return NULL;
}
*/


static int find_parent(dom_tree *this){
	while(1){
		if(this->current->parent->left == this->current){	/*只有当前结点是其父结点之左子才是真正的父子关系*/
			this->goto_parent(this);
			return 0;
		}

		this->goto_parent(this);

	}

	return -1;
}



