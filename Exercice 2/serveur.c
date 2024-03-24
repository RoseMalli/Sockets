#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int somme(int * arr) {
	int somme_totale = 0; 
	int n = sizeof(arr) / sizeof(arr[0]);
	for (int i = 0; i < n; i++) {
		somme_totale = somme_totale + arr[i];
	}
	return somme_totale;
}

int main() {
    int socketServeur; 
	struct sockaddr_in AddrServeur;
	struct sockaddr_in AddrClient;
	char * buffer; 
	char buff[256];
	unsigned int AddrLen; 

	socketServeur = socket(AF_INET,SOCK_STREAM,0);
	if(socketServeur == -1){
		printf("La création de la socket a échouée\n");
		exit(0);
	}
	
	else {
		printf("La socket a été créée avec succès...\n");
	}

	AddrServeur.sin_family = AF_INET;
	AddrServeur.sin_addr.s_addr = htonl(INADDR_ANY);
	AddrServeur.sin_port = htons(PORT);
	
	if((bind(socketServeur, (struct sockaddr *)&AddrServeur, sizeof(AddrServeur))) != 0){
		printf("L'association de l'adresse a échouée...\n");
		exit(0);
	}
	
	else {
		printf("L'adresse a été associée à la socket..\n");
	}

	if(listen(socketServeur, 10) == -1){
		printf("Erreur lors du lancement de l'écoute \n");
		exit(0);
	}
	
	AddrLen = (unsigned int )sizeof(AddrClient);

	int socket_connectee = accept(socketServeur, (struct sockaddr*)&AddrClient, &AddrLen);
	if(socket_connectee == -1){
		printf("Erreur accept \n");
		exit(0);
	}

    while(1) {
        recv(socket_connectee, buff, sizeof(buff), 0);
		printf("\nClient : ");
		printf("%s\n", buff);

		char * trame_char = malloc(sizeof buff);
    	memcpy(trame_char, buff, sizeof buff); 

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

		crc(data1, generateur1, longueur_trame, longueur_trame_char, longueur_generateur_char);

		int reste[15];
		int i, j;

		for(i = 0, j = longueur_trame_char; i < longueur_generateur_char - 1; i++, j++) {
			reste[i] = data1[j];
		}

		printf("\n-> Reste de la division binaire par le générateur : ");
		for(i = 0; i < longueur_generateur_char - 1; i++) {
			printf("%d", reste[i]); 
		}
		printf("\n");

		int test = somme(reste);

		printf("-> La trame est-elle valide ? ");

		if(test == 0) {
			buffer = "ACK";
			printf("%s\n", buffer);
		}

		else {
			buffer = "NACK";
			printf("%s\n", buffer);
		}

		send(socket_connectee, buffer, sizeof(buffer),0);

		if(strncmp("ACK", buffer, 3) == 0) {
            printf("\nJ'ai quitté...\n");
            exit(0);
        }
    }
    shutdown(socketServeur, 2);
}