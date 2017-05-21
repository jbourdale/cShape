# Présentation

cShape est une application permettant à des utilisateurs de dessiner
des polygones sur un écran partagé sur le réseau par tous.
Projet réalisé dans le cadre d'un projet d'étude de 2ème année 
de DUT Informatique.

# Dépendances

	- libsdl2-dev
	- libsdl2-ttf-dev

# Compilation

Il y a deux unités à compiler séparement, le client et le serveur.
Dans cShape/ :
	- make : Compile le client et le serveur
	- make cShape_client : Compile le client
	- make cShape_serveur : Compile le serveur
	- make clean : Supprime les exécutables

# Utilisation

L'application est pensée pour que l'expérience utilisateur soit la plus
confortable possible. Les clients ont donc une fenêtre avec laquelle
ils peuvent interagir via les I/O standards (clavier/souris).

	# Lancement
		Serveur: ./cShape_serveur [PORT]
			où PORT défini le port d'écoute sur lequel les clients
			enverront leur polygones.

		Client: ./cShape_client [IP] [PORT]
			où l'IP et le port correspondent à un serveur actif.

	# Use
		Serveur: Il n'y a pas d'I/O sur l'application serveur, les logs
			peuvent être redirigés vers un fichier via ">> out".

		Client: 
			Dessin : A la souris, cliquer pour valider une nouvelle ligne
				puis sur ENTREE pour valider et envoyer la figure au
				serveur.

			Couleur : Appuyer sur C puis cliquer sur la couleur avec laquelle
				vous voulez dessiner.

			Effacer l'écran : Appuyer sur R puis cliquer sur la couleur avec 
				laquelle vous voulez effacer l'écran.
			
			Voir barre de menu du bas de la fenêtre.

	# Exit
		Serveur: SIGKILL/SIGINT.
		Client : SIGKILL/SIGINT/Fermeture de la fenêtre graphique.

# Fonctionnement

	Serveur : 
		- Ouverture et écoute d'un socket UDP sur le port spécifié.
		- Gestion d'une liste de client (Connexion/Deconnexion).
		- Réception des commandes et relais aux autres si c'est une
		  instruction de dessin (figure / effacement).

	Client :
		- Ouverture d'un socket UDP.
		- Envoi d'une requête de connexion au serveur.
		- Si connecté, envoi et réception des instructions
		  de dessin sur le serveur.
		- Lors de la fermeture: déconnexion.

# Protocole
	
	Envoi d'une figure:
		Requête : F nb_lignes A R G B x1 y1 x2 y2 ...
		Taille  : 
			F = 1 o (char)
			nb_lignes = 4 o (int)
			A,R,G,B = 4 * 1 o (unsigned char)
			x1,y1,x2,y2 = nb_lignes * 16 o (4 int)

		nb_lignes ne peut pas dépasser 90 car je voulais que la taille
		de la plus grande requête ne soit pas découpée en plusieurs trames. 
		Or une trame UDP pèse 1480o et

			(1480 - 8 - 9)/16 = 91,4 arroundi à 90

		où 8 est l'entête UDP et 9 la taille constante de la requête.

		Cet envoi est plus lourd que celui dans le protocole suggéré dans
		l'énoncé du projet pour une ligne simple (25o contre 21o) et un 
		rectangle (73o contre 21o).
		Mais permet une meilleure flexibilité avec la possibilité 
		d'envoyer des figures autre qu'une ligne ou un rectangle.

		L'envoi de nb_lignes peut paraitre facultatif (déduction avec
		le nombre de coordonnées reçu) mais permet de verifier que
		la figure envoyée est complète.

	Connexion (pour que le serveur enregistre ce client):
		Requête : N
		Taille : 
			N = 1 o (char) (N = NEW CLIENT)

	Deconnexion (pour que le serveur arrete d'envoyé a ce client):
		Requête : D
		Taille :
			D = 1 o (char)

	Effacer l'ecran :
		Requête : C A R G B
		Taille : 
			C,A,R,G,B = 5 * 1o (char)

# Auteur

	Jules Bourdalé.
	etudiant@jbourdale.fr
