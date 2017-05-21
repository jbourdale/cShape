#include "serveur.h"

/**
 * Delete the id user from clients array, message won't be send to him anymore
 * @param id client ID
 * @param clients array client
 * @param nbClients size of clients
 */
void cshape_disconnect(int id, struct sockaddr_in clients[], int *nbClients){
  printf("Disconnecting client %d\n", id);

  struct sockaddr_in new[CSHAPE_NB_CLIENTS];
  int i=0, j=0;

  while(i<*nbClients){
    if (i!= id){
      new[j] = clients[j];
      j++;
    }
    i++;
  }

  --*nbClients;
  printf("Clients remaining : %d", *nbClients);
  clients = new;
}
/**
 * Custom sendTo function that send msg to clientID
 * clientID can be equals to CSHAPE_BROADCAT, in this case, msg will be send
 * to all 'connected' clients
 * @param socket UDP server socket
 * @param msg Text to send
 * @param msgSize size of msg
 * @param flags sendTo msg, rf man sendto
 * @param clients list of all client
 * @param nbClients size of clients
 * @param clientID client id dest
 * @param senderID client id source
 */
int cshape_sendTo(int socket, char* msg, int msgSize,int flags, struct sockaddr_in clients[], int nbClients, int clientID, int senderID){

  if(clientID == CSHAPE_BROADCAST){
    for(int i=0; i<nbClients; i++){
        if(i != senderID){ //Ne pas le renvoyé a l'user qui est la source
          printf("Sending (%d bytes) to client n°%d ", msgSize, i);
          printAddr("at", &clients[i]);
          printf(" >> ");
          for(int i=0; i<msgSize;i++){
            printf("%x ", msg[i]);
          }
          printf("\n");

          if(sendto(socket, msg, msgSize, flags,(struct sockaddr*) &clients[i], sizeof(clients[i]))<msgSize){
            perror("sendTo broadcast");
            return -1;
          }
        }
    }
  }else{
    printf("Sending (%d bytes) to client n°%d ", msgSize, clientID);
    printAddr("at", &clients[clientID]);
    printf(" >> ");
    for(int i=0; i<msgSize;i++){
      printf("%x ", msg[i]);
    }
    printf("\n");

    if(sendto(socket, msg, msgSize, flags,(struct sockaddr*) &clients[clientID], sizeof(clients[clientID]))<msgSize){
      perror("sendTo clientID");
      return -1;
    }
  }
  return 0;
}
/**
 * Init the client array
 * @param clients client array
 */
void init_clients(struct sockaddr_in clients[]){

  for(int i= 0; i < CSHAPE_NB_CLIENTS;i++){
    clients[i].sin_port = 0;
    clients[i].sin_addr.s_addr = INADDR_ANY;
  }
}
/**
 * Return the client ID
 * @param clients clients array
 * @param nbClients size of clients
 * @param cli client to look for
 */
int getClientId(struct sockaddr_in clients[], int nbClients, struct sockaddr_in cli){
  int found = 0,i=0;
  while(!found && i < nbClients){
    if(clients[i].sin_port == cli.sin_port && clients[i].sin_addr.s_addr == cli.sin_addr.s_addr)
        found = 1;
    else i++;
  }
  return i;
}
/**
 * Return true if the client cli is in clients[]
 * @param clients client array
 * @param nbClients size of clients
 * @param cli client to look for
 */
_Bool isAlreadyKnowed(struct sockaddr_in clients[], int nbClients, struct sockaddr_in cli){

  int found = 0,i=0;
  while(!found && i <= nbClients){
    if(clients[i].sin_port == cli.sin_port && clients[i].sin_addr.s_addr == cli.sin_addr.s_addr)
        found = 1;
    i++;
  }
  return found;
}

/**
 * Adding cli to clients[] list
 * @param clients client array
 * @param nbClients size of clients
 * @param cli client to add
 * @return the new number of clients
 */
int addClient(struct sockaddr_in clients[], int nbClients, struct sockaddr_in* cli){
  if (nbClients < CSHAPE_NB_CLIENTS){
    printAddr("New client ", cli);
    struct sockaddr_in new;
    new.sin_family = cli->sin_family;
    new.sin_port = cli->sin_port;
    new.sin_addr.s_addr = cli->sin_addr.s_addr;

    clients[nbClients] = new;
    nbClients++;
  }
  printf("Clients connected : %d\n", nbClients);
  return nbClients;
}
/**
 * Describe how to use the program
 * @param name name of the launcher (argv[0])
 */
void usage(char *name){
  printf("cShape server version\n");
  printf("Usage : \n");
  printf("\t%s PORT\n", name);
  printf("Exemple : \n");
  printf("\t%s 12345 \n", name);
  exit(-1);
}


int main(int argc, char** argv) {

  if(argc != 2) usage(argv[0]);

  int port = atoi(argv[1]);
  if (port == 0) usage(argv[0]);

  printf("Launching serveur...\n");

  int skt = socket(AF_INET, SOCK_DGRAM, 0);
  if (skt<0){
    perror("socket init");
    exit(-1);
  }

  struct sockaddr_in servAddr, tmpAddr;
  struct sockaddr_in clientsAddr[CSHAPE_NB_CLIENTS];
  int nbClients=0;


  //Init serv addr
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(port);
  servAddr.sin_addr.s_addr = INADDR_ANY;

  printAddr("Serveur running on ", &servAddr);
  printf("\n");
  //Binding skt on servAddr
  if (bind(skt, (struct sockaddr*) &servAddr, sizeof(servAddr))<0){
    perror("bind");
    exit(-1);
  }

  char clientMsg[CSHAPE_TAILLE_RECIEVE]; //On ne recevra la longueur d'une trame UDP => Pas de découpage
  unsigned int tmpSize = sizeof(tmpAddr);
  int msgSize;

  //Server loop
  _Bool serv_running = 1;
  while (serv_running) {
    //Reception
    if ((msgSize=recvfrom(skt, clientMsg, CSHAPE_TAILLE_RECIEVE-1, 0, (struct sockaddr*) &tmpAddr, &tmpSize))<0) {
      perror("recvfrom");break;
    }
    if (msgSize>-1){ //Si on a recu qqchose
      printf("Get %d bytes ", msgSize);
      printAddr("from ", &tmpAddr);
    }

    int idSender = getClientId(clientsAddr,nbClients,tmpAddr);

    switch(clientMsg[0]){ //First letter of the msg
      case 'D': //Deconnexion
        cshape_disconnect(idSender,clientsAddr, &nbClients);
        break;
      case 'N': //New client
        if (!isAlreadyKnowed(clientsAddr, nbClients, tmpAddr)){
          int tmp = addClient(clientsAddr, nbClients, &tmpAddr);
          //Si le client a bien été ajouté
          if (tmp != nbClients){
            nbClients = tmp;
            //On envoit au client qui vient de se connecter qu'on la bien recu
            if (cshape_sendTo(skt, "Y",1,0,clientsAddr,nbClients, idSender, -1)<0){
              perror("cshape_sendto");
              break;
            }
            //On envoit a tout le monde le fait de reset son affichage (pour synchroniser l'afficahge de tout le monde)
            char request[5];
            request[0] = 'C';
            request[1] = 0xFF; //255
            request[2] = 0xFF;
            request[3] = 0xFF;
            request[4] = 0xFF;
            if (cshape_sendTo(skt, request,5,0,clientsAddr,nbClients, CSHAPE_BROADCAST, idSender)<0){
              perror("cshape_sendto");
              break;
            }

          }else{
            //Refus de connexion
            printf("Serveur full\n");
            if (cshape_sendTo(skt, "N",1,0,clientsAddr,nbClients, idSender, -1)<0){
              perror("cshape_sendto");
              break;
            }
          }
        } //fin nouveau client
        break;
      default:
        printf("Broadcast except client n°%d\n", idSender);
        //Envoi a tous les autres clients ce que le serveur vient de recevoir
        if (cshape_sendTo(skt, clientMsg,msgSize,0,clientsAddr,nbClients, CSHAPE_BROADCAST, idSender)<0){
          perror("cshape_sendto");
          break;
        }
        break;
    }//end switch
  } //end serveur loop

  close(skt);
  return 0;
}
