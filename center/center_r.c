/*************************************************************************
 * @Author                :  haidee
 * @Date                  :  2019-05-22T11:14:23+08:00
 * @Last modified by      :  haidee
 * @Last modified time    :  2019-05-22T11:47:42+08:00
 ************************************************************************/



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <cgic.h>
#include <mysql/mysql.h>

int cgiMain() {
	MYSQL mysql;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char * query = NULL;

	char * temp = NULL;
	int i = 0;
	char data[100] = {0};

	char username[100] = {0};
	char password[20]= {0};

	mysql_init(&mysql);

	/* Send the content type, letting the browser know this is HTML */
	cgiHeaderContentType("text/html");
	fprintf(cgiOut,"<html><head>\n");

	if (!mysql_real_connect(&mysql,"localhost","root","970317","remote_monitor",0,NULL,0)) {
		fprintf(cgiOut,"<meta http-equiv=\"refresh\" content=\"0; url=../Login/index.html\" >\n");
		fprintf(cgiOut,"<script>alert(\"Center Error: Connect DB Failed\")</script>\n");
		fprintf(cgiOut,"</head></html>\n");
		return 0;
	}

	strcpy(data,getenv("QUERY_STRING"));
	temp=strstr(data,"&");
	*temp++ = '\0';
	sscanf(data,"username=%s",username);
	sscanf(temp,"password=%s",password);

	if (!strlen(username) || !strlen(password)) {
		fprintf(cgiOut,"<meta http-equiv=\"refresh\" content=\"0; url=../Login/index.html\" >\n");
		fprintf(cgiOut,"<script>alert(\"Username or password should be not null!\")</script>\n");
		fprintf(cgiOut,"</head></html>\n");
		return 0;
	}

	query = "select * from user_db";

	if(mysql_real_query(&mysql,query,(unsigned int)strlen(query))) {
		fprintf(cgiOut,"<meta http-equiv=\"refresh\" content=\"0; url=../Login/index.html\" >\n");
		fprintf(cgiOut,"<script>alert(\"Center Error : Query DB failed!\")</script>\n");
		fprintf(cgiOut,"</head></html>\n");
		return 0;
	}

	res = mysql_store_result(&mysql);

	while(row = mysql_fetch_row(res)) {
		if(!strcmp(username,row[1]) && !strcmp(password,row[2])) {
			break;
		}
	}

	if(!row) {
		fprintf(cgiOut,"<meta http-equiv=\"refresh\" content=\"0; url=../Login/index.html\" >\n");
		fprintf(cgiOut,"<script>alert(\"Username or password error!\")</script>\n");
		fprintf(cgiOut,"</head></html>\n");
		return 0;
	}

	query = "select * from machine_db";

	if(mysql_real_query(&mysql,query,(unsigned int)strlen(query))) {
		fprintf(cgiOut,"<meta http-equiv=\"refresh\" content=\"0; url=../Login/index.html\" >\n");
		fprintf(cgiOut,"<script>alert(\"Center Error : Query DB failed!\")</script>\n");
		fprintf(cgiOut,"</head></html>\n");
		return 0;
	}

	res = mysql_store_result(&mysql);

	/* Top of the page */
	fprintf(cgiOut,"\t<head align=\"center\">\n");
	fprintf(cgiOut,"\t\t<meta charset=\"utf-8\">\n");
	fprintf(cgiOut,"\t\t<title>Remote Monitor System Center</title>\n");
	fprintf(cgiOut,"<script>alert(\"Login Success!\")</script>\n");
	fprintf(cgiOut,"\t</head>\n");
	fprintf(cgiOut,"\t<body align=\"center\" bgcolor=\"#000000\">\n");
	fprintf(cgiOut,"\t\t<font color=\"#FFFFFF\">\n");
	fprintf(cgiOut,"\t\t<h1>Remote Monitor System Center</h1>\n");
	fprintf(cgiOut,"\t\t<table border=\"0\" align=\"center\" cellspacing=\"10\"><tr>\n");

	while(row = mysql_fetch_row(res)) {
		i++;
		fprintf(cgiOut,"\t\t\t<td><form action=\"../server");
		fprintf(cgiOut,"%d",i);
		fprintf(cgiOut,"/cgi-bin/monitor_r.cgi\" mothod=\"GET\" align=\"center\">\n");
		fprintf(cgiOut,"\t\t\t\t<input name=\"image\" type=\"image\" src=\"../server");
		fprintf(cgiOut,"%d",i);
		fprintf(cgiOut,"_mjpg/?action=stream\" width=\"640\" height=\"480\"></img>\n");
		fprintf(cgiOut,"\t\t\t\t<br><h3>machine IP:");
		fprintf(cgiOut,row[1]);
		fprintf(cgiOut,"</h3>\n");
		fprintf(cgiOut,"\t\t\t</form></td>\n");
	}
	fprintf(cgiOut,"\t\t</tr></table>\n");
	fprintf(cgiOut,"\t</body>\n</html>\n");
	return 0;
}
