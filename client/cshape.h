#ifndef __CSHAPE_H__
#define __CSHAPE_H__

#define CSHAPE_WINDOW_WIDTH             800
#define CSHAPE_WINDOW_HEIGHT            600
#define CSHAPE_WINDOW_TITLE             "cShape"
#define CSHAPE_NBLINESMAX_PER_SHAPE     100
#define CSHAPE_NBFIGMAX                 100

//Point
typedef struct cShape_plot{
    int x;
    int y;
}cShape_plot;
//Structure définissant une ligne
typedef struct cShape_line{
    cShape_plot plot_first;
    cShape_plot plot_last;
} cShape_line;
//Structure définissant une figure
typedef struct cShape_shape{
    int nb_lines;
    cShape_line lines[CSHAPE_NBLINESMAX_PER_SHAPE]; //Nb de ligne max par figures
}cShape_shape;


#endif
