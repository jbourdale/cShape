#include <errno.h>
#include "display.h"


#define CSHAPE_WINDOW_WIDTH 800
#define CSHAPE_WINDOW_HEIGHT 600
#define CSHAPE_WINDOW_TITLE "cShape"

//Structure définissant une figure
struct cshape_shape{
    int nb_lines;
    cshape_line[10] lines;
};

//Structure définissant une ligne
struct cshape_line{
    int x1;
    int y1;
    int x2;
    int y2;
};

static void errno_exit (const char *s)
{
  fprintf (stderr, "%s error %d, %s\n",s, errno, strerror (errno));
  exit (EXIT_FAILURE);
}

_Bool cShape_initDisplay(int width, int height, const char *title){
    return displayInit(title,width,height) == 0;
}

void cShape_mainLoop(){

    _Bool quit = false;
    SDL_Event e;
    int mouse_x,mouse_y, nb_figures;
    int line[2][2] = {
                        {-1,-1},
                        {-1,-1}
                     };

    cshape_shape[100] figures;


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

                //Mouse Event
                case SDL_MOUSEBUTTONDOWN:
                    SDL_GetMouseState(&mouse_x, &mouse_y);
                    printf("MouseEvent at (%d,%d)\n", mouse_x, mouse_y);

                    if (line[0][0] == -1){
                        line[0][0] = mouse_x;
                        line[0][1] = mouse_y;
                    }else{
                        line[1][0] = mouse_x;
                        line[1][1] = mouse_y;
                        displayDrawLine(line[0][0], line[0][1], line[1][0], line[1][1], 127, 255, 0, 0);
                        line[0][0] = -1;
                        line[0][1] = -1;
                    }

                    break;
            }
        }

        //Render actual scene to don't let SDL erase our scene
        displayPersistentScreen();
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
