/*
 * render.h
 *
 *  Created on: 2011-1-16
 *      Author: leiming
 */

#ifndef RENDER_H_
#define RENDER_H_

#include "parse.h"

#define TEXT_CONTENT_LEN 100

extern char text_content[TEXT_CONTENT_LEN];
extern int screen_width;
extern int screen_height;

/*
 * 渲染一棵DOM树 使每个标签的盒子要素都齐备
 */
int render_dom(dom_tree *dom);


#endif /* RENDER_H_ */
