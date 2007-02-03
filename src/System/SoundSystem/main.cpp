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

#include "ALWavSound.h"

int main()
{

	SoundSystemInterface* SoundSystem;
	initInterfaceSystem(&SoundSystem);

	std::ifstream	f("09 - Out Of Vogue.ogg",std::ios::binary);

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
	ssi->setPitch(0.2);
	ssi->play();
	for(;;)
	{
		ssi->process();
	}

	getche();

	SoundSystem->releaseSoundNode(ssi);

	delete[] buffer;

	releaseInterfaceSystem(SoundSystem);


	/*SoundSystemInterface* SoundSystem;
	initInterfaceSystem(&SoundSystem);
 
	
	std::ifstream	f("1.ogg",std::ios::binary);

	f.seekg(0,std::ios::end); 
	size_t filesize = f.tellg();

	f.seekg(0,std::ios::beg);
	char* buffer = new char[filesize];
	f.read(buffer,filesize);

	f.close();

	SoundDataDesc	desc_to_load = {
		OGG,
		(void*)buffer,
		filesize,
		true
	};

	SoundSourceInterface* ssi = SoundSystem->loadSoundNode(desc_to_load,0);
	//ssi->setLooped(true);
	ssi->play();

	for (;;)
	{
		if(ssi->process()==false)
		{
			printf("! \n");
			break;
		}
//		ssi->stop();

	}
	
	getche();

	SoundSystem->releaseSoundNode(ssi);

	delete[] buffer;

f.open("2.ogg",std::ios::binary);

	f.seekg(0,std::ios::end); 
	filesize = f.tellg();

	f.seekg(0,std::ios::beg);
	buffer = new char[filesize];
	f.read(buffer,filesize);

	f.close();

	SoundDataDesc desc_to_load1 = {
		OGG,
		(void*)buffer,
		filesize,
		true
	};


	ssi = SoundSystem->loadSoundNode(desc_to_load1,0);
	
	ssi->play();

	for (;;)
	{
		if(!ssi->process())
		{
			printf("! \n");
			break;
		}
	}

	getche();

	SoundSystem->releaseSoundNode(ssi);

	delete[] buffer;

	releaseInterfaceSystem(SoundSystem);
*/
};