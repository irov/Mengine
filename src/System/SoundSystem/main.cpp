//#include "vld.h"

#include <iostream>
//#include "interfaces.h"
//#include "SoundSystem.h"

#	pragma comment (lib, "Squalld.lib")

#include	<conio.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<time.h>
//SoundSystem.def
#include "windows.h"

#include <fstream>

//#include	"BaseSoundSource.h"

#include "SquallSoundSource.h"

class Listener : public SoundNodeListenerInterface
{
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
	SoundDataDesc	desc_to_load = {
		SoundDataDesc::OGG,
		"2.ogg",
		true
	};

	Listener* l = new Listener();

	SoundSourceInterface* ssi = SoundSystem->loadSoundNode(desc_to_load,l);
//	ssi->setLoop(true);
//	ssi->setPitch(1);
	ssi->play();

	//agetche();
	int key = 0;
	int u = 0;
	do
	{
		u++;
		if (kbhit())
			key = getch();

		if (u == 6000)
		{
			ssi->stop();
		}
		if (u == 28000)
		{
			ssi->play();
		}
		if (u == 30000)
		{
			ssi->pause();
		}
		if (u == 60000)
		{
			ssi->play();
		}
		if (u == 70000)
		{
			ssi->pause();
		}
		if (u == 72000)
		{
			ssi->play();
		}
		if (u == 73000)
		{
			ssi->stop();
		}
	} while (key != 27);

	//ssi->pause();
//	char ch = 't';
//	while(ch != '.')
//	{
//		std::cin>>ch;
		//getche();
//.	};
	//Sleep(2000); 
	SoundSystem->releaseSoundNode(ssi);

	delete l;
	releaseInterfaceSystem(SoundSystem);
};