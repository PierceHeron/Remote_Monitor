#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#include <cgic.h>

#define IF_NAME "enp8s0"

int get_local_ip(char * ifname, char * ip)
{
	char *temp = NULL;
	int inet_sock;
	struct ifreq ifr;

	inet_sock = socket(AF_INET, SOCK_DGRAM, 0);

	memset(ifr.ifr_name, 0, sizeof(ifr.ifr_name));
	memcpy(ifr.ifr_name, ifname, strlen(ifname));

	if(ioctl(inet_sock, SIOCGIFADDR, &ifr)) {
		return -1;
	}

	temp = inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr);

	memcpy(ip, temp, strlen(temp));

	close(inet_sock);

	return 0;
}

int cgiMain() {
	char ip[32] = {0};

	get_local_ip(IF_NAME,ip);
	/* Send the content type, letting the browser know this is HTML */
	cgiHeaderContentType("text/html");
	fprintf(cgiOut,"<html>\n");

	//strcpy(data,getenv("QUERY_STRING"));
	//temp=strstr(data,"&");
	//*temp++ = '\0';
	//sscanf(data,"username=%s",username);
	//sscanf(temp,"password=%s",password);

	fprintf(cgiOut,"<html>\n");
	fprintf(cgiOut,"\t<head align=\"center\">\n");
	fprintf(cgiOut,"\t\t<meta charset=\"utf-8\">\n");
	fprintf(cgiOut,"\t\t<title>Remote Monitor System</title>\n");
	fprintf(cgiOut,"\t</head>\n");
	fprintf(cgiOut,"\t<body align=\"center\" bgcolor=\"#000000\">\n");
	fprintf(cgiOut,"\t\t<h1><font color=\"#FFFFFF\">Remote Monitor System</h1>\n");
	fprintf(cgiOut,"\t\t<img src=\"http://");
	fprintf(cgiOut,(const char *)ip);
	fprintf(cgiOut,":8080/?action=stream\" width=\"640\" height=\"480\"></img>\n");			
	fprintf(cgiOut,"<br>\n");

	if(!access("/dev/motor_drv",F_OK)) {
		fprintf(cgiOut,"\t\t<iframe src=\"motor.cgi?\" frameborder=\"no\" allowTransparency=\"true\" scrolling=\"no\"></iframe>\n");
	}

	if(!access("/dev/led_drv",F_OK)) {
		fprintf(cgiOut,"\t\t<iframe src=\"led.cgi?\" frameborder=\"no\" allowTransparency=\"true\" scrolling=\"no\"></iframe>\n");
	}

	fprintf(cgiOut,"\t</body>\n</html>\n");
	return 0;
}
