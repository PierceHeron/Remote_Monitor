#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "cgic.h"

int cgiMain() {
	char * data;
	int led;
	int fd;
	char write_buf[2] = {0},read_buf[4] = {0};
	
	/* Send the content type, letting the browser know this is HTML */
	cgiHeaderContentType("text/html");

	fd = open("/dev/led_drv", O_RDWR);

	data = getenv("QUERY_STRING");

	if(!strncmp(data,"LED=",4) && sscanf(data,"LED=%d",&led))
	{
		read(fd, read_buf, 4);
		if(read_buf[led])
			write_buf[0] = 0;
		else
			write_buf[0] = 1;
		write_buf[1] = led;
		write(fd, write_buf, 2);
	}

	/* Top of the page */
	fprintf(cgiOut,"<html>\n");
	fprintf(cgiOut,"\t<head align=\"center\">\n");
	fprintf(cgiOut,"\t\t<meta charset=\"utf-8\">\n");
	fprintf(cgiOut,"\t</head>\n");
	fprintf(cgiOut,"\t<body align=\"center\" bgcolor=\"transparent\">\n");
	fprintf(cgiOut,"\t\t<font color=\"#FFFFFF\">\n");
	fprintf(cgiOut,"\t\t<form action=\"led.cgi\" mothod=\"GET\" align=\"center\"><br>\n");
	fprintf(cgiOut,"\t\t\t<table border=\"0\" align=\"center\" cellspacing=\"10\">\n");
	
	read(fd, read_buf, 4);
	
	if(read_buf[0]) {
		fprintf(cgiOut,"\t\t\t<tr><td><button type=\"submit\" name=\"LED\" value=\"0\"  style=\"background: #00FF00; border:0; width:60px; height:30px; color:#000000\">LED0</button></td>\n");
	} else {
		fprintf(cgiOut,"\t\t\t<tr><td><button type=\"submit\" name=\"LED\" value=\"0\"  style=\"background: #EF0000; border:0; width:60px; height:30px; color:#FFFFFF\">LED0</button></td>\n");
	}

	if(read_buf[1]) {
		fprintf(cgiOut,"\t\t\t<td><button type=\"submit\" name=\"LED\" value=\"1\"  style=\"background: #00FF00; border:0; width:60px; height:30px; color:#000000\">LED1</button></td></tr>\n");
	} else {
		fprintf(cgiOut,"\t\t\t<td><button type=\"submit\" name=\"LED\" value=\"1\"  style=\"background: #EF0000; border:0; width:60px; height:30px; color:#FFFFFF\">LED1</button></td></tr>\n");
	}
	
	if(read_buf[2]) {
		fprintf(cgiOut,"\t\t\t<tr><td><button type=\"submit\" name=\"LED\" value=\"2\"  style=\"background: #00FF00; border:0; width:60px; height:30px; color:#000000\">LED2</button></td>\n");
	} else {
		fprintf(cgiOut,"\t\t\t<tr><td><button type=\"submit\" name=\"LED\" value=\"2\"  style=\"background: #EF0000; border:0; width:60px; height:30px; color:#FFFFFF\">LED2</button></td>\n");
	}
	
	if(read_buf[3]) {
		fprintf(cgiOut,"\t\t\t<td><button type=\"submit\" name=\"LED\" value=\"3\"  style=\"background: #00FF00; border:0; width:60px; height:30px; color:#000000\">LED3</button></td></tr>\n");
	} else {
		fprintf(cgiOut,"\t\t\t<td><button type=\"submit\" name=\"LED\" value=\"3\"  style=\"background: #EF0000; border:0; width:60px; height:30px; color:#FFFFFF\">LED3</button></td></tr>\n");
	}
	fprintf(cgiOut,"\t\t\t</table>\n");
	fprintf(cgiOut,"\t\t</form>\n");
	fprintf(cgiOut,"\t</body>\n</html>\n");

	close(fd);
	return 0;
}
