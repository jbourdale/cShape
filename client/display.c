#include "display.h"
#include "cshape.h"
#include <stdio.h>
#include <unistd.h>

static SDL_Window *sdlWindow = NULL;
static SDL_Renderer *sdlRenderer = NULL;
static SDL_Texture *sdlScreenTexture = NULL;
static SDL_Texture *sdlTexture = NULL;

/**
 * Opens a window with a title and a size.
 * @param caption the title of the window
 * @param width the width of the window
 * @param height the height of the window
 * @return 0 if ok, 1 if error
 */
int displayInit(const char* caption, int width, int height) {
  // First init the SDL system
  if (SDL_Init(SDL_INIT_VIDEO) != 0){
    printf("SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }

  TTF_Init();
  // Second creates the window
  sdlWindow = SDL_CreateWindow(caption,
			       SDL_WINDOWPOS_UNDEFINED,
			       SDL_WINDOWPOS_UNDEFINED,
			       width, height,
			       SDL_WINDOW_OPENGL);
  if (sdlWindow == NULL) {
    printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
    return 1;
  }

  // Third creates the renderer for this window
  sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
  if (sdlRenderer == NULL) {
    printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
    return 1;
  }
  sdlScreenTexture = SDL_CreateTexture(sdlRenderer,
				 SDL_PIXELFORMAT_ARGB8888,
				 SDL_TEXTUREACCESS_TARGET,
				 width, height);
  if (sdlScreenTexture == NULL) {
    printf("SDL_CreateTexture failed : %s\n",SDL_GetError());
    return -1;
  }

  SDL_SetRenderDrawBlendMode(sdlRenderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderTarget(sdlRenderer, sdlScreenTexture);
  SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 255, 255);
  SDL_RenderClear(sdlRenderer);
  SDL_SetRenderTarget(sdlRenderer, NULL);
  SDL_RenderClear(sdlRenderer);
  SDL_RenderPresent(sdlRenderer);
  return 0;
} // displayInit(caption, width, height)


void cShape_render(cShape_shape figs[], int figs_size, cShape_shape figActuel, cShape_popup popup){

    //Clear de la scene
    SDL_RenderClear(sdlRenderer);
    displayDrawRect(0, 0,CSHAPE_WINDOW_WIDTH,CSHAPE_WINDOW_HEIGHT, 255, 255, 255, 255, true);

    //Affichage de toutes les figures
    for(int i=0; i<figs_size; i++){
        //printf("fig[%d], nb line = %d\n", i, figs[i].nb_lines);
        for(int j=0; j<figs[i].nb_lines;j++){
            int x1 = figs[i].lines[j].plot_first.x;
            int y1 = figs[i].lines[j].plot_first.y;
            int x2 = figs[i].lines[j].plot_last.x;
            int y2 = figs[i].lines[j].plot_last.y;
            //printf("Render line : (%d,%d);(%d,%d)\n", x1,y1,x2,y2);
            //printf("Color : {%d,%d,%d,%d}",figs[i].color.a,figs[i].color.r,figs[i].color.g,figs[i].color.b);
            displayDrawLine(x1,y1,x2,y2,figs[i].color.a,figs[i].color.r,figs[i].color.g,figs[i].color.b);
        }
    }

    //Affichage de la figure en cours d'édition
    if (!(figActuel.nb_lines == 0 && figActuel.lines[figActuel.nb_lines].plot_first.x == -1)){
        for(int i=0; i<=figActuel.nb_lines; i++){
            int x1 = figActuel.lines[i].plot_first.x;
            int y1 = figActuel.lines[i].plot_first.y;
            int x2 = figActuel.lines[i].plot_last.x;
            int y2 = figActuel.lines[i].plot_last.y;
            //printf("Color : {%d,%d,%d,%d}",figActuel.color.a,figActuel.color.r,figActuel.color.g,figActuel.color.b);
            //printf("Render line : (%d,%d);(%d,%d)\n", x1,y1,x2,y2);
            displayDrawLine(x1,y1,x2,y2,figActuel.color.a,figActuel.color.r,figActuel.color.g,figActuel.color.b);
        }
    }

    //Affichage de la popup si il y en a une
    cShape_display_popup_frame(popup);


    //Rendering the "hard" part of the screen
    SDL_RenderCopy(sdlRenderer, sdlScreenTexture, NULL, NULL);


    //Rendering the text
    cShape_display_popup_text(popup);



    SDL_RenderPresent(sdlRenderer);
}

/** Displays the persistent screen on-screen.
 */
void displayPersistentScreen() {
  SDL_RenderClear(sdlRenderer);
  SDL_RenderCopy(sdlRenderer, sdlScreenTexture, NULL, NULL);
  SDL_RenderPresent(sdlRenderer);
}

/** Checks for SDL events (non-blocking)
 * @return true iff the application should quit
 */
_Bool checkEvents() {
  SDL_Event e;
  _Bool quit = false;
  while (SDL_PollEvent(&e)){
    switch (e.type) {
    case SDL_QUIT:
    case SDL_KEYDOWN:
    case SDL_MOUSEBUTTONDOWN:
      quit = true;
    }
  } // while more events
  return quit;
} // checkEvents()

/**
 * closes the window and terminates the SDL system
 */
void displayQuit() {
  if (sdlScreenTexture != NULL) {
    SDL_DestroyTexture(sdlScreenTexture);
    sdlScreenTexture = NULL;
  }
  if (sdlTexture != NULL) {
    SDL_DestroyTexture(sdlTexture);
    sdlTexture = NULL;
  }
  if (sdlRenderer != NULL) {
    SDL_DestroyRenderer(sdlRenderer);
    sdlRenderer = NULL;
  }
  if (sdlWindow != NULL) {
    SDL_DestroyWindow(sdlWindow);
    sdlWindow = NULL;
  }
  SDL_Quit();
} //displayQuit()

/** Waits for the specified time.
 * @param milliseconds the time to wait. 1000 is 1 second.
 */
void delay(int milliseconds) {
  SDL_Delay(milliseconds);
} // delay(int)

/** Displays the specified pixels at the specified location of the window.
 * @param pixels a linear array of ARGB pixels (2x2 pixels => B00,G00,R00,A00,B01,G01,R01,B10,G10,R10,A10,B11,G11,R11)
 * @param x the left-coordinate of the top-left corner of the rectangle to fill with pixels
 * @param y the top-coordinate of the top-left corner of the rectangle to fill with pixels
 * @param with the width of the rectangle to fill with pixels
 * @param height the height of the rectangle to fill with pixels
 * @param scale the scale to apply to the pixels during drawing. 2 is twice larger, 0.25 is quarter.
 */
void displayPixels(void *pixels, int x, int y, int width, int height, float scale, _Bool persistent) {
  SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(pixels,width,height,32,4*width,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000); // standard masks for ARGB
  if (surf == NULL)
    printf("SDL_CreateSurfaceFrom failed : %s\n",SDL_GetError());
  else {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(sdlRenderer, surf);
    if (texture == NULL)
      printf("SDL_CreateTextureFromSurface failed : %s\n",SDL_GetError());
    else {
      SDL_Rect dest = {x,y,(int)(width*scale+0.5),(int)(height*scale+0.5)}; // Round pixels to the nearest
      if (persistent) {
        if (SDL_SetRenderTarget(sdlRenderer, sdlScreenTexture) != 0)
            printf("SDL_SetRenderTarget failed : %s\n", SDL_GetError());
        } else {
            SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
            SDL_RenderClear(sdlRenderer);
            SDL_RenderCopy(sdlRenderer, sdlScreenTexture, NULL, NULL);
        }
        if (SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND) != 0)
            printf("SDL_SetTextureBlendMode failed : %s\n",SDL_GetError());
        if (SDL_RenderCopy(sdlRenderer, texture, NULL, &dest) != 0)
            printf("SDL_RenderCopy failed : %s\n",SDL_GetError());
        else {
            if (persistent) {
                if (SDL_SetRenderTarget(sdlRenderer, NULL) != 0)
                    printf("SDL_SetRenderTarget failed : %s\n", SDL_GetError());
                SDL_RenderCopy(sdlRenderer, sdlScreenTexture, NULL, NULL);
            }
            SDL_RenderPresent(sdlRenderer);
        }
        SDL_DestroyTexture(texture);
    } // if surf created

    SDL_FreeSurface(surf);
  } // if surf created
} // displayPixels(void*, int, int, int, int)

/** Prepares a texture of the requested size.
 * @param width the texture width
 * @param height the texture height
 * @return 0 if Ok ; -1 if error
 */
int displayPrepareTexture(int width, int height) {
  sdlTexture = SDL_CreateTexture(sdlRenderer,
				 SDL_PIXELFORMAT_ARGB8888,
				 SDL_TEXTUREACCESS_STREAMING,
				 width, height);
  if (sdlTexture == NULL) {
    printf("SDL_CreateTexture failed : %s\n",SDL_GetError());
    return -1;
  }
  if (SDL_SetTextureBlendMode(sdlTexture, SDL_BLENDMODE_BLEND) != 0)
    printf("SDL_SetTextureBlendMode failed : %s\n",SDL_GetError());
  return 0;
}

/** Updates the texture with specified pixels and put it on the window.
 * @param pixels [in] the pixels to put into the texture
 * @param x the left-coordinate of the top-left corner of the window rectangle to fill with pixels
 * @param y the top-coordinate of the top-left corner of the window rectangle to fill with pixels
 * @param width the width of the texture
 * @param height the height of the texture
 * @param scale the scale to apply to the pixels during drawing. 2 is twice larger, 0.25 is quarter.
 */
void displayUpdateAndDisplayTexture(void *pixels, int x, int y, int width, int height, float scale, _Bool persistent) {
  SDL_UpdateTexture(sdlTexture, NULL, pixels, width * sizeof (Uint32));
  if (persistent) {
    if (SDL_SetRenderTarget(sdlRenderer, sdlScreenTexture)!=0)
      printf("SDL_SetRenderTarget failed : %s\n", SDL_GetError());
  } else {
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    SDL_RenderClear(sdlRenderer);
    SDL_RenderCopy(sdlRenderer, sdlScreenTexture, NULL, NULL);
  }
  SDL_Rect dest = {x,y,(int)(width*scale+0.5),(int)(height*scale+0.5)}; // Round
  SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &dest);
  if (persistent) {
    if (SDL_SetRenderTarget(sdlRenderer, NULL) != 0)
      printf("SDL_SetRenderTarget failed : %s\n", SDL_GetError());
    SDL_RenderCopy(sdlRenderer, sdlScreenTexture, NULL, NULL);
  }
  SDL_RenderPresent(sdlRenderer);
}


/** Draw a line on the persistent screen
 * @param x1 the x-coordinate of the first terminal
 * @param y1 the y-coordinate of the first terminal
 * @param x2 the x-coordinate of the last terminal
 * @param y2 the y-coordinate of the last terminal
 * @param a the color's alpha  component
 * @param r the color's red component
 * @param g the color's green component
 * @param b the color's blue component
 */
void displayDrawLine(int x1, int y1, int x2, int y2,
		     int a, int r, int g, int b) {
  if (SDL_SetRenderTarget(sdlRenderer, sdlScreenTexture) != 0)
    printf("SDL_SetRenderTarget failed : %s\n", SDL_GetError());
  if (SDL_SetRenderDrawColor(sdlRenderer, r, g, b, a) != 0) {
    printf("SDL_SetRenderDrawColor failed : %s\n",SDL_GetError());
  } else {
    if (SDL_RenderDrawLine(sdlRenderer, x1, y1, x2, y2) != 0)
      printf("SDL_RenderDrawLine failed : %s\n",SDL_GetError());
  }
  if (SDL_SetRenderTarget(sdlRenderer, NULL) != 0)
    printf("SDL_SetRenderTarget failed : %s\n", SDL_GetError());
} // displayDrawLine(int, int, int, int,int)

/** Draw a rectangle on the persistent screen
 * @param x the x-coordinate of the top-left corner
 * @param y the y-coordinate of the top-left corner
 * @param width the width of the rectangle
 * @param height the height of the rectangle
 * @param a the color's alpha  component
 * @param r the color's red component
 * @param g the color's green component
 * @param b the color's blue component
 * @param fill true iff the rectangle is to be filled
 */
void displayDrawRect(int x, int y, int width, int height,
		     int a, int r, int g, int b, _Bool fill) {
  if (SDL_SetRenderTarget(sdlRenderer, sdlScreenTexture) != 0)
    printf("SDL_SetRenderTarget failed : %s\n", SDL_GetError());
  if (SDL_SetRenderDrawColor(sdlRenderer, r, g, b, a) != 0) {
    printf("SDL_SetRenderDrawColor failed : %s\n",SDL_GetError());
  } else {
    struct SDL_Rect rect = {x,y,width,height};
    if (fill) {
      if (SDL_RenderFillRect(sdlRenderer, &rect) != 0)
	printf("SDL_RenderFillRect failed : %s\n",SDL_GetError());
    } else {
      if (SDL_RenderDrawRect(sdlRenderer, &rect) != 0)
	printf("SDL_RenderDrawRect failed : %s\n",SDL_GetError());
    }
  }
  if (SDL_SetRenderTarget(sdlRenderer, NULL) != 0)
    printf("SDL_SetRenderTarget failed : %s\n", SDL_GetError());
} // displayDrawLine(int, int, int, int,int)


/**
* Render the message we want to display to a texture for drawing
*/

void renderText(const char* text, const char *fontFile, int size, SDL_Color fg, SDL_Color bg ,int x, int y, int w, int h){
    TTF_Font* Arial = TTF_OpenFont("/usr/share/fonts/truetype/msttcorefonts/Arial.ttf", size);

    SDL_Surface* surfaceMessage = TTF_RenderText_Shaded(Arial, text, fg, bg);

    SDL_Texture* Message = SDL_CreateTextureFromSurface(sdlRenderer, surfaceMessage);

    SDL_Rect Message_rect;
    Message_rect.x = x;
    Message_rect.y = y;
    Message_rect.w = w;
    Message_rect.h = h;

    SDL_RenderCopy(sdlRenderer, Message, NULL, &Message_rect);
    //TTF_Quit();
}

void cShape_display_popup_frame(cShape_popup popup){
    //init popup frame
    //black border
    if(popup.type != -1){
        displayDrawRect(150, 150, 500, 300,
                 255, 0, 0, 0,
                 true);
        //grey interrior
        displayDrawRect(155, 155, 490, 290,
                 255, 73, 73, 73,
                true);

        switch(popup.type){
            case CSHAPE_POPUP_COLOR:
                //Displaying color choose
                //Vert
                displayDrawRect(225,300, 50,50,255,0,255,0,true);
                //Rouge
                displayDrawRect(325,300, 50,50,255,255,0,0,true);
                //Bleu
                displayDrawRect(425,300, 50,50,255,0,0,255,true);
                //Noir
                displayDrawRect(525,300, 50,50,255,0,0,0,true);
                break;

        }
    }
}

void cShape_display_popup_text(cShape_popup popup){

    if (popup.type != -1){
        SDL_Color fontColor = {255, 255, 255};
        SDL_Color backgroundFontColor = {73,73,73};
        renderText(popup.title, "Arial.ttf", 50, fontColor,backgroundFontColor, 160,160, 200,50);

        switch(popup.type){
            case CSHAPE_POPUP_COLOR:
                break;
            case CSHAPE_POPUP_IMAGE:
                break;
        }
    }
}






























