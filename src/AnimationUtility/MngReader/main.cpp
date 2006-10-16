#include "MNG.h"

using namespace MengeLib;

int main()
{
	FILE*	fp = fopen("output1.mng","rb");
	fseek(fp, 0, SEEK_END); 
	unsigned int length=ftell(fp);
	fseek(fp, 0, SEEK_SET);
	unsigned char* buffer = new unsigned char[length];
	fread(buffer,1,length,fp);
	fclose(fp);
	mngDesc	desc;
	readMNG(desc,buffer,length);
	freeMNG(desc);
	delete buffer;
};