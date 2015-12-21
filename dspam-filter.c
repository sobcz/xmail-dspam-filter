#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define OutputCRLF

void fail(char *msg, int code) {
	printf("FAIL: %s\n", msg);
	exit(code);
}

int main(int iArgCount, char *pszArgs[]) {
	FILE *fdata, *fpipe, *ftemp;
	char line[4096];
	char command[1024];
	char tempfile[] = "/tmp/dsfilter_XXXXXX";
	char separator[] = "<<MAIL-DATA>>";
	long datapos;
	int header = 1;
	int len;
	int fd = -1;
	//
	if(iArgCount != 3) fail("usage: dspam-filter <recipicient> <message file>", 0);
	if((fdata = fopen(pszArgs[2], "r+")) == NULL) fail("Can't open input file", 0);
	if((fd = mkstemp(tempfile)) < 0) fail("Can't create mail temporary file", 0);
	if((ftemp = fdopen(fd, "w")) == NULL) {
		close(fd);
		unlink(tempfile);
		fail("Can't open mail temporary file", 0);
	}
	//
	while(fgets(line, sizeof(line), fdata)) {
		if(!header) {
			len = strlen(line);
			if(line[len - 2] == 0x0d) {
				line[len - 2] = 0x0a;
				line[len - 1] = 0x00;
			}
			fputs(line, ftemp);
		}
		if(strncmp(line, separator, sizeof(separator) - 1) == 0) {
			header = 0;
			datapos = ftell(fdata);
		}
	}
	fclose(ftemp);
	//
	sprintf(command, "dspam --user %s --client --deliver=innocent,spam --stdout < %s", pszArgs[1], tempfile);
	if(!(fpipe = (FILE *)popen(command, "r"))) {
		close(fd);
		unlink(tempfile);
		fail("Problem with pipe", 0);
	}
	//
	fseek(fdata, datapos, 0);
	while(fgets(line, sizeof(line) - 1, fpipe)) {
#ifdef OutputCRLF
		len = strlen(line);
		if(line[len - 2] != 0x0d && line[len - 1] == 0x0a) {
			line[len - 1] = 0x0d;
			line[len + 0] = 0x0a;
			line[len + 1] = 0x00;
		}
#endif
		fputs(line, fdata);
	}
	pclose(fpipe);
	close(fd);
	unlink(tempfile);
	//
	exit(7);
}
