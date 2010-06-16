#include <stdio.h>
#include <windows.h>
#include "BinParser.h"
#include "BinParsListener.h"
#include "BinStreamReader.h"

//////////////////////////////////////////////////////////////////////////
void main(int argc, char* argv[])
{
	BinParsListener listener;
	BinStreamReader reader(argv[1]);
	BinParser parser(&reader, &listener);

	system("pause");
}