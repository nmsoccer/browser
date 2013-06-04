/*
 * test.c
 *
 *  Created on: 2010-12-8
 *      Author: leiming
 */


#include <stdio.h>
#include <string.h>

#include "dom_node.h"
#include "parse_css.h"
#include "parse.h"
/*
int visit_data(void *data){
	dom_node *pn = (dom_node *)data;
	printf("type is:%d  " , pn->type);
	printf("node is:%s" , pn->value);
	if(pn->parent){
		printf("parent is:%s" , pn->parent->value);
	}
	if(pn->brother){
		printf("brother is:%s" , pn->brother->value);
	}

	printf("\n");

	return -1;
}


int visit_style(void *data , void *arg){
	style_name *s_name = (style_name *)data;

	printf("style-> name:%s\n" , s_name->name);
	printf("value:%s\n" , s_name->s_value->value);

	return -1;
}
*/
/*
		label_tb->set_line(label_tb , ('b' - 'a')%('a'));
		printf("label_tb b size is:%d\n" ,  label_tb->get_nr_node(label_tb));
		label_tb->foreach(label_tb , visit_style , NULL);

		id_tb->set_line(id_tb , ('c' - 'a')%('a'));
		printf("id_tb c size is:%d\n" , id_tb->get_nr_node(id_tb));
		id_tb->foreach(id_tb , visit_style , NULL);

		id_tb->set_line(id_tb , ('t' - 'a')%('a'));
		printf("id_tb t size is:%d\n" , id_tb->get_nr_node(id_tb));
		id_tb->foreach(id_tb , visit_style , NULL);
*/

/*
	dom->goto_root(dom);
	printf("size of dom is:%d\n" , dom->get_size(dom));
	dom->pre_trav(dom , visit_data , NCNG);
*/


int main(int argc , char **argv){
	dom_tree *dom;

	dom = parse("better.html");
	if(!dom){
		printf("dom is empty!\n");
		return -1;
	}

	scan_dom(dom);

	render_dom(dom);

	if(use_css){
		printf("using css~@~\n");

		destruct_css_tb(id_tb);
		destruct_css_tb(label_tb);
		destruct_css_tb(class_tb);
	}

	destruct(dom);

	return 0;
}
