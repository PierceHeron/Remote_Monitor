#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "cgic.h"

#define 	UP		0
#define 	LEFT		1
#define		RESET		2
#define		RIGHT		3
#define		DOWN		4

int cgiMain() {
	char * data;
	int fd;
	int direction = 0;
	char write_buf[2] = {0};

	/* Send the content type, letting the browser know this is HTML */
	cgiHeaderContentType("text/html");

	fd = open("/dev/motor_drv", O_WRONLY);

	data = getenv("QUERY_STRING");

	if(sscanf(data,"Direction=%d",&direction))
	{
		switch(direction)
		{
			case	UP	: write_buf[0] = 0; write_buf[1] = 1; break;
			case	LEFT	: write_buf[0] = 1; write_buf[1] = 1; break;
			case	RESET	: write_buf[0] = 0; write_buf[1] = 0; 
					  write(fd, write_buf, 2); 
					  write_buf[0] = 1; break;
			case 	RIGHT	: write_buf[0] = 1; write_buf[1] = 2; break;
			case	DOWN	: write_buf[0] = 0; write_buf[1] = 2; break;
		}
		write(fd, write_buf, 2);
	}

	/* Top of the page */
	fprintf(cgiOut,"<html>\n");
	fprintf(cgiOut,"\t<head align=\"center\">\n");
	fprintf(cgiOut,"\t\t<meta charset=\"utf-8\">\n");
	fprintf(cgiOut,"\t</head>\n");
	fprintf(cgiOut,"\t<body align=\"center\" bgcolor=\"transparent\">\n");
	fprintf(cgiOut,"\t\t<form action=\"motor.cgi\" mothod=\"GET\" align=\"center\">\n");
	fprintf(cgiOut,"\t\t\t<button type=\"submit\" name=\"Direction\" value=0  style=\"background: transparent;border:0\"><img src=\"../icon/up.png\" width=40px height=40px></button><br>\n");
	fprintf(cgiOut,"\t\t\t<button type=\"submit\" name=\"Direction\" value=1  style=\"background: transparent;border:0\"><img src=\"../icon/left.png\" width=40px height=40px></button>\n");
	fprintf(cgiOut,"\t\t\t<button type=\"submit\" name=\"Direction\" value=2  style=\"background: transparent;border:0\"><img src=\"../icon/CrazyEye.ico\" width=40px height=40px></button>\n");
	fprintf(cgiOut,"\t\t\t<button type=\"submit\" name=\"Direction\" value=3  style=\"background: transparent;border:0\"><img src=\"../icon/right.png\" width=40px height=40px></button><br>\n");
	fprintf(cgiOut,"\t\t\t<button type=\"submit\" name=\"Direction\" value=4  style=\"background: transparent;border:0\"><img src=\"../icon/down.png\" width=42px height=42px></button>\n");
	fprintf(cgiOut,"\t\t</form>\n");
	fprintf(cgiOut,"\t</body>\n</html>\n");

	close(fd);

	return 0;
}
