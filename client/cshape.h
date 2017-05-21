/**
  Header pour toutes les fonctions non graphique de cShape :
  Reseau, utilitaires... définie dans cShape.c
**/

#ifndef __CSHAPE_H__
#define __CSHAPE_H__

#include "cshape_struct.h"

/**
 * Print CLI help to launch properly cShape
 */
void usage(char *name);
/**
 * Handle and parsing incomming bytes from serveur
 * @param skt UDP socket
 * @param figures shape array of all shape draw
 * @param nb_figure size of figures
 * @param bg Background color
 */
void handle_reading(int skt, cShape_shape* figures, int *nb_figures, SDL_Color* bg);
/**
 * Convert bytes array into int
 * @param b bytes array
 * @param length length of int (4 default)
 */
int bytesToInt(unsigned char* b, unsigned length);
/**
 * Convert int to bytes array
 * @param buff pointer on bytes array to fill
 * @param x int to convert
 */
void intToBytes(unsigned char buff[], unsigned int x);
/**
 * Check if x and y are in r
 */
_Bool in_bounds(SDL_Rect r, int x, int y);
/**
 * Common errno exit
 * @param s Msg to display before exit
 */
void errno_exit (const char *s);
/**
 * Open zenity (installed by default on most UNIX distro) file selector to pick
 * a image
 * @param path str where path will be returned
 */
void cShape_image_file_picker(char path[1024]);
/**
 * Send something to the server to allow the server to save this client
 * And wait for confirmation (1sec)
 * @param skt UDP socket
 */
_Bool cShape_connect(int skt);
/**
 * Tell the server to not handle this client anymore
 * @param skt UDP socket
 */
void cShape_disconnect(int skt);
/**
 * Closing SDL
 */
void cShape_quit();
/**
 * Handle all SDL_Event in the main_loop and react to them
 * @param quit main loop boolean
 * @param figActuel in construction shape
 * @param figures list of all users shapes
 * @param nb_figures size of figures
 * @param popup struct that define cshape popup
 * @param skt UDP socket
 * @param bg Background color
 */
void handle_event(_Bool *quit,cShape_shape *figActuel,cShape_shape figures[],int *nb_figures,cShape_popup *popup, int skt, SDL_Color* bg);
/**
 * Return a initialised socket
 */
int init_skt();
/**
 * Main loop of the application, handle SDL event and network event
 * @param skt UDP socket
 */
void cShape_mainLoop(int skt);
/**
 * Send shape sh to server using skt
 * @param sh cShape_shape to send
 * @param skt UDP socket
 */
int cShape_send_shape(cShape_shape sh, int skt);

#endif
