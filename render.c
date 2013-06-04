/*
 * render.c
 *
 *  Created on: 2011-1-16
 *      Author: leiming
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "html_label.h"
#include "dom_node.h"
#include "css_attr.h"
#include "parse.h"
#include "render.h"

char text_content[TEXT_CONTENT_LEN];
int screen_width = 800;
int screen_height = 600;

static dom_node *current = NULL;

/*
 * 该函数最后扫描一遍DOM树用来确定每个标签之矩形的基本要素存在。否则试图为其寻找之
 */
static int do_render_dom(void *data , void *arg){
	dom_node *pn = (dom_node *)data;
	element *box;

	dom_node *parent;
	dom_node *brother;

	float ftemp;

	if(!pn){
		printf("NULL\n");
		return -1;
	}

	if(pn->type == ROOT){
		/*base attr*/
		pn->rect.border_width = 0;
		pn->rect.background = WHITE;
		pn->rect.border_color = BLACK;
		pn->rect.border_style = BRS_NONE;

		pn->rect.clear = CLE_NONE;
		pn->rect.floating = FLO_NONE;
		pn->rect.foreground = BLACK;
		pn->rect.font_size = 10;
		pn->rect.font_family = FOF_ARIAL;

		pn->rect.position = POS_STATIC;
		pn->rect.text_align = TEA_LEFT;

		return -1;
	}
	if(pn->type == ATTR){
//		printf("attr in %s is %s\n" , current->value , pn->value);
		return -1;
	}
	if(pn->type == TEXT){
		printf("		@@@@text in %s:%s\n" , current->value , pn->value);
		return -1;
	}
	/*ELEMENT*/
	current = pn;
	printf(">>>render %s  parent is:%s" , current->value , current->parent->value);
	brother = current->brother;/*找到兄弟类型为元素的结点*/
	while(brother){
		if(brother->type == ELEMENT){
			break;
		}
		brother = brother->brother;
	}
	if(brother){
		printf(" brother type:%d value is:%s\n" , brother->type , brother->value);
	}else{
		printf("\n");
	}


	if(strcasecmp(pn->value , HTML) == 0){
		return -1;
	}
	if(strcasecmp(pn->value , HEAD) == 0){
		return -1;
	}
	if(strcasecmp(pn->value , TITLE) == 0){
		return -1;
	}
	if(strcasecmp(pn->value , LINK) == 0){
		return -1;
	}


	box = &pn->rect;
	/*处理标签宽度*/
	if(box->width){
		if(box->width < 0){	/*如果是百分比那么是相对于父对象计算*/
			box->width = (0 - box->width) * (pn->parent->rect.width) / 100 ;
		}
		if(box->width == 0){	/*处理宽度太小就设置为1*/
			box->width = 1;
		}
	}else{/*默认的长度*/
		if(strcasecmp(current->value , BODY) == 0){	/*body */
			box->width = screen_width;
			goto end_render_width;
		}
		if(strcasecmp(current->value , H1) == 0){	/*h1 */
			box->width = screen_width;
			goto end_render_width;
		}
		if(strcasecmp(current->value , H2) == 0){	/*h2 */
			box->width = screen_width;
			goto end_render_width;
		}else{
			box->width = 10;
		}
	}
end_render_width:
	printf("	width is:%d\n" , box->width);


	/*处理标签高度*/
	if(box->height){
		if(box->height < 0){	/*如果是百分比那么是相对于父对象计算*/
			box->height = (0 - box->height) * (pn->parent->rect.height) / 100 ;
		}
		if(box->height == 0){	/*处理高度太小就设置为1*/
			box->height = 1;
		}
	}else{/*默认的高度*/
		if(strcasecmp(current->value , BODY) == 0){	/*body */
			box->height = screen_height;
			goto end_render_height;
		}
		if(strcasecmp(current->value , H1) == 0){	/*h1 */
			box->height = 10;
			goto end_render_height;
		}
		if(strcasecmp(current->value , H2) == 0){	/*h2 */
			box->height = 10;
			goto end_render_height;
		}else{
			box->height = 10;
		}
	}
end_render_height:
	printf("	height is:%d\n" , box->height);


	/*处理上边距*/
	if(box->margin[0]){
		if(box->margin[0] < 0){	/*如果是百分比那么是相对于父对象高度计算*/
			box->margin[0] = (0 - box->margin[0]) * (pn->parent->rect.height) / 100 ;
		}
		if(box->margin[0] == 0){	/*太小就设置为1*/
			box->margin[0] = 1;
		}
//		printf("	margin top is:%d" , box->margin[0]);
	}

	/*处理右边距*/
	if(box->margin[1]){
		if(box->margin[1] < 0){	/*如果是百分比那么是相对于父对象长度计算*/
			box->margin[1] = (0 - box->margin[1]) * (pn->parent->rect.width) / 100 ;
		}
		if(box->margin[1] == 0){	/*太小就设置为1*/
			box->margin[1] = 1;
		}
//		printf("	margin right is:%d" , box->margin[1]);
	}


	/*处理下边距*/
	if(box->margin[2]){
		if(box->margin[2] < 0){	/*如果是百分比那么是相对于父对象高度计算*/
			box->margin[2] = (0 - box->margin[2]) * (pn->parent->rect.height) / 100 ;
		}
		if(box->margin[2] == 0){	/*太小就设置为1*/
			box->margin[2] = 1;
		}
//		printf("	margin bottom is:%d" , box->margin[2]);
	}


	/*处理左边距*/
	if(box->margin[3]){
		if(box->margin[3] < 0){	/*如果是百分比那么是相对于父对象长度计算*/
			box->margin[3] = (0 - box->margin[3]) * (pn->parent->rect.width) / 100 ;
		}
		if(box->margin[3] == 0){	/*太小就设置为1*/
			box->margin[3] = 1;
		}
//		printf("	margin left is:%d" , box->margin[3]);
	}


	/*EXTRA EXISTS*/
	if(box->extra){
		/*处理内上边距*/
		if(box->extra->padding[0]){
			if(box->extra->padding[0] < 0){	/*如果是百分比那么是相对于父对象高度计算*/
				box->extra->padding[0] = (0 - box->extra->padding[0]) * (pn->parent->rect.height) / 100 ;
			}
//			printf("	padding top is:%d" , box->extra->padding[0]);
		}


		/*处理内右边距*/
		if(box->extra->padding[1]){
			if(box->extra->padding[1] < 0){	/*如果是百分比那么是相对于父对象宽度计算*/
				box->extra->padding[1] = (0 - box->extra->padding[1]) * (pn->parent->rect.width) / 100 ;
			}
//			printf("	padding right is:%d" , box->extra->padding[1]);
		}


		/*处理内下边距*/
		if(box->extra->padding[2]){
			if(box->extra->padding[2] < 0){	/*如果是百分比那么是相对于父对象高度计算*/
				box->extra->padding[2] = (0 - box->extra->padding[2]) * (pn->parent->rect.height) / 100 ;
			}
//			printf("	padding bottom is:%d" , box->extra->padding[2]);
		}


		/*处理内左边距*/
		if(box->extra->padding[3]){
			if(box->extra->padding[3] < 0){	/*如果是百分比那么是相对于父对象宽度计算*/
				box->extra->padding[3] = (0 - box->extra->padding[3]) * (pn->parent->rect.width) / 100 ;
			}
//			printf("	padding right is:%d" , box->extra->padding[3]);
		}
	}

	/*处理标签位置*/
	if(!box->position){
		parent = current->parent;
		while(parent){
			if(parent->rect.position){
				box->position = parent->rect.position;
				break;
			}
			parent = parent->parent;
		}
	}
//	printf("	position is:%d" , box->position);

	/*处理浮动*/
	if(!box->floating){	/*如果没有设置浮动那么继承父之浮动*/
		parent = current->parent;
		while(parent){
			if(parent->rect.floating){
				box->floating = parent->rect.floating;
				break;
			}
			parent = parent->parent;
		}
	}
//	printf("	floating is:%d" , box->floating);

	/*
	 * 左上角的x与y坐标拥有不同的策略；
	 * 位置绝对情况下都是绝对坐标
	 * 位置默认情况与相对位置情况下
	 *(默认位置等于相对位置):
	 * 不浮动:x是相对父而y是相对兄弟
	 * 浮动:x相对兄弟y是等高
	 */

	/*处理标签左上x坐标
	 * 首先考察位置：如故是绝对位置那么x是相对于屏幕决定；
	 * 如果是相对位置或默认那么x是相对于父标签；
	 *
	 * 在相对位置的情况下考擦浮动：
	 * 如果不浮动那么直接考察相对于父标签的偏移不考虑兄弟标签的位置；
	 * 只有在左浮动或者右浮动的情况下才考虑兄弟标签的位置
	 */
	if(!box->x){/*如果x为0表示需要通过查找其父、兄等标签来获取*/
		if(strcasecmp(current->value , BODY) == 0){
			box->x = 0;
			goto end_render_x;
		}

		if(box->position == POS_ABSOLUTE){	/*如果是绝对定位*/
			if(box->margin[3]){	/*如果左边距存在 */
				box->x = box->margin[3];
				goto end_render_x;
			}
			if(box->margin[1]){	/*如果右边距存在 左右边距同时存在的情况下以左边距为准*/
				box->x = screen_width - box->margin[1] - box->width;
				goto end_render_x;
			}
			//如果没有设置则为0
		}	/*end position absolute*/

		if(box->position == POS_STATIC || box->position == POS_RELATIVE){	/*如果是相对定位或者默认定位*/
			if(box->floating == FLO_LEFT){	/*如果左浮动*/
				brother = current->brother;/*找到兄弟类型为元素的结点*/
				while(brother){
					if(brother->type == ELEMENT){
						break;
					}
					brother = brother->brother;
				}
				if(!brother){	/*如果这是父元素的第一个子元素即它没有兄弟。处置方法和floating_none一样*/
					goto floating_none;
				}
				/*拥有兄弟元素*/
				box->x =  brother->rect.x + brother->rect.width + brother->rect.margin[1] + box->margin[3];
				goto end_render_x;

			}	/*end floating left*/

			if(box->floating == FLO_RIGHT){	/*如果右浮动*/
				brother = current->brother;/*找到兄弟类型为元素的结点*/
				while(brother){
					if(brother->type == ELEMENT){
						break;
					}
					brother = brother->brother;
				}
				if(!brother){	/*如果这是父元素的第一个子元素即它没有兄弟*/
									/*偏移到父元素最右边*/
					box->x = current->parent->rect.x + (current->parent->rect.width - box->width - box->margin[1]);
					if(current->parent->rect.extra){	/*需要考虑内右边界*/
						box->x = box->x - current->parent->rect.extra->padding[1];
					}
					goto end_render_x;
				}
				/*拥有兄弟元素*/
				box->x =  brother->rect.x - box->width - box->margin[1];
				goto end_render_x;

			}	/*end floating right*/

			if(box->floating == FLO_NONE){	/*如果不浮动那么直接相对父元素 不考虑兄弟元素*/
floating_none:
				if(box->margin[3]){	/*如果左边距存在 */
					box->x = current->parent->rect.x + box->margin[3];
					if(current->parent->rect.extra){	/*需要考虑内左边界*/
						box->x = box->x + current->parent->rect.extra->padding[3];
					}
					goto end_render_x;
				}
				if(box->margin[1]){	/*如果右边距存在*/
					box->x = current->parent->rect.x + (current->parent->rect.width - box->width - box->margin[1]);
					if(current->parent->rect.extra){	/*需要考虑内右边界*/
						box->x = box->x - current->parent->rect.extra->padding[1];
					}
					goto end_render_x;
				}
				/*如果没有设置偏移那么偏移为最左边*/
				box->x = current->parent->rect.x + 1;
				if(current->parent->rect.extra){	/*需要考虑内左边界*/
					box->x = box->x + current->parent->rect.extra->padding[3] + 1;
				}
				goto end_render_x;

			}	/*end floating none*/

		}	/*end static&relative*/
	}
end_render_x:
	printf("	x is:%d\n" , box->x);

	/*处理左上y坐标
	 * 首先考察位置：如故是绝对位置那么y是相对于屏幕决定；
	 * 如果是默认或者相对位置那么y是相对于父标签；
	 *
	 * 在默认和相对位置的情况下考察浮动：
	 * 如果不浮动那么在兄弟标签下方；
	 * 如果左浮动或者右浮动的情况下与兄弟标签等高
	 */
	if(!box->y){
		if(strcasecmp(current->value , BODY) == 0){
			box->y = 0;
			goto end_render_y;
		}

		if(box->position == POS_ABSOLUTE){
			if(box->margin[0]){	/*如果上边距存在 */
				box->y = box->margin[0];
				goto end_render_y;
			}
			if(box->margin[2]){	/*如果下边距存在 上下边距同时存在的情况下以上边距为准*/
				box->y = screen_height - box->margin[2] - box->height;
				goto end_render_y;
			}
			//如果没有设置则为0
		}	/*position absolute*/

		if(box->position == POS_STATIC || box->position == POS_RELATIVE){	/*如果是相对定位或者默认定位*/
			while(brother){
				if(brother->type == ELEMENT){
					break;
				}
				brother = brother->brother;
			}

			if(!brother){	/*如果是长兄 那么紧靠父之上*/
				if(box->margin[0]){	/*top*/
					box->y = current->parent->rect.y + box->margin[0];
					if(current->parent->rect.extra){
						box->y += current->parent->rect.extra->padding[0];
					}
					goto end_render_y;
				}
				if(box->margin[2]){	/*bottom*/
					box->y = current->parent->rect.y + current->parent->rect.height - box->margin[2];
					if(current->parent->rect.extra){
						box->y -= current->parent->rect.extra->padding[2];
					}
					goto end_render_y;
				}
				//如果都没有设置
				box->y = current->parent->rect.y + 1;
				goto end_render_y;
			}
			/*兄弟存在*/
			if(box->floating == FLO_NONE){	/*如果没有浮动那么在兄弟的下方*/
				box->y = brother->rect.y + brother->rect.height + box->margin[0] + 1;
				goto end_render_y;
			}
			if(box->floating == FLO_LEFT || box->floating == FLO_RIGHT){	/*存在浮动则与兄弟等高*/
				box->y = brother->rect.y;
				goto end_render_y;
			}

		}	/*position static&relative*/
	}
end_render_y:
	printf("	y is:%d\n" , box->y);

	/*处理边框宽度*/
	if(!box->border_width){	/*如果没有设置宽度那么寻找父标签的情况*/
		parent = current->parent;
		while(parent){	/*逐次寻找父标签的宽度设置直到为空或者宽度不为0*/
			if(parent->rect.border_width){
				box->border_width = parent->rect.border_width;
				break;
			}
			parent = parent->parent;
		}
	}
//	printf("	border_width is:%d" , box->border_width);

	/*处理前景色*/
	if(!box->foreground){	/*如果没有设置前景色那么继承父之前景色*/
		parent = current->parent;
		while(parent){
			if(parent->rect.foreground){
				box->foreground = parent->rect.foreground;
				break;
			}
			parent = parent->parent;
		}
	}
	printf("	foreground is:%d\n" , box->foreground);

	/*处理背景色*/
	if(!box->background){	/*如果没有设置背景色那么继承父之背景色*/
		parent = current->parent;
		while(parent){
			if(parent->rect.background){
				box->background = parent->rect.background;
				break;
			}
			parent = parent->parent;
		}
	}
//	printf("	background is:%d" , box->background);

	/*处理边框背景色*/
	if(box->border_width){		/*必须要边框宽度不为0*/
		if(!box->border_color){	/*如果没有设置边框背景色那么继承父之背景色*/
			parent = current->parent;
			while(parent){
				if(parent->rect.border_color){
					box->border_color = parent->rect.border_color;
					break;
				}
				parent = parent->parent;
			}
		}

	}
//	printf("	border_color is:%d" , box->border_color);

	/*处理字体类型*/
	if(!box->font_family){	/*如果没有设置字体那么继承父之字体*/
		parent = current->parent;
		while(parent){
			if(parent->rect.font_family){
				box->font_family = parent->rect.font_family;
				break;
			}
			parent = parent->parent;
		}
	}

	/*处理字体大小*/
	if(!box->font_size){	/*如果没有设置字体大小那么继承父之字体大小*/
		parent = current->parent;
		while(parent){
			if(parent->rect.font_size){
				box->font_size = parent->rect.font_size;
				break;
			}
			parent = parent->parent;
		}
	}


//	printf("	font_family is:%d" , box->font_family);

	/*处理文字对齐*/
	if(!box->text_align){	/*如果没有设置对齐那么继承父之对齐*/
		parent = current->parent;
		while(parent){
			if(parent->rect.text_align){
				box->text_align = parent->rect.text_align;
				break;
			}
			parent = parent->parent;
		}
	}
//	printf("	text_align is:%d" , box->text_align);

	/*处理文字消除*/
	if(!box->clear){	/*如果没有设置消除那么继承父之消除*/
		parent = current->parent;
		while(parent){
			if(parent->rect.clear){
				box->clear = parent->rect.clear;
				break;
			}
			parent = parent->parent;
		}
	}
//	printf("	clear is:%d" , box->clear);

	/*处理边框类型*/
	if(!box->border_style){	/*如果没有设置边框类型那么继承父之样式*/
		parent = current->parent;
		while(parent){
			if(parent->rect.border_style){
				box->border_style = parent->rect.border_style;
				break;
			}
			parent = parent->parent;
		}
	}
//	printf(" border_style is:%d" , box->border_style);

	if(box->extra){
		printf("extra is existed\n");
	}
	return -1;
}



/*
 * 渲染一棵DOM树 使每个标签的盒子要素都齐备
 */
int render_dom(dom_tree *this){
	this->goto_root(this);
	this->pre_trav(this , do_render_dom , NULL , NCNG);
	return 0;
}



















