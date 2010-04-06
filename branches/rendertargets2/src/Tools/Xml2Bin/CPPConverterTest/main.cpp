#include <stdio.h>
#include <windows.h>

typedef void(WINAPI *PFN_Initialize)(char *);
typedef void(WINAPI *PFN_Convert)(char *, char*);

void main(int argc, char* argv[])
{
	HINSTANCE hMyDll;
	hMyDll = ::LoadLibraryA("Xml2Bin.dll");

	PFN_Initialize p_Initialize;
	PFN_Convert p_Convert;
	p_Initialize = (PFN_Initialize)::GetProcAddress(hMyDll, "initialize");
	p_Convert = (PFN_Convert)::GetProcAddress(hMyDll, "convert");
	p_Initialize(argv[1]);
	p_Convert(argv[2], argv[3]);

	::FreeLibrary(hMyDll); 
	system("pause");
}