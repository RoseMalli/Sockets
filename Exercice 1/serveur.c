#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define PORT 8080

int somme(long n) {
    int somme_totale = 0;
	int m; 
    while(n > 0) {    
    	m = n % 10;    
        somme_totale = somme_totale + m;    
        n = n / 10;    
    } 
    return somme_totale;  
}
        
int main() {
	char buffer[25];
	char * buf; 
    int socketServeur; 
	struct sockaddr_in AddrServeur;
	struct sockaddr_in AddrClient;
	unsigned int AddrLen; 
	long n;

	socketServeur = socket(AF_INET, SOCK_DGRAM, 0);

	if(socketServeur == -1) {
		printf("La création de la socket a échouée\n");
		exit(0);
	}
	
	else {
		printf("La socket a été créée avec succès...\n");
	}

	AddrServeur.sin_family = AF_INET;
	AddrServeur.sin_addr.s_addr = htonl(INADDR_ANY);
	AddrServeur.sin_port = htons(PORT);

	
	if((bind(socketServeur, (struct sockaddr *)&AddrServeur, sizeof(AddrServeur))) != 0) {
		printf("L'association de l'adresse a échouée...\n");
		exit(0);
	}

	else {
		printf("L'adresse a été associée à la socket..\n");
	}
	
	AddrLen = (unsigned int)sizeof(AddrClient);

    while(1) {
		recvfrom(socketServeur, buffer, sizeof(buffer), 0, (struct sockaddr *)&AddrClient, &AddrLen);
		printf("\nClient : %s\n", buffer);

		n = atol(buffer);

		int i = somme(n);

		printf("\n-> La trame est-elle valide ? ");

        if(i % 2 == 0) {
            buf = "ACK";
            printf("%s\n", buf);
        }

		else {
			buf = "NACK";
			printf("%s\n", buf);
		}

        sendto(socketServeur, buf, sizeof(buf),0, (struct sockaddr *)&AddrClient, AddrLen);

        if(strncmp("ACK", buf, 3) == 0) {
            printf("\nJ'ai quitté...\n");
            exit(0);
        }
    }

    shutdown(socketServeur, 2);
}