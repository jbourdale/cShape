/**
	Header définissant toutes les fonctions principalement graphique de
	l'application définies dans display.c
**/

#ifndef __DISPLAY__
#define __DISPLAY__

#include "cshape_struct.h"

/**
 * Delete all item of shape array
 * @param figures shape array
 * @param size size of figures
 */
void cShape_clear(cShape_shape* figures, int* size);
/**
 * Detecting if the last point of the shape is near the first one
 * to collapse them
 * @param shape to parse
 */
void cShape_rounding_last_lines(cShape_shape* shape);
/**
 * Copy shape and append it to figs.
 * @param shape shape to submit
 * @param figs shapes array
 * @param figs_size size of figs
 */
void cShape_submit_shape(cShape_shape* shape, cShape_shape figs[], int* figs_size);
/**
 * Init shape
 * @param shape shape to init
 */
void cShape_initShape(cShape_shape* shape);
/**
 * Init cShape display
 */
_Bool cShape_initDisplay();
/**
 * Render the message we want to display to a texture and draw it on the
 * persistent screen
 * @param text Message to draw
 * @param font font used to render the text
 * @param fg Font color
 * @param bg Background color
 * @param x x-coordinate
 * @param y y-coordinate
 * @param w width of rect where will be render the text
 * @param h height of rect where will be render the text
 */
void renderText(const char* text, TTF_Font* font, SDL_Color fg, SDL_Color bg ,int x, int y, int w, int h);
/**
 * Render the frame of popup
 * @param popup popup that will be display
 */
void cShape_display_popup_frame(cShape_popup popup);
/**
 * Render the text of popup
 * @param popup popup that will be display
 */
void cShape_display_popup_text(cShape_popup popup);
/**
 * Setter and init for popup p
 * @param p popup to set
 * @param type type of the popup p
 */
void cShape_set_popup(cShape_popup* p,int type);
/**
 * Init popup p
 * @param p popup to init
 */
void cShape_initPopup(cShape_popup* p);
/**
 * Render every item of cShape, all the shape, popup ...
 * @param figs array of shape we will render
 * @param figs_size size of figs
 * @param figActuel currently edit shape
 * @param popup cShape_popup that define if popup is up or no
 * @param bg Background color
 */
void cShape_render(cShape_shape figs[], int figs_size, cShape_shape figActuel, cShape_popup popup, SDL_Color bg);
/** Opens a window with a title and a size.
 * @param caption the title of the window
 * @param width the width of the window
 * @param height the height of the window
 * @return 0 if ok, 1 if error
 */
int displayInit(const char* caption, int width, int height);
/** Displays the persistent screen on-screen.
 */
void displayPersistentScreen();
/** Waits for the specified time.
 * @param milliseconds the time to wait. 1000 is 1 second.
 */
void delay(int milliseconds);
/** Displays the specified pixels at the specified location of the window.
 * @param pixels [in] a linear array of ARGB pixels (2x2 pixels => B00,G00,R00,A00,B01,G01,R01,B10,G10,R10,A10,B11,G11,R11)
 * @param x the left-coordinate of the top-left corner of the rectangle to fill with pixels
 * @param y the top-coordinate of the top-left corner of the rectangle to fill with pixels
 * @param with the width of the rectangle to fill with pixels
 * @param height the height of the rectangle to fill with pixels
 * @param scale the scale to apply to the pixels during drawing. 2 is twice larger, 0.25 is quarter.
 */
void displayPixels(void *pixels, int x, int y, int width, int height,float scale,_Bool persistent);
/** Prepares a texture of the requested size.
 * @param width the texture width
 * @param height the texture height
 * @return 0 if Ok ; -1 if error
 */
int displayPrepareTexture(int width, int height) ;
/** Updates the texture with specified pixels and put it on the window.
 * @param pixels [in] the pixels to put into the texture
 * @param x the left-coordinate of the top-left corner of the window rectangle to fill with pixels
 * @param y the top-coordinate of the top-left corner of the window rectangle to fill with pixels
 * @param width the width of the texture
 * @param height the height of the texture
 * @param scale the scale to apply to the pixels during drawing. 2 is twice larger, 0.25 is quarter.
 */
void displayUpdateAndDisplayTexture(void *pixels, int x, int y, int width, int height, float scale,_Bool persistent);
/** Draw a line on the screen
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
		     int a, int r, int g, int b);
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
		     int a, int r, int g, int b, _Bool fill);
/**
 * closes the window and terminates the SDL system
 */
void displayQuit();
/**
 * Render the down menu
 */
 void cShape_down_menu();
#endif
