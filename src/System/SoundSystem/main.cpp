#include "..\\..\\..\\dependencies\\vld\\vld.h"

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
	void listenPaused(bool _pause)
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
		false,
		true
	};

	Listener* l = new Listener();

	SoundSourceInterface* ssi = SoundSystem->loadSoundNode(desc_to_load,l);
	
	
	ssi->play();
	ssi->setVolume(0.9);

	int key = 0;
	int u = 0;
	do
	{
		u++;
		if (kbhit())
			key = getch();
	} while (key != 27);

	SoundSystem->releaseSoundNode(ssi);
	ssi = NULL;
	ssi = SoundSystem->loadSoundNode(desc_to_load,l);

	ssi->play();
/*	key = 0;
	do
	{
		u++;
		if (kbhit())
			key = getch();
	} while (key != 27);
*/
	u = 0;
	key = 0;
	do
	{
		u++;
		if (kbhit())
			key = getch();

		if (u == 600)
		{
			ssi->stop();
		}
		if (u == 2800+5000)
		{
			ssi->play();
		}
		if (u == 3000+10000)
		{
			ssi->stop();
		}
		if (u == 4000+15000)
		{
			ssi->play();
		}
		if (u == 5000+20000)
		{
			ssi->pause();
		}
		if (u == 7200+30000)
		{
			ssi->play();
		}
		if (u == 9300+40000)
		{
			ssi->stop();
		}
		const float * p=  ssi->getPosition();
	} while (key != 27);


//	getche();
	SoundSystem->releaseSoundNode(ssi);
	//getche();
	releaseInterfaceSystem(SoundSystem);

	delete l;
};