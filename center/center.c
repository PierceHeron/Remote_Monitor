#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <cgic.h>

int cgiMain() {
	char * data;
	/* Send the content type, letting the browser know this is HTML */
	cgiHeaderContentType("text/html");

	data = getenv("QUERY_STRING");

	/* Top of the page */
	fprintf(cgiOut,"<html>\n");
	fprintf(cgiOut,"\t<head align=\"center\">\n");
	fprintf(cgiOut,"\t\t<meta charset=\"utf-8\">\n");
	fprintf(cgiOut,"\t\t<title>Camera Frame</title>\n");
	fprintf(cgiOut,"\t</head>\n");
	fprintf(cgiOut,"\t<body align=\"center\" bgcolor=\"transparent\">\n");
	fprintf(cgiOut,"\t\t<img src=\"");
	fprintf(cgiOut,(const char *)data);
	fprintf(cgiOut,":8080/?action=stream\" width=\"640\" height=\"480\"></img>\n");
	fprintf(cgiOut,"\t\t<form action=\"");
	fprintf(cgiOut,(const char *)data);		
	fprintf(cgiOut,"/RemoteMonitor.html\" mothod=\"GET\" align=\"center\"><br>\n");
	fprintf(cgiOut,"\t\t\t<button type=\"submit\" name=\"machine\" value=\"0\">Enter</button>\n");
	fprintf(cgiOut,"\t\t</form>\n");
	fprintf(cgiOut,"\t</body>\n</html>\n");
	return 0;
}
