CC = gcc
CFLAGS = -fno-builtin `pkg-config --cflags gtk+-2.0`
LD = -L /usr/local/lib/ds -lds `pkg-config --libs gtk+-2.0`
LIB = get_token.o parse_css.o parse.o render.o

b:browser.o $(LIB)
	$(CC) $(CFLAGS) browser.o $(LIB) $(LD) -o b
	
browser.o:browser.c
	$(CC) $(CFLAGS) -c $<
get_token.o:get_token.c
	$(CC) $(CFLAGS) -c $<
parse_css.o:parse_css.c
	$(CC) $(CFLAGS) -c $<	
parse.o:parse.c
	$(CC) $(CFLAGS) -c $<
render.o:render.c
	$(CC) $(CFLAGS) -c $<	
	
clean:
	rm *.o
	rm b	