#include "vld.h"

#include <iostream>
//#include "interfaces.h"
//#include "SoundSystem.h"

#include	<conio.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<time.h>
//SoundSystem.def
#include "windows.h"

#include <fstream>

//#include	"BaseSoundSource.h"

#include "FSLSoundSource.h"

class Listener : public SoundNodeListenerInterface
{
	bool listenRecycled()
	{
		return true;
	}
	void listenPaused()
	{
			printf("listernPuased\n");

	}
	void listenStopped()
	{
		printf("listenStopped\n");
	}
};

int main()
{
	SoundSystemInterface* SoundSystem;
	initInterfaceSystem(&SoundSystem);

	std::ifstream	f("test.ogg",std::ios::binary);

	f.seekg(0,std::ios::end); 
	size_t filesize = f.tellg();

	f.seekg(0,std::ios::beg);
	char* buffer = new char[filesize];
	f.read(buffer,filesize);

	f.close();

	SoundDataDesc	desc_to_load = {
		SoundDataDesc::OGG,
		(void*)buffer,
		filesize,
		true
	};

	Listener* l = new Listener();

	SoundSourceInterface* ssi = SoundSystem->loadSoundNode(desc_to_load,l);
//	ssi->setLoop(true);
//	ssi->setPitch(1);
	ssi->play();

	getche();
//	char ch = 't';
//	while(ch != '.')
//	{
//		std::cin>>ch;
		//getche();
//.	};
	//Sleep(2000); 
	SoundSystem->releaseSoundNode(ssi);

	delete[] buffer;
	delete l;
	releaseInterfaceSystem(SoundSystem);
};