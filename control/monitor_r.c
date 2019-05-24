/*************************************************************************
 * @Author                :  haidee
 * @Date                  :  2019-05-22T11:45:16+08:00
 * @Last modified by      :  haidee
 * @Last modified time    :  2019-05-22T11:46:18+08:00
 ************************************************************************/



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#include <cgic.h>

#define SERVER_IP "192.168.43.45"



int cgiMain() {
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
	fprintf(cgiOut,SERVER_IP);
	fprintf(cgiOut,"/server2_mjpg/?action=stream\" width=\"640\" height=\"480\"></img>\n");			
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
