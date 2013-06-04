/*
 * browser.c
 *
 *  Created on: 2011-1-20
 *      Author: leiming
 */
#include <cairo.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "html_label.h"
#include "dom_node.h"
#include "parse_css.h"
#include "parse.h"
#include "css_attr.h"
#include "render.h"

dom_tree *dom;	/*装满HTML结点的二叉树*/

GtkWidget *top_win;	/*顶层窗口*/

cairo_t *cr;		/*画笔*/

struct _text_location{	/*文字的位置*/
	unsigned char set;
	short x;
	short y;
}text_location;


dom_node *current;

static int do_draw_rect(void *data , void *arg){
	dom_node *pn = (dom_node *)data;

	if(pn->type == ATTR | pn->type == ROOT){
		return -1;
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

	if(pn->type == TEXT){	/*书写文字*/
		if(!current){
			return -1;
		}
		if(!text_location.set){	/*如果没有设置文字*/
			text_location.set = 1;

			printf("color is;%d\n" , current->rect.foreground);
			switch(current->rect.foreground){
			case BLACK:
				cairo_set_source_rgba(cr , 0 , 0 , 0 , 0.6);
				break;
			case WHITE:
				cairo_set_source_rgba(cr , 1 , 1 , 1 , 1);
				break;
			case RED:
				cairo_set_source_rgba(cr , 1 , 0 , 0 , 0.5);
				break;
			case BLUE:
				cairo_set_source_rgba(cr , 0 , 0 , 1 , 0.6);
				break;
			case GREEN:
				cairo_set_source_rgba(cr , 0 , 1 , 0 , 0.5);
				break;
			case YELLOW:
				cairo_set_source_rgba(cr , 0.25, 0.25, 0.5, 0.5);
				break;
			default:	/*默认黑色*/
				cairo_set_source_rgba(cr , 0 , 0 , 0 , 0.6);
				break;
			}

			switch(current->rect.text_align){	/*当前标签的文字对齐方式*/
			case TEA_CENTER:
				text_location.x = current->rect.x + current->rect.width / 2;
				text_location.y = current->rect.y + (current->rect.height - current->rect.font_size) / 2;
				break;
			case TEA_RIGHT:
				break;
			case TEA_LEFT:
			default:
				text_location.x = current->rect.x + 1;
				text_location.y = current->rect.y + (current->rect.height - current->rect.font_size) / 2;
				break;
			}
			cairo_move_to(cr , text_location.x , text_location.y);
		}

		cairo_show_text(cr , pn->value);
		return -1;
	}
	/*ELEMENT*/
	/*可绘制标签*/
	current = pn;
	text_location.set = 0;
	printf("~~~~ready to draw %s\n" , pn->value);
	printf("font_size is:%d\n" , pn->rect.font_size);
	cairo_set_font_size(cr , pn->rect.font_size);
//	printf("border_color is;%d\n" , pn->rect.border_color);	/*绘制矩形用边框颜色绘文字用前景色*/
	switch(pn->rect.border_color){
	case BLACK:
		cairo_set_source_rgba(cr , 0 , 0 , 0 , 0.6);
		break;
	case WHITE:
		cairo_set_source_rgba(cr , 1 , 1 , 1 , 1);
		break;
	case RED:
		cairo_set_source_rgba(cr , 1 , 0 , 0 , 0.5);
		break;
	case BLUE:
		cairo_set_source_rgba(cr , 0 , 0 , 1 , 0.6);
		break;
	case GREEN:
		cairo_set_source_rgba(cr , 0 , 1 , 0 , 0.5);
		break;
	case YELLOW:
		cairo_set_source_rgba(cr , 0.25, 0.25, 0.5, 0.5);
		break;
	}

	cairo_rectangle(cr , pn->rect.x , pn->rect.y , pn->rect.width , pn->rect.height);	/*绘制对应矩形*/
	printf("border_width is;%d\n" , pn->rect.border_width);
    cairo_set_line_width (cr, pn->rect.border_width);/*边框宽度用设定宽度*/
    cairo_set_line_join (cr, CAIRO_LINE_JOIN_BEVEL);
	cairo_stroke_preserve(cr);

	printf("back is;%d\n" , pn->rect.background);
	switch(pn->rect.background){	/*用背景色填充背景*/
	case BLACK:
		cairo_set_source_rgba(cr , 0 , 0 , 0 , 0.6);
		break;
	case WHITE:
		cairo_set_source_rgba(cr , 1 , 1 , 1 , 1);
		break;
	case RED:
		cairo_set_source_rgba(cr , 1 , 0 , 0 , 0.5);
		break;
	case BLUE:
		cairo_set_source_rgba(cr , 0 , 0 , 1 , 0.6);
		break;
	case GREEN:
		cairo_set_source_rgba(cr , 0 , 1 , 0 , 0.5);
		break;
	case YELLOW:
		cairo_set_source_rgba(cr , 0.25, 0.25, 0.5, 0.5);
		break;
	}
	cairo_fill(cr);	/*填充*/

	return -1;
}


static gboolean on_expose_event(GtkWidget *widget , GdkEventExpose *event , gpointer data){
	cr = gdk_cairo_create(widget->window);

	cairo_select_font_face(cr , "Adobe Heiti Std" , CAIRO_FONT_SLANT_NORMAL , CAIRO_FONT_WEIGHT_NORMAL);
//	cairo_get_font_size();
/*
	cairo_t *cr;

	cairo_pattern_t *pt;

	cr = gdk_cairo_create(widget->window);

	cairo_set_source_rgb(cr , 0.627 , 0 , 0);
	cairo_select_font_face(cr , "Adobe Heiti Std" , CAIRO_FONT_SLANT_NORMAL , CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr , 24.0);

	cairo_move_to(cr , 10.0 , 34.0);
	cairo_show_text(cr , "leiming");
	cairo_move_to(cr , 50 , 50);
	cairo_line_to(cr , 100 , 100);
	cairo_stroke(cr);


	cairo_set_source_rgb(cr , 0.69 , 0.19 , 0);
	cairo_arc(cr , 100 , 100 , 20 , 0 , 2 * M_PI);
	cairo_stroke_preserve(cr);

	cairo_set_source_rgb(cr , 0.3 , 0.4 , 0.6);
	cairo_fill(cr);



    cairo_set_source_rgba (cr, 0, 0, 0, 1);
    cairo_set_line_width (cr, 10);


    cairo_set_line_cap (cr, CAIRO_LINE_CAP_BUTT);
    cairo_move_to (cr, 40, 60);
    cairo_line_to (cr, 360, 60);
    cairo_stroke (cr);

    cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
    cairo_move_to (cr, 40, 150);
    cairo_line_to (cr, 360, 150);
    cairo_stroke (cr);

    cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
    cairo_move_to (cr, 40, 240);
    cairo_line_to (cr, 360, 240);
    cairo_stroke (cr);

    cairo_rectangle (cr, 30, 30, 100, 100);
    cairo_set_line_width (cr, 14);
    cairo_set_line_join (cr, CAIRO_LINE_JOIN_MITER);
    cairo_stroke (cr);

    cairo_rectangle (cr, 160, 30, 100, 100);
    cairo_set_line_width (cr, 14);
    cairo_set_line_join (cr, CAIRO_LINE_JOIN_BEVEL);
    cairo_stroke (cr);

    cairo_rectangle (cr, 100, 160, 100, 100);
    cairo_set_line_width (cr, 14);
    cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
    cairo_stroke (cr);


    cairo_set_source_rgb(cr , 0 , 0 , 0);
    cairo_set_line_width(cr , 1);

    cairo_rectangle(cr , 20 , 20 , 120 , 80);
    cairo_stroke_preserve(cr);
    cairo_set_source_rgb(cr , 1 , 1 , 1);
    cairo_fill(cr);


    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_arc (cr, 90, 160, 40, M_PI / 4, M_PI);
    cairo_close_path (cr);
    cairo_stroke_preserve (cr);
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_fill (cr);



    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_translate (cr, 220, 180);
    cairo_scale (cr, 1, 0.7);
    cairo_arc (cr, 0, 0, 50, 0, 2 * M_PI);
    cairo_stroke_preserve (cr);
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_fill (cr);
*/

/*
	pt = cairo_pattern_create_for_surface(surface);
	cairo_set_source(cr , pt);
	cairo_pattern_set_extend(cairo_get_source(cr) , CAIRO_EXTEND_REPEAT);
	cairo_rectangle(cr , 20 , 20 , 100 , 100);
	cairo_fill(cr);

    gint i;
    for (i = 1; i <= 10; i++) {
            cairo_set_source_rgba (cr, 0, 0, 1, i * 0.1);
            cairo_rectangle (cr, 50 * i, 20, 40, 40);
            cairo_fill (cr);
    }

//	cairo_pattern_destroy(pt);
	cairo_destroy(cr);
*/

	dom->goto_root(dom);
	dom->pre_trav(dom , do_draw_rect , NULL , NCNG);

	return FALSE;
}

static gboolean close_browser(GtkWidget *widget , GdkEventExpose *event , gpointer data){
	printf("good bye!\n");
	if(use_css){
		printf("using css~@~\n");

		destruct_css_tb(id_tb);
		destruct_css_tb(label_tb);
		destruct_css_tb(class_tb);
	}
	destruct(dom);
	cairo_destroy(cr);
	gtk_main_quit();
	gtk_exit(0);
}


int main(int argc , char **argv){
	if(argc < 2){
		printf("Please input the name of html file!\n");
		return -1;
	}

	gtk_init(&argc , &argv);

	top_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	g_signal_connect(top_win , "expose-event" , G_CALLBACK(on_expose_event) , NULL);
	g_signal_connect(top_win , "destroy" , G_CALLBACK(close_browser) , NULL);

	gtk_window_set_position(GTK_WINDOW(top_win) , GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(top_win) , screen_width , screen_height);
	gtk_widget_set_app_paintable(top_win , TRUE);

	text_location.set = 0;	/*没有设置位置*/
	dom = parse(argv[1]);
	if(!dom){
		printf("dom is empty!\n");
		return -1;
	}
	scan_dom(dom);
	render_dom(dom);




	gtk_widget_show_all(top_win);
	gtk_main();

	return 0;
}
