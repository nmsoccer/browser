/*
 * element.h
 *
 *  Created on: 2011-1-3
 *      Author: leiming
 */

#ifndef ELEMENT_H_
#define ELEMENT_H_
/*
 * 实际上每个html元素对应的都是一个矩形
 * 绘制矩形之要素:x , y , width , height , foreground-color , background-color , img即可
 *	而在下面其他的元素很大程度上都是用于计算以上元素的。尤其是起点x , y
 *	其内部成员的值严重依赖于css属性值
 */

/*
 * CSS元素对于任一个HTML元素来说不是必须的，它可能继承于父元素
 */
typedef struct _extra_element{
	char bg_image[16];
	unsigned char bg_repeat;
	unsigned char bg_attach;
	unsigned char bg_position;
	unsigned short border_width[4];	/*top right bottom left*/


	unsigned char display;

	unsigned char font_style;
	unsigned char font_variant;
	unsigned char font_weight;

	unsigned char line_height;
	unsigned char letter_spacing;
	unsigned char list_style;
	unsigned char list_style_type;
	char list_style_image[16];
	unsigned char list_style_position;

	/*padding top\right\down\left*/
	short padding[4];

	unsigned char text_decoration;
	unsigned char text_transform;
	unsigned char text_indent;

	unsigned char vertical_align;

	unsigned short white_spacing;
	unsigned short word_spacing;


}extra_element;


typedef struct _element{
	unsigned char disable;	/*表明该标签是否作废*/

	short x;	/*起点位置。计算该位置是解析dom树的重点所在*/
	short y;

	short width;	/*长宽 若为正则是像素 若为负则是百分比*/
	short height;
	short border_width;/*边框宽*/

	unsigned long foreground;	/*前景与后景色*/
	unsigned long background;
	unsigned long border_color;	/*边框颜色*/

	/*CSS 常用属性*/
	unsigned	char position;	/*确定是相对位置还是绝对位置*/
	unsigned char font_family;
	unsigned char font_size;
	unsigned char text_align;	/*文本对齐方式*/
	unsigned char floating;	/*文字环绕*/
	unsigned char clear;	/*是否允许浮动存在*/
	unsigned char border_style;	/*边框样式*/

	short margin[4];	/*margin top\right\down\left 如果是正数表示绝对值 负数是百分比*/

	/*CSS 额外属性*/
	extra_element *extra;
}element;


#endif /* ELEMENT_H_ */
