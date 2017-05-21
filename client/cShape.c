#include "cshape.h" //Toutes les fonctions non graphique (initialisation, network ...)
#include "display.h" //Fonctions uniquement graphique

static struct sockaddr_in servAddr;

int main(int argc, char** argv){
	delay(1000);
  //Init network
	int skt = init_skt(argv, argc);

	//Init SDL display
	if(!cShape_initDisplay()){
			errno_exit("Error init display");
	}

	//Run
  cShape_mainLoop(skt);

	//Quit
	cShape_quit();
}
/**
 * Closing SDL
 */
void cShape_quit(){
	displayQuit();
	TTF_Quit();
}
/**
 * Handle and parsing incomming bytes from serveur
 * @param skt UDP socket
 * @param figures shape array of all shape draw
 * @param nb_figure size of figures
 * @param bg Background color
 */
void handle_reading(int skt, cShape_shape* figures, int *nb_figures, SDL_Color* bg){

  char clientMsg[1480]; //On ne recevra la longueur d'une trame UDP => Pas de découpage
  //unsigned int tmpSize = sizeof(servAddr);
  int msgSize;

  if ((msgSize=recvfrom(skt, clientMsg,1480, 0, NULL,NULL))<0){
    perror("recvfrom");
    exit(-1);
  }

  if (msgSize>-1){
    clientMsg[msgSize] = 0; // insère le \0 final au cas où le client ne l'aurait pas mis
    printf("Got %d bytes\n", msgSize);
		for(int i=0; i<msgSize;i++){
			printf("%x", clientMsg[i]);
		}
		printf("\n");
  }

	int curseur = 0;
	switch(clientMsg[curseur]){
		case 'F':
				curseur++; //commence a 1 car 0 = 'F'
				cShape_shape new;
				cShape_initShape(&new);

				//Getting nb_lines
				unsigned char bytes[4];
				memcpy(bytes, &clientMsg[curseur], sizeof(int));
				curseur += 4;
				new.nb_lines = bytesToInt(bytes, 4);

				//Getting color
				new.color.a = clientMsg[curseur]; curseur++;
				new.color.r = clientMsg[curseur]; curseur++;
				new.color.g = clientMsg[curseur]; curseur++;
				new.color.b = clientMsg[curseur]; curseur++;

				for(int i=0; i<new.nb_lines;i++){
					//x1
					memset(bytes, 0, 4);
					memcpy(bytes, &clientMsg[curseur], sizeof(int));
					new.lines[i].plot_first.x = bytesToInt(bytes, 4);
					curseur += 4;
					//y1
					memset(bytes, 0, 4);
					memcpy(bytes, &clientMsg[curseur], sizeof(int));
					new.lines[i].plot_first.y = bytesToInt(bytes, 4);
					curseur += 4;
					//x2
					memset(bytes, 0, 4);
					memcpy(bytes, &clientMsg[curseur], sizeof(int));
					new.lines[i].plot_last.x = bytesToInt(bytes, 4);
					curseur += 4;
					//y2
					memset(bytes, 0, 4);
					memcpy(bytes, &clientMsg[curseur], sizeof(int));
					new.lines[i].plot_last.y = bytesToInt(bytes, 4);
					curseur += 4;
				}

				cShape_submit_shape(&new, figures, nb_figures);

				break; //case F
		case 'C':
		case 'N':
				curseur++;
				bg->a = clientMsg[curseur]; curseur++;
				bg->r = clientMsg[curseur]; curseur++;
				bg->g = clientMsg[curseur]; curseur++;
				bg->b = clientMsg[curseur]; curseur++;
				cShape_clear(figures, nb_figures);
				break;
	}
}
/**
 * Convert bytes array into int
 * @param b bytes array
 * @param length length of int (4 default)
 */
int bytesToInt(unsigned char* b, unsigned length)
{
	int val = 0;
	int j = 0;
	for (int i = length-1; i >= 0; --i)
	{
		val += (b[i] & 0xFF) << (8*j);
		++j;
	}

	return val;
}
/**
 * Convert int to bytes array
 * @param buff pointer on bytes array to fill
 * @param x int to convert
 */
void intToBytes(unsigned char buff[], unsigned int x){
   unsigned int tmp = htonl(x);
   memcpy(buff, &tmp, sizeof(int));
}
/**
 * Check if x and y are in r
 */
_Bool in_bounds(SDL_Rect r, int x, int y){
    return ((x > r.x && x < r.x+r.w) && (y > r.y && y < r.y + r.w));
}
/**
 * Common errno exit
 * @param s Msg to display before exit
 */
void errno_exit (const char *s)
{
  fprintf (stderr, "%s error %d, %s\n",s, errno, strerror (errno));
  exit (EXIT_FAILURE);
}
/**
 * Open zenity (installed by default on most UNIX distro) file selector to pick
 * a image
 * @param path str where path will be returned
 */
void cShape_image_file_picker(char path[1024]){
    FILE *fp;
    fp = popen("/usr/bin/zenity --file-selection --file-filter=''*.png' '*.jpg''", "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit(1);
    }

    /* Read the output a line at a time - output it. */
    while (fgets(path, 1023, fp) != NULL);

    /* close */
    pclose(fp);

}
/**
 * Send something to the server to allow the server to save this client
 * And wait for confirmation (1sec)
 * @param skt UDP socket
 */
_Bool cShape_connect(int skt){
	if (sendto(skt, "N", 1, 0, (struct sockaddr*) &servAddr, sizeof(servAddr))<0)
		perror("sendto");

  //Wait for confirmation

	struct timeval timeout;     /* timeout for select() */
	timeout.tv_sec = 1;         /* seconds */
	timeout.tv_usec = 0;        /* microseconds */
	fd_set readfs;

	FD_ZERO(&readfs);FD_SET(skt, &readfs); //Reset select();

	//Non blocking read() on socket
	if(select(skt + 1, &readfs, NULL, NULL, &timeout) < 0){
		perror("select()");exit(errno);
	}
	/* READING IF DATA AVAILIBLE*/
	if(FD_ISSET(skt, &readfs)){
		char clientMsg[1];
		int msgSize;

		if ((msgSize=recvfrom(skt, clientMsg,1, 0, NULL,NULL))<0){
			perror("recvfrom");
			exit(-1);
		}
		if(clientMsg[0] == 'Y'){
			printf("Connected\n");
			return true;
		}
		if(clientMsg[0] == 'N'){
			printf("Serveur full, can't connect.\n");
			return false;
		}
	}
  return false;
}
/**
 * Tell the server to not handle this client anymore
 * @param skt UDP socket
 */
void cShape_disconnect(int skt){
	if (sendto(skt, "D", 1, 0, (struct sockaddr*) &servAddr, sizeof(servAddr))<0)
		perror("sendto");
}
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
void handle_event(_Bool *quit,cShape_shape *figActuel,cShape_shape figures[],int *nb_figures,cShape_popup *popup, int skt, SDL_Color* bg){
  /* VARIABLE POUR LE COLOR PICKER */
  _Bool clicOnColor = false;
  SDL_Color cVert = {0,255,0,255};
  SDL_Color cRouge = {255,0,0,255};
  SDL_Color cBleu = {0,0,255,255};
  SDL_Color cNoir = {0,0,0,255};
	SDL_Color cBlanc = {255,255,255,255};

  SDL_Rect vert = {175,300, 50,50};
  SDL_Rect rouge = {275,300, 50,50};
  SDL_Rect bleu = {375,300, 50,50};
  SDL_Rect noir = {475,300, 50,50};
	SDL_Rect blanc = {575,300,50,50};
  /* FIN VAR COLOR PICKER */

  char path[1024];

  int mouse_x,mouse_y;
  SDL_Event e;

  while(SDL_PollEvent(&e)){
      switch(e.type){
          //Quit event
          case SDL_QUIT:
              *quit = true;
              break;

          case SDL_KEYDOWN:
              switch(e.key.keysym.sym){
                  case SDLK_RETURN:
											if(cShape_send_shape(*figActuel, skt) > 0){
													cShape_submit_shape(figActuel,figures, nb_figures);
											}
                      break;

                  case SDLK_c:
                      cShape_set_popup(popup,CSHAPE_POPUP_COLOR);
                      break;

                  case SDLK_i:
                      cShape_image_file_picker(path);
                      printf("Path : %s\n", path);
                      break;

                  case SDLK_r:
											cShape_set_popup(popup, CSHAPE_POPUP_REFRESH);
                      break;
              }
              break;

          case SDL_MOUSEMOTION:
              //Si la souris bouge, la ligne qu'on est entrain de tracer suivra
              //printf("Bonjour\n");
              SDL_GetMouseState(&mouse_x, &mouse_y);
              if(!(figActuel->nb_lines == 0 && figActuel->lines[figActuel->nb_lines].plot_first.x == -1)){
                  figActuel->lines[figActuel->nb_lines].plot_last.x = mouse_x;
                  figActuel->lines[figActuel->nb_lines].plot_last.y = mouse_y;
              }
              break;
          //Mouse Event
          case SDL_MOUSEBUTTONDOWN:
              SDL_GetMouseState(&mouse_x, &mouse_y);
              /*printf("MouseEvent at (%d,%d)\n", mouse_x, mouse_y);*/

              //Si il n'y a pas de popup active
              if(popup->type == -1){

                  //Si c'est le tout premier point de la figure
                  if(figActuel->nb_lines == 0 && figActuel->lines[figActuel->nb_lines].plot_first.x == -1){
                      //On set le premier point
                      figActuel->lines[figActuel->nb_lines].plot_first.x = mouse_x;
                      figActuel->lines[figActuel->nb_lines].plot_first.y = mouse_y;

                      //Et on mets le dernier de la ligne au niveau du curseur
                      //car il a été initialisé a -1;-1
                      figActuel->lines[figActuel->nb_lines].plot_last.x = mouse_x;
                      figActuel->lines[figActuel->nb_lines].plot_last.y = mouse_y;

                  }else{
                      //Si c'est n'importe quel autre point
                      //On finit la ligne actuelle
                      figActuel->lines[figActuel->nb_lines].plot_last.x = mouse_x;
                      figActuel->lines[figActuel->nb_lines].plot_last.y = mouse_y;
                      //Et on passe a la suivante en initialisant son premier point
                      //a la suite de la ligne que l'on vient de finir
                      figActuel->nb_lines = figActuel->nb_lines+1;
                      figActuel->lines[figActuel->nb_lines].plot_first.x = mouse_x;
                      figActuel->lines[figActuel->nb_lines].plot_first.y = mouse_y;

                      figActuel->lines[figActuel->nb_lines].plot_last.x = mouse_x;
                      figActuel->lines[figActuel->nb_lines].plot_last.y = mouse_y;

											if(figActuel->nb_lines == CSHAPE_NBLINESMAX_PER_SHAPE-1){
												if(cShape_send_shape(*figActuel, skt) > 0){
														cShape_submit_shape(figActuel,figures, nb_figures);
												}
											}

                  }
              }else{ //Clic on popup
                  switch(popup->type){
                      case CSHAPE_POPUP_COLOR: //Clic sur popup "color"
                      		clicOnColor = true;
                          if(in_bounds(vert, mouse_x, mouse_y)){
                              figActuel->color = cVert;
                          }else if(in_bounds(rouge, mouse_x, mouse_y)){
                              figActuel->color = cRouge;
                          }else if(in_bounds(bleu, mouse_x, mouse_y)){
                              figActuel->color = cBleu;
                          }else if(in_bounds(noir, mouse_x, mouse_y)){
                              figActuel->color = cNoir;
                          }else if(in_bounds(blanc, mouse_x, mouse_y)){
															figActuel->color = cBlanc;
													}else{
														clicOnColor = false;
													}

                          if(clicOnColor){
                              cShape_initPopup(popup);
                          }
                          break; //fin color
											case CSHAPE_POPUP_REFRESH: //clic popup refresh
													clicOnColor = true;
													if(in_bounds(vert, mouse_x, mouse_y)){
															*bg = cVert;
													}else if(in_bounds(rouge, mouse_x, mouse_y)){
															*bg = cRouge;
													}else if(in_bounds(bleu, mouse_x, mouse_y)){
															*bg = cBleu;
													}else if(in_bounds(noir, mouse_x, mouse_y)){
															*bg = cNoir;
													}else if(in_bounds(blanc, mouse_x, mouse_y)){
															*bg = cBlanc;
													}else{
														clicOnColor = false;
													}
													if(clicOnColor){ //Si on a bien clic sur une case couleur
															cShape_initPopup(popup);
															cShape_clear(figures, nb_figures);
															char request[5];
															request[0] = 'C';
															request[1] = bg->a;
															request[2] = bg->r;
															request[3] = bg->g;
															request[4] = bg->b;

															printf("Sending (5 bytes) : ");
															for(int i=0; i<5;i++){
																printf("%x ", request[i]);
															}
															printf("\n");

															if (sendto(skt, request, 5, 0, (struct sockaddr*) &servAddr, sizeof(servAddr))<0) perror("sendto");
													}
													break; //fin refresh
                  }
              }
              break; //mouseEvent
      }
  }
}
/**
 * Return a initialised socket
 */
int init_skt(char** argv, int argc){

	//init socket
	int skt = socket(AF_INET, SOCK_DGRAM, 0);
	if (skt<0) perror("socket");

	if(argc != 3){
		usage(argv[0]);
		errno_exit("Exiting");
	}else{
		int try=0;
		_Bool connect = false;
		while(try < 3 && !connect){
			//argv 1 : adresse du serveur
			char* ip = argv[1];
			//argv 2 : port, checking if it's a int
			int port = atoi(argv[2]);

			//Init server addr
		  servAddr.sin_family = AF_INET;
		  servAddr.sin_port = htons(port);
		  servAddr.sin_addr.s_addr = inet_addr(ip);

			printf("Connexion n°%d on %s port %d...\n", try+1, ip, port);
			connect = cShape_connect(skt);
			try++;
		}
		if(!connect){
			errno_exit("Can't connect to serveur. Exiting");
		}
	}
	return skt;
}
/**
 * Main loop of the application, handle SDL event and network event
 * @param skt UDP socket
 */
void cShape_mainLoop(int skt){
    /* SELECT() VARIABLES */
    struct timeval timeout;     /* timeout for select() */
    timeout.tv_sec = 0;         /* seconds */
    timeout.tv_usec = 0;        /* microseconds */
    fd_set readfs;
    /* END SELECT() VARIABLES */

    /* CSHAPE GUI VARIABLES */
    int nb_figures = 0;
    cShape_shape figures[CSHAPE_NBFIGMAX]; //Tableau de figures
    cShape_popup popup; cShape_initPopup(&popup);
    cShape_shape figActuel; cShape_initShape(&figActuel); //Figure qu'on est entrain de construire
		SDL_Color backgroundColor = {255,255,255,255};
    /* END CSHAPE GUI VARIABLES */

    //cShape loop
    _Bool quit = false;
    while(!quit){
      FD_ZERO(&readfs);FD_SET(skt, &readfs); //Reset select();

      //Non blocking read() on socket
      if(select(skt + 1, &readfs, NULL, NULL, &timeout) < 0){
        perror("select()");exit(errno);
      }
      /* READING IF DATA AVAILIBLE*/
      if(FD_ISSET(skt, &readfs)){
          handle_reading(skt, figures, &nb_figures, &backgroundColor);
      }
      /* EVENTS */
      else{
          handle_event(&quit,&figActuel,figures,&nb_figures,&popup, skt, &backgroundColor);
      }

      //Render scene
      delay(80);
      cShape_render(figures, nb_figures, figActuel, popup, backgroundColor);
    }
		cShape_disconnect(skt);
}
/**
 * Send shape sh to server using skt
 * @param sh cShape_shape to send
 * @param skt UDP socket
 */
int cShape_send_shape(cShape_shape sh, int skt){
  /*
    Syntaxe : F  nb_lines  a  r  g  b   x1  y1  x2  y2 ..
    Octects : 1     4      1  1  1  1   4   4   4   4

    taille : 1  +   4      +   4*1    + nb_lines * (4*4)
    taille max : 9+90*16 = 1449 octets + 8 entete UDP = 1457octets
  */

	if (sh.nb_lines < 1){
		return -1;
	}

	char request[1480];
	memset(request, 0, 1480);
  int curseur = 0;
  unsigned char bytes[4]; //bytes array for 4bytes int
  unsigned char letter = 'F'; //request letter

  //Adding letter to the request
  request[curseur] = letter;
  curseur++;

  //Adding nb_lines
  intToBytes(bytes,sh.nb_lines);
  for(int i = 0; i<sizeof(int); i++){
    request[curseur] = bytes[i];
    curseur++;
  }

  //Adding color
  request[curseur] = sh.color.a;
  curseur++;
  request[curseur] = sh.color.r;
  curseur++;
  request[curseur] = sh.color.g;
  curseur++;
  request[curseur] = sh.color.b;
  curseur++;

  //Adding lines
  for(int i = 0; i<sh.nb_lines; i++){
    memset(bytes, 0, 4); //reset bytes buffer
    //x1
    intToBytes(bytes, sh.lines[i].plot_first.x);
    for(int i = 0; i<sizeof(int); i++){
      request[curseur] = bytes[i];
      curseur++;
    }
    //y1
    intToBytes(bytes, sh.lines[i].plot_first.y);
    for(int i = 0; i<sizeof(int); i++){
      request[curseur] = bytes[i];

      curseur++;
    }
    //x2
    intToBytes(bytes, sh.lines[i].plot_last.x);
    for(int i = 0; i<sizeof(int); i++){
      request[curseur] = bytes[i];
      curseur++;
    }
    //y2
    intToBytes(bytes, sh.lines[i].plot_last.y);
    for(int i = 0; i<sizeof(int); i++){
      request[curseur] = bytes[i];
      curseur++;
    }
  }

	printf("Sending (%d bytes) : ", curseur);
	for(int i=0; i<curseur;i++){
		printf("%x ", request[i]);
	}
	printf("\n");
	
	if (sendto(skt, request, curseur, 0, (struct sockaddr*) &servAddr, sizeof(servAddr))<0)
		perror("sendto");

	return 1;

}
/**
 * Print CLI help to launch properly cShape
 */
void usage(char *name){
	printf("Usage : \n");
	printf("\t%s IP PORT\n", name);
	printf("Exemple : \n");
	printf("\t\t %s 127.0.0.1 12345\n", name);
	exit(-1);
}
