#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

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
	int socketClient; 
	struct sockaddr_in AddrServeur; 
	unsigned int AddrLen; 
	long n;

	socketClient = socket(AF_INET, SOCK_DGRAM, 0);

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

    while(1) {
		programme :
			printf("\nSaisissez votre trame (16 bits max) : ");
			scanf("%ld", &n);

			int i = somme(n);

			if(i % 2 == 0) {
				n = n * 10;
				printf("\n-> Votre trame après le bit de parité paire : %ld\n", n);
			}

			else {
				n = (n * 10) + 1;
				printf("\n-> Votre trame après le bit de parité paire : %ld\n", n); 
			}

			sprintf(buffer,"%ld",n);
			
			sendto(socketClient, buffer, sizeof(buffer), 0, (struct sockaddr *)&AddrServeur, AddrLen);
			recvfrom(socketClient, buffer, sizeof(buffer), 0, (struct sockaddr *)&AddrServeur, &AddrLen); 

			if(strncmp("ACK", buffer, 3) == 0) {
				printf("\nServeur : %s\n", buffer);
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