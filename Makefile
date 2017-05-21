# To allow declaration in for loop, debug and display all warning
CCflags=-std=gnu99 -g -Wall

#SDL for cshape_client
SDLflags=-lSDL2 -lSDL2_ttf

#Math.h
Mflags=-lm

#--------------------------------------------------

all : cShape_client cShape_serveur

#--------------------------------------------------

cShape_client : client/display.c client/cShape.c
	gcc $^ -o $@ ${SDLflags} ${Mflags} ${CCflags}


cShape_serveur : server/serveur.c
	gcc  $^ -o $@ ${CFLAGS}

#--------------------------------------------------

clean :
	rm cShape_serveur cShape_client
