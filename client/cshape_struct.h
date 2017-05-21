/**
  Fichier header pour les structures ou données communes à toute l'application,
  import etc.
**/

#ifndef __CSHAPE_STRUCT_H__
#define __CSHAPE_STRUCT_H__

/* SDL */
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/* NETWORK */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* SELECT */
#include <sys/types.h>
#include <sys/wait.h>

/* MISCELLANEOUS */
#include <errno.h> //Errno exit
#include <stdio.h> //Basic import
#include <math.h> //cShape_rounding_last_lines
#include <unistd.h>
#include <stdbool.h> //_Bool

#define CSHAPE_WINDOW_WIDTH             800
#define CSHAPE_WINDOW_HEIGHT            600
#define CSHAPE_WINDOW_TITLE             "cShape - draw online with friends - DUT Informatique 2A "
#define CSHAPE_NBLINESMAX_PER_SHAPE     90  //90 parce que 9 + 90*16 + 8 = 1457 < 1480 octets (trame UDP), voir README#Protocole
#define CSHAPE_NBFIGMAX                 100
#define CSHAPE_ROUNDING_DISTANCE        10
#define CSHAPE_POPUP_COLOR              1
#define CSHAPE_POPUP_IMAGE              2
#define CSHAPE_POPUP_REFRESH            3
#define CSHAPE_COLOR_DEFAULT            {255,0,0,255}

/**
 * Structure définissant les popups de menu qui apparaissent dans l'application
 */
typedef struct cShape_popup{
    int type;
    SDL_Color color;
    char* title;
    char* imagePath;
    char* information;
}cShape_popup;
/**
 * Structure définissant un point dans cShape
 */
typedef struct cShape_plot{
    int x;
    int y;
}cShape_plot;
/**
 * Structure définissant une ligne dans cShape
 */
typedef struct cShape_line{
    cShape_plot plot_first;
    cShape_plot plot_last;
} cShape_line;
/**
 * Structure définissant un polygone dans cShape
 */
typedef struct cShape_shape{
    int nb_lines;
    cShape_line lines[CSHAPE_NBLINESMAX_PER_SHAPE]; //Nb de ligne max par figures
    SDL_Color color;
}cShape_shape;
/**
 * Structure définissant une image dans cShape. Non implementé pour l'instant
 */
typedef struct cShape_img{
    int x;
    int y;
    int w;
    int h;

    //Rajouter un tableau de pixels
}cShape_img;

/**
 * Structure définissant un dessin (figure ou img) dans cShape. Non implementé
 */
typedef struct cShape_draw{
    int type; //1 si c'est une shape, 2 si c'est une image
    int id; //id qui permet de retrouver le dessin correspondant dans le tableau soit d'image, soit de shape
}cShape_draw;

#endif
