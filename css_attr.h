/*
 * css_attr.h
 *
 *  Created on: 2010-12-29
 *      Author: leiming
 */

#ifndef CSS_ATTR_H_
#define CSS_ATTR_H_

/*
 * CSS的预定义属性标签
 * 同时也是作为dom_node之ATTR类型的CLASS
 */
/*CLASS OF ATTR*/
#define BOTTOM				"bottom"
//距离父对象之下端


#define BG_COLOR			"background-color"
//背景色

#define BG_IMG				"background-image"
//背景图案

#define BG_REPEAT		"background-repeat"
//背景重复方式
#define BGR_NO_REPEAT		0
#define BGR_REPEAT_X			1
#define BGR_REPEAT_Y			2

#define BG_ATTACH		"background-attachment"
//设置滚动
#define BGA_FIXED					1
#define BGA_SCROLL				2

#define BG_POS				"background-position"
//初始位置
#define BGP_LEFT					1
#define BGP_RIGHT				2
#define BGP_TOP					3
#define BGP_BOTTOM			4


#define BR_TOP_W			"border-top-width"
#define BR_RIGHT_W		"border-right-width"
#define BR_BOTTOM_W	"border-bottom-width"
#define BR_LEFT_W		"border-left-width"
//边框宽度
#define BRW_MEDIUM				2
#define BRW_THIN					1
#define BRW_THICK				3

#define BR_WIDTH			"border-width"
//一次定义宽度

#define BR_COLOR			"border-color"
//设置边框颜色

#define BR_STYLE			"border-style"
//边框样式
#define BRS_NONE					1
#define BRS_SOLID					2
#define BRS_DASH					3
#define BRS_DOTTED				4


#define BR_TOP				"border-top"
#define BR_RIGHT			"border-right"
#define BR_BOTTOM		"border-bottom"
#define BR_LEFT				"border-left"
//一次定义边框


/*C#################################*/
#define COLOR				"color"
//前景色
#define BLACK	1
#define WHITE		2
#define RED		3
#define GREEN	4
#define BLUE      5
#define YELLOW	6

#define CLEAR				"clear"
//哪一边环绕
#define CLE_NONE		1
#define CLE_LEFT		2
#define CLE_RIGHT		3
#define CLE_BOTH		4

#define CURSOR				"cursor"

/*D#################################*/
#define DISPLAY				"display"
//定义是否显示
#define DIS_NONE			1
#define DIS_INLINE			2
#define DIS_BLOCK			3
#define DIS_LIST_ITEM	4

/*F################################*/
#define FLOAT					"float"
//文字环绕
#define FLO_NONE			1
#define FLO_LEFT			2
#define FLO_RIGHT			3

#define FONT_FAMILY		"font-family"
//使用字体
#define FOF_ARIAL			1
#define FOF_NCURSIVE	2
#define FOF_SERIF			3
#define FOF_SANS_SERIF		4
#define FOF_MONOSPACE		5
#define FOF_FANTASY	 		6

#define FONT_STYLE		"font-style"
//字体是否斜体
#define FOS_NORMAL		1
#define FOS_ITALIC		2
#define FOS_OBLIQUE	3

#define FONT_VARIANT	"font-variant"
//字体大小写
#define FOV_NORMAL			1
#define FOV_SMALL_CAPS	2

#define FONT_WEIGHT	"font-weight"
//字体粗细
#define FOW_NORMAL		1
#define FOW_BOLD				2
#define FOW_BOLDER			3
#define FOW_LIGHTER		4


#define FONT_SIZE			"font-size"
//字体大小

/*H#########################*/
#define HEIGHT				"height"
//高度
#define HEI_AUTO				1

/*L#########################*/
#define LEFT					"left"
//距离父对象之左端


#define LINE_HEIGHT		"line-height"
//文本行高
#define LIH_NORMAL			1

#define LETTER_SPACE	"letter-spacing"
//字母间距
#define LES_NORMAL			1

#define LIST_S				"list-style"
#define LIST_S_TYPE		"list-style-type"
#define LIST_S_IMAGE	"list-style-image"
#define LIST_S_POS		"list-style-position"


/*M########################*/
#define MARGIN				"margin"
#define MARGIN_TOP		"margin-top"
#define MARGIN_RIGHT	"margin-right"
#define MARGIN_BOTTOM	"margin-bottom"
#define MARGIN_LEFT	"margin-left"
//边距属性
#define MAR_AUTO				1

/*P#########################*/
#define PAD_TOP			"padding-top"
#define PAD_RIGHT			"padding-right"
#define PAD_BOTTOM		"padding-bottom"
#define PAD_LEFT			"padding-left"
//填充距属性

#define POSITION			"position"
//定位
#define POS_STATIC			1
#define POS_RELATIVE		2
#define POS_ABSOLUTE		3



/*R#########################*/
#define RIGHT					"right"
//距离父对象之右端




/*T#########################*/
#define TOP					"top"
//距离父对象之顶端

#define TEXT_DEC			"text-decoration"
//文本装饰
#define TED_NONE					1
#define TED_UNDERLINE		2
#define TED_OVERLINE			3
#define TED_LINE_THROUGH	4
#define TED_BLINK					5


#define TEXT_TRANS		"text-transform"
//文本转换方式
#define TET_NONE				1
#define TET_UPPERCASE	2
#define TET_LOWERCASE	3
#define TET_CAPITALIZE	4

#define TEXT_ALIGN		"text-align"
//文本对齐方式
#define TEA_LEFT				1
#define TEA_CENTER			2
#define TEA_RIGHT				3
#define TEA_JUSTIFY			4

#define TEXT_INDENT		"text-indent"
//首行缩进方式

/*V##########################*/
#define VER_ALIGN			"vertical-align"
//垂直方向位置

/*W##########################*/
#define WIDTH					"width"
//长度
#define WID_AUTO				1

#define WHITE_SPACE	"white-space"
//处理空白
#define WHS_NORMAL		1
#define WHS_PRE				2
#define WHS_NOWRAP		3

#define WORD_SPACE	"word-spacing"
//单词间距
#define WOS_NORMAL		1



#endif /* CSS_ATTR_H_ */
