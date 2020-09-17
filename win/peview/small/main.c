#include <stdio.h>
#include <stdlib.h>


char fileName[256] = {0};
int  fileNum = 0;
int  fileSize = 1024*1024;

#define BaseFileName "%i-%s"	//begin from 1;
#define MANHELP "-m file(merge files)\n -p file(apart file) -s size(KB) -n num\n"
int main(int argc, char *argv[])
{
	int i = 0, bMerge = 0;
	if(argc < 3)
	{
		printf("%s %s\n",argv[0],MANHELP);
		return 0;
	}
	i = 1;
	while(i < argc)
	{
		if(0 == strcmp(argv[i],"-m"))
		{
			bMerge = 1;
			strcpy(fileName,argv[i+1]);
			i++;
		}
		else if(0 == strcmp(argv[i],"-p"))
		{
			bMerge = 0;
			strcpy(fileName,argv[i+1]);
			i++;
		}
		else if(0 == strcmp(argv[i],"-s"))
		{
			fileSize = atoi(argv[i+1]);
			fileSize *= 1024;
			i++;
			fileNum = 0;
		}
		else if(0 == strcmp(argv[i],"-n"))
		{
			fileNum = atoi(argv[i+1]);
			i++;
			fileSize = 0;
		}
		else
		{
			printf("unsupport option %s\n",argv[i]);
			return 0;
		}
		i++;
	}
	if(bMerge)
		mergeFile();
	else
		apartFile();
}

void mergeFile()
{
	int i = 0, len = 0, tLen = 0, bLen = 0;
        char name[128] = {0}, buff[8192] = {0};
        FILE *fp = NULL, *ifp = NULL;

        fp = fopen(fileName,"wb");
        if(!fp)
        {
                printf("fopen %s failed\n",fileName);
                return;
        }
	i = 1;
	while(1)
        {
                if(!ifp)
                {
                        sprintf(name,BaseFileName,i,fileName);
                        ifp = fopen(name,"rb");
                }
		if(!ifp)
		{
			printf("%s input finish last none\n",name);
			break;
		}
                bLen = sizeof(buff);
                bLen = fread(buff,1,bLen,ifp);
                if(bLen <= 0)
                {
			printf("%s input finish\n",name);
			fclose(ifp);
			ifp = NULL;
			i++;
			continue;
		}
                if(bLen != fwrite(buff,1,bLen,fp))
                {
                        printf("fwrite error\n");
                        break;
                }
	}
	if(ifp)
		fclose(ifp);
	if(fp)
		fclose(fp);
}

void apartFile()
{
	int i = 0, len = 0, tLen = 0, bLen = 0;
	char name[128] = {0}, buff[8192] = {0};
	FILE *fp = NULL, *ofp = NULL;
	
	fp = fopen(fileName,"rb");
	if(!fp)
	{
		printf("fopen %s failed\n",fileName);
		return;
	}
	fseek(fp,0,SEEK_END);
	tLen = ftell(fp);
	fseek(fp,0,SEEK_SET);
	if(fileNum)
		len = tLen/fileNum;
	else if(fileSize)
		len = fileSize;
	else
		len = 1024*1024;
	
	tLen = 0, i = 1;
	while(1)
	{
		if(!ofp)
		{
			sprintf(name,BaseFileName,i,fileName);
			ofp = fopen(name,"wb");
			tLen = 0;
		}
		if(!ofp)
			break;
		bLen = (len >= tLen + sizeof(buff)) ? sizeof(buff):(len - tLen);
		bLen = fread(buff,1,bLen,fp);
		if(bLen <= 0)
		{
			printf("%s output finished last\n",name);
			break;
		}
		if(bLen != fwrite(buff,1,bLen,ofp))
		{
			printf("fwrite error\n");
			break;
		}
		tLen += bLen;
		if(tLen >= len)
		{
			fclose(ofp);
			ofp = NULL;
			i++;
			printf("%s output finished\n",name);
		}
	}
	
	if(ofp)
		fclose(ofp);
	if(fp)
		fclose(fp);
}
