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

	SoundSourceInterface* ssi = SoundSystem->loadSoundNode(desc_to_load,0);
	ssi->setLoop(true);
	ssi->setPitch(1);
	ssi->play();

	getche();

	SoundSystem->releaseSoundNode(ssi);

	delete[] buffer;

	releaseInterfaceSystem(SoundSystem);
};