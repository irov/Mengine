#	pragma comment (lib, "Squalld.lib")

#	include	<conio.h>
#	include	<stdio.h>
	
#	include	"SquallSoundSource.h"

class Listener : public SoundNodeListenerInterface
{
	void listenPaused(bool _pause)
	{
		printf("listernPaused\n");
	}
	void listenStopped()
	{
		printf("listenStopped\n");
	}
};

int main()
{
	SoundSystemInterface * SoundSystem = NULL;

	initInterfaceSystem(&SoundSystem);

	SoundBufferInterface * Sample = SoundSystem->createSoundBufferFromFile("Click.ogg",false);

	SoundNodeListenerInterface * listener = new Listener();

	SoundSourceInterface * Source = SoundSystem->createSoundSource(false,Sample,listener);
	
	Source->play();

	int key = 0;

	/* 
		Ошибка вот в чем:

		у меня последняя позиция и длинна которые "нормальные" это 
		12861 | 12863
		дальше идет неправильная позиция: 
		0 | 12863
		0 | 12863
		5 | 12863
		и т.д. до
		139 | 12863
		
		и звук перестает играть, вызывается callback на останов.
	*/

/*	do
	{
		float pos = Source->getPosMs();
		float len = Source->getLengthMS();
		printf("%f | %f \n",pos, len);

		if (kbhit())
		{
			key = getch();
		}
	} 
	while (key != 27);
*/
	SoundSystem->releaseSoundNode(Source);

	SoundSystem->releaseSoundBuffer(Sample);
	
	releaseInterfaceSystem(SoundSystem);
};