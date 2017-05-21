#ifndef __SERVEUR_H__
#define __SERVEUR_H__

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "utils.h"

#define CSHAPE_TAILLE_RECIEVE           1480
#define CSHAPE_NB_CLIENTS               20
#define CSHAPE_BROADCAST                999

/**
 * Delete the id user from clients array, message won't be send to him anymore
 * @param id client ID
 * @param clients array client
 * @param nbClients size of clients
 */
void cshape_disconnect(int id, struct sockaddr_in clients[], int *nbClients);
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
int cshape_sendTo(int socket, char* msg, int msgSize,int flags, struct sockaddr_in clients[], int nbClients, int clientID, int senderID);
/**
 * Init the client array
 * @param clients client array
 */
void init_clients(struct sockaddr_in clients[]);
/**
 * Return the client ID
 * @param clients clients array
 * @param nbClients size of clients
 * @param cli client to look for
 */
int getClientId(struct sockaddr_in clients[], int nbClients, struct sockaddr_in cli);
/**
 * Return true if the client cli is in clients[]
 * @param clients client array
 * @param nbClients size of clients
 * @param cli client to look for
 */
_Bool isAlreadyKnowed(struct sockaddr_in clients[], int nbClients, struct sockaddr_in cli);
/**
 * Adding cli to clients[] list
 * @param clients client array
 * @param nbClients size of clients
 * @param cli client to add
 * @return the new number of clients
 */
int addClient(struct sockaddr_in clients[], int nbClients, struct sockaddr_in* cli);
/**
 * Describe how to use the program
 * @param name name of the launcher (argv[0])
 */
void usage(char *name);

#endif
