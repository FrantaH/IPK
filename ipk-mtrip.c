#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <signal.h>
#include <math.h>



#define MIN(a, b)       (((a) < (b)) ? (a) : (b))
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))
static int sock;

void closeServer()
{
	printf("losing socket and server\n"); //closing :D ale když tam je to ^C tak to hezky doplní
	close(sock);
	exit(0);
}

float difftimespec(struct timespec *ts1, struct timespec *ts2)
{
   return difftime(ts1->tv_sec, ts2->tv_sec) + (ts1->tv_nsec - ts2->tv_nsec)*1e-9;
}


int main(int argc , char *argv[])
{
	
	int opt = 0;
	char buff[65526];		//docasny buffer s max velikosti posilanou pomoci UDP
	
	if (argc == 4)
	{
		int port;
		
		if (!strcmp(argv[1],"reflect"))
		{
			while ((opt = getopt(argc, argv, "p:")) != -1) {
				   switch (opt) {
					   
				   case 'p':
					   port = atoi(optarg);
					   break;

				   default: /* '?' */
					   fprintf(stderr, "neco je spatne s %s\n", argv[0]);
					   exit(EXIT_FAILURE);
				   }
			}
			
			
			/** REFLEKTOR **/
			printf("port=%d\n",port);
		
			socklen_t clientSize;
			long long int tmp;
			struct timeval tm;
			struct sockaddr_in refl, metr;
			unsigned long long int prijato;
			
			memset(&refl, 0, sizeof(refl));
			refl.sin_family = AF_INET;
			refl.sin_addr.s_addr = htonl(INADDR_ANY);
			refl.sin_port = htons(port);
			
			sock = socket(AF_INET, SOCK_DGRAM,0);
			printf("socket vytvoren\n");
			
			if(bind(sock, (struct sockaddr *)&refl, sizeof(refl))<0)
			{
				fprintf(stderr, "bind selhal\n");
				exit(EXIT_FAILURE);
			} 
			printf("socket bindnut\n");
			signal(SIGINT, closeServer);
			
			while(1)
			{
				clientSize = sizeof(metr);
				tm.tv_sec=0;
				tm.tv_usec=0;
				tmp = 0;
				prijato = 0;
				setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tm, sizeof(tm));
				printf("cekam na ping\n");
				tmp = recvfrom(sock, &buff, 2,0,(struct sockaddr *)&metr,&clientSize);
				
				if (tmp==2 && buff[1]=='R')
				{
					printf("dosla zprava o rtt\n");
					buff[1]='B';
					sendto(sock, &buff, 2, MSG_DONTWAIT, (struct sockaddr *)&metr,clientSize);
				} else 
					printf("dosla zprava o rtt SPATNE \n");
				
				tm.tv_sec=0;
				tm.tv_usec=500000;
				setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tm, sizeof(tm));
				
				while (1)
				{
					clientSize = sizeof(metr);
					if ((tmp = recvfrom(sock, &buff, sizeof(buff),0,(struct sockaddr *)&metr,&clientSize))<0)
					{
						break;
					} 
					prijato += tmp;
				}
				sendto(sock, &prijato, sizeof(prijato), MSG_DONTWAIT, (struct sockaddr *)&metr,clientSize);
				
			}
			/***************KONIEC**********************/
			
			
			
			
		} else {
			fprintf(stderr, "spatny prvni argument \n");
            exit(EXIT_FAILURE);
		}
	} else if (argc == 10)
	{
		int port, checkTime, sondSize;
		char* host;
		
		if (!strcmp(argv[1],"meter"))
		{
			
			while ((opt = getopt(argc, argv, "h:p:s:t:")) != -1) {
                switch (opt) {
                case 'h':
                    host = optarg;
                    break;
                case 'p':
                    port = atoi(optarg);
                    break;
                case 's':
                    sondSize = atoi(optarg);
                    break;
                case 't':
                    checkTime = atoi(optarg);
                    break;

                default: /* '?' */
                    fprintf(stderr, "neco je spatne s %s\n", argv[0]);
                    exit(EXIT_FAILURE);
                }
			}
			
			
			
			/** METR **/
			printf("host=%s\nport=%d\nvelikost sondy=%d\ncas=%d\n",host, port, sondSize, checkTime);
			//printf("%d\n",htons(host));
			if (checkTime<1)
			{
				fprintf(stderr, "doba mereni musi byt alespon 1\n");
				exit(EXIT_FAILURE);
			}
			if (sondSize>65526)
			{
				fprintf(stderr, "velikost sondy je moc velka\n");
				exit(EXIT_FAILURE);
			}
			if (checkTime>15)
			{
				fprintf(stderr, "cas do zitra nemame :P zadej cas pod 15\n");
				exit(EXIT_FAILURE);
			}
			
			int tmp;
			struct timespec ts1, ts2;
			struct timeval tm;
			struct hostent *hostent;
			struct sockaddr_in reflec;
			double rttcas = 0.0;
			double celkovaodezva = 0.0;
			unsigned long long int preneseno = 0;
			unsigned long long int mereni;
			unsigned long long int odchylkaPole[20];
			double max = 0.0;
			double min = INFINITY;
			
			tm.tv_sec = 2;
			tm.tv_usec = 0;
			printf("nastavuji socket\n");
			
			sock = socket(AF_INET, SOCK_DGRAM, 0);
			setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tm, sizeof(tm));
			
			reflec.sin_family = AF_INET;
			reflec.sin_port = htons(port);
			hostent = gethostbyname(host);
			memcpy(&(reflec.sin_addr.s_addr), hostent->h_addr, hostent->h_length);
			
			printf("vstupuji do cyklu!\n");
			
			for (int i=1; i<checkTime+1;i++)
			{
				clock_gettime(CLOCK_MONOTONIC, &ts1);
				buff[1]='R';
				tmp = sendto(sock, &buff, 2, MSG_DONTWAIT, (struct sockaddr *)&reflec,sizeof(reflec));
				if (tmp<0)
					printf("chyba pri send\n");
				
				printf("cekam na odpoved\n");
				tmp = recv(sock, &buff, sizeof(buff), 0);
				if (tmp==2 && buff[1]=='B')
				{
					rttcas=0.0;
					clock_gettime(CLOCK_MONOTONIC, &ts2);
					rttcas = difftimespec(&ts2, &ts1);
				} else {
					fprintf(stderr, "chyba pri mereni RTT (reflektor neodpovida)\n");
					exit(EXIT_FAILURE);
				}
				
				clock_gettime(CLOCK_MONOTONIC, &ts1);
				while (difftimespec(&ts2, &ts1)<1)
				{
					sendto(sock, &buff, sondSize, MSG_DONTWAIT, (struct sockaddr *)&reflec,sizeof(reflec));
					clock_gettime(CLOCK_MONOTONIC, &ts2);
				}
				mereni = 0;
				tmp = recv(sock, &mereni, sizeof(mereni), 0);
				if (tmp!=sizeof(mereni))
				{
					fprintf(stderr, "spatne data :( \n");
					exit(EXIT_FAILURE);
				}
				preneseno += mereni;
				odchylkaPole[i]=mereni;
				celkovaodezva += rttcas;
				printf("RTT = %f\nrychlost=%llu B/s\n\n",rttcas,mereni);
				max = MAX(max,mereni);
				min = MIN(min,mereni);
			}
			
			
			double odchylka;
			double prenesenoFloat;
			if (celkovaodezva==0.0)
			{
				fprintf(stderr, "chyba pri mereni RTT (reflektor neodpovida)\n");
				exit(EXIT_FAILURE);
			} else
			{
				celkovaodezva = celkovaodezva/checkTime;
				printf("\nprumerne RTT = %f\n",celkovaodezva);
				printf("minimalni rychlost = %f MiB/s\n",(min/1024/1024));
				printf("maximalni rychlost = %f MiB/s\n",(max/1024/1024));
				
				for (int i = 1;i<checkTime+1;i++)
				{
					odchylka += (odchylkaPole[i] - (preneseno/checkTime)) * (odchylkaPole[i] - (preneseno/checkTime));
				}
				odchylka = sqrt((odchylka/checkTime));
				prenesenoFloat = preneseno/1024/1024/checkTime;
				printf("prumerna rychlost = %f MiB/s\n",prenesenoFloat);
				printf("odchylka = %f MiB/s\n",odchylka/1024/1024);
			}
			
			close(sock);
			/******************************/
			
		} else {
			fprintf(stderr, "spatny prvni argument \n");
            exit(EXIT_FAILURE);
		}
	} else {
		fprintf(stderr, "spatny pocet argumentu \n");
                    exit(EXIT_FAILURE);
	}
	
	return 0;
}
