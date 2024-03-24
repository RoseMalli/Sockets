#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT 8080

void crc(int trame[], int generateur[], int trame_fin, int longueur_trame, int longueur_generateur) {
	int i = 0;
	while(i < longueur_trame) {
		for(int j = 0; j < longueur_generateur; j++) {
			if(trame[i+j] == generateur[j]) {
				trame[i+j] = 0;
			}
			else {
				trame[i+j] = 1;
			}
		}
		while(i < trame_fin && trame[i] != 1) {
			i++;
		}
	}
}

int main() {
    char buffer[50]; 
	char buff[256];
	int socketClient;
	struct sockaddr_in AddrServeur; 
	unsigned int AddrLen; 

	socketClient = socket(AF_INET, SOCK_STREAM,0);

	if(socketClient == -1) {
		printf("La création de socket a échouée...\n");
		exit(0);
	}
	
	else {
		printf("La socket a été créée avec succès...\n");
	}

	AddrServeur.sin_family = AF_INET; 
	AddrServeur.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	AddrServeur.sin_port = htons(PORT); 

	AddrLen = (unsigned int)sizeof(AddrServeur); 

	if(connect(socketClient, (struct sockaddr *)&AddrServeur, AddrLen) == -1){
		printf("Erreur connect \n");
		exit(0);
	}

    while(1) {
		programme:
			printf("\nSaisissez votre trame (16 bits maximum): ");
			char trame_char[256];
			scanf("\n%[^\n]", trame_char);

			char generateur_char[256] = "101";

			int longueur_trame_char = strlen(trame_char);
			int longueur_generateur_char = strlen(generateur_char);
			int longueur_trame = longueur_trame_char + longueur_generateur_char - 1;

			int data[longueur_trame];
			int data1[longueur_trame];
			int generateur1[longueur_generateur_char];
			
			for(int i = 0; i < longueur_generateur_char; i++) {
				generateur1[i] = generateur_char[i] - '0';
			}

			for(int i = 0; i < longueur_trame; i++) {
				if(i < longueur_trame_char) {
					data1[i] = data[i] = trame_char[i] - '0';
				}

				else {
					data1[i] = data[i] = 0;
				}
			}

			printf("\n-> Votre trame en multipliant par le degré du générateur : ");
			for(int i = 0; i < longueur_trame; i++) {
				printf("%d", data1[i]);
			}
			printf("\n");

			crc(data1, generateur1, longueur_trame, longueur_trame_char, longueur_generateur_char);
			
			int reste[15];
			int i, j;

			for(i = 0, j = longueur_trame_char; i < longueur_generateur_char - 1; i++, j++) {
				reste[i] = data1[j];
			}

			printf("-> Reste de la division binaire par le générateur : ");
			for(int i = 0; i < longueur_generateur_char - 1; i++) {
				printf("%d", reste[i]); 
			}
			printf("\n");
			
			for(int i = 0; i < longueur_trame; i++) {
				if(i < longueur_trame_char) {
					data1[i] = data[i] = trame_char[i] - '0';
				}
			}

			printf("-> Trame finale qui sera envoyer au serveur : ");
			for(int i = 0; i < longueur_trame; i++) {
				printf("%d", data1[i]);
			}
			printf("\n");

			long k = 0;
			int n = sizeof(data1)/sizeof(data1[0]);
			for (int i = 0; i < n; i++)
				k = 10 * k + data1[i];

			sprintf(buff, "%ld", k);

			send(socketClient, buff, sizeof(buff), 0);
			recv(socketClient, buffer, sizeof(buffer), 0); 

			printf("\nServeur : %s\n", buffer);

			if(strncmp("ACK", buffer, 3) == 0) {
				printf("Le serveur a quitté...\n");
				exit(0); 
			}

			else {
				sleep(5);
				goto programme;
			}
	}
    shutdown(socketClient, 2);
}