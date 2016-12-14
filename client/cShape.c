#include <errno.h>
#include <stdio.h>
#include <math.h>
#include "cshape.h"
#include "display.h"


#include <SDL2/SDL.h>

static void errno_exit (const char *s)
{
  fprintf (stderr, "%s error %d, %s\n",s, errno, strerror (errno));
  exit (EXIT_FAILURE);
}

_Bool cShape_initDisplay(int width, int height, const char *title){
    return displayInit(title,width,height) == 0;
}

void cShape_initShape(cShape_shape* shape){
    shape->nb_lines = 0;

    //On parcourt toutes les lignes possibles pour les inits correctement
    for(int i = 0; i<CSHAPE_NBLINESMAX_PER_SHAPE; i++){
        shape->lines[i].plot_first.x = -1;
        shape->lines[i].plot_first.y = -1;
        shape->lines[i].plot_last.x = -1;
        shape->lines[i].plot_last.y = -1;
    }
}

/**
 * Detecting if the last point of the shape is near the first one
 * to collapse them
 */
void cShape_rounding_last_lines(cShape_shape* shape){

    cShape_plot first_plot= shape->lines[0].plot_first;
    cShape_plot last_plot= shape->lines[shape->nb_lines-1].plot_last;


    // sqrt( (x2-x1) + (y2-y1))
    double dist = sqrt( pow((last_plot.x-first_plot.x),2)+ pow((last_plot.y-first_plot.y),2));
    printf("DIST : %lf\n", dist);
    if (dist <= CSHAPE_ROUNDING_DISTANCE){
        printf("Assez prof pour les collapse");
        shape->lines[shape->nb_lines-1].plot_last.x = first_plot.x;
        shape->lines[shape->nb_lines-1].plot_last.y = first_plot.y;
    }

}

void cShape_submit_shape(cShape_shape* shape, cShape_shape figs[], int* figs_size){

    printf("\n\n\ncS_s_s\n");


    printf("   nbLigne entrant : %d\n", shape->nb_lines);

    if(shape->nb_lines > 0){

        cShape_rounding_last_lines(shape);

        cShape_shape new_shape;
        cShape_initShape(&new_shape);

        //Copying shape into new_shape
        new_shape.nb_lines = shape->nb_lines;
        for(int i = 0; i < shape->nb_lines; i++){
            new_shape.lines[i].plot_first.x = shape->lines[i].plot_first.x;
            new_shape.lines[i].plot_first.y = shape->lines[i].plot_first.y;

            new_shape.lines[i].plot_last.x = shape->lines[i].plot_last.x;
            new_shape.lines[i].plot_last.y = shape->lines[i].plot_last.y;
        }


        //Adding it to the main list of shapes
        figs[*figs_size] = new_shape;
        *figs_size = *figs_size+1;

        cShape_initShape(shape);
    }
    //exit(0);
}

void cShape_mainLoop(){

    _Bool quit = false;
    SDL_Event e;
    int mouse_x,mouse_y, nb_figures = 0;
    Uint8* keys;
    cShape_shape figures[CSHAPE_NBFIGMAX]; //Tableau de figures

    cShape_shape figActuel; //Figure qu'on est entrain de construire
    cShape_initShape(&figActuel);


    //Init SDL display
    if(!cShape_initDisplay(CSHAPE_WINDOW_WIDTH,CSHAPE_WINDOW_HEIGHT, CSHAPE_WINDOW_TITLE)){
        errno_exit("Error init display");
    }

    //cShape loop
    while(!quit){

        //SDL Event loop
        while(SDL_PollEvent(&e)){
            switch(e.type){
                //Quit event
                case SDL_QUIT:
                    quit = true;
                    break;

                case SDL_KEYDOWN:
                    //printf("\n\nKEYDOWN : %d\n\n", e.key.keysym.sym);
                    if ( e.key.keysym.sym == 13 ) { //Si ENTREE est PRESSED
                        //Finition de la figure
                        //printf("ENTER DOWN\nnbfig = %d\n", nb_figures);
                        cShape_submit_shape(&figActuel,figures, &nb_figures);
                        //printf("   nbLigne sortant : %d\n", figures[nb_figures-1].nb_lines);
                    }
                    break;

                case SDL_MOUSEMOTION:
                    //Si la souris bouge, la ligne qu'on est entrain de tracer suivra
                    //printf("Bonjour\n");
                    SDL_GetMouseState(&mouse_x, &mouse_y);
                    if(!(figActuel.nb_lines == 0 && figActuel.lines[figActuel.nb_lines].plot_first.x == -1)){
                        figActuel.lines[figActuel.nb_lines].plot_last.x = mouse_x;
                        figActuel.lines[figActuel.nb_lines].plot_last.y = mouse_y;
                    }
                    break;
                //Mouse Event
                case SDL_MOUSEBUTTONDOWN:
                    SDL_GetMouseState(&mouse_x, &mouse_y);
                    printf("MouseEvent at (%d,%d)\n", mouse_x, mouse_y);

                    //Si c'est le tout premier point de la figure
                    if(figActuel.nb_lines == 0 && figActuel.lines[figActuel.nb_lines].plot_first.x == -1){
                        //On set le premier point
                        figActuel.lines[figActuel.nb_lines].plot_first.x = mouse_x;
                        figActuel.lines[figActuel.nb_lines].plot_first.y = mouse_y;

                        //Et on mets le dernier de la ligne au niveau du curseur
                        //car il a été initialisé a -1;-1
                        figActuel.lines[figActuel.nb_lines].plot_last.x = mouse_x;
                        figActuel.lines[figActuel.nb_lines].plot_last.y = mouse_y;

                    }else{
                        //Si c'est n'importe quel autre point
                        //On finit la ligne actuelle
                        figActuel.lines[figActuel.nb_lines].plot_last.x = mouse_x;
                        figActuel.lines[figActuel.nb_lines].plot_last.y = mouse_y;
                        //Et on passe a la suivante en initialisant son premier point
                        //a la suite de la ligne que l'on vient de finir
                        figActuel.nb_lines++;
                        figActuel.lines[figActuel.nb_lines].plot_first.x = mouse_x;
                        figActuel.lines[figActuel.nb_lines].plot_first.y = mouse_y;

                        figActuel.lines[figActuel.nb_lines].plot_last.x = mouse_x;
                        figActuel.lines[figActuel.nb_lines].plot_last.y = mouse_y;

                    }
                    break;
            }
        }

        delay(20);
        //Render scene
        cShape_render(figures, nb_figures, figActuel);
    }

}



int main(int argc, char* args[]){
  delay(1000);
  cShape_mainLoop();

  /*
  if (displayInit("test",800,600) == 0) {

    //fond blanc
    displayDrawRect(0, 0, 800, 600, 255, 255, 255, 255, true);

    while (! quit()) {
      delay(20);
      //mouseEvent();
      //Render actual scene to don't let SDL erase our scene
      displayPersistentScreen();
    }
    displayQuit();
    return 0;
  }
  return 1;
  */
}
