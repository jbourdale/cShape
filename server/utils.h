#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

/* Mets en majuscules la chaîne : 32 octets entre le 'a' (97) et le 'A' (65)
int i;
for (i=0; i<recu; ++i)
  if ((tmp[i]>='a') && (tmp[i]<='z'))
     tmp[i] -= 32;
*/


/** Affiche le message spécifié, ainsi que l'adresse réseau donnée.
 * @param msg le message à afficher
 * @param adr l'adresse à afficher
 */
void printAddr(char * msg, struct sockaddr_in* adr) {
  char* ip = inet_ntoa(adr->sin_addr);
  int port = ntohs(adr->sin_port);
  printf("%s : IP=%s Port=%d\n",msg, ip, port);
}

/** Remplit le buffer specifié avec le nombre exact d'octets demandés.
 * @param s la socket ou lire les données
 * @param buf le buffer où stocker les données
 * @param taille le nombre d'octets à lire
 * @return 0 si lecture réussi, -1 si erreur.
 */
int lireTCP(int s, void *buf, int taille) {
#ifdef DEBUG
  printf("Je lis %d octets...\n",taille);
#endif
  while (taille > 0) {
    int l = read(s, buf, taille);
    if (l<0) {
      perror("lireTCP");
      return -1;
    } else if (l==0) {
#ifdef DEBUG
      printf("Plus de données :(( \n");
#endif
      return -1;
    } else {
      taille -= l;
      buf += l;
    }
  } // while
#ifdef DEBUG
  printf("fait!\n");
#endif
  return 0;
} // lireTCP(s,buf,taille)
