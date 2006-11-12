#	pragma once

/*	
	��� �������� ������.
	�� ������ eSoundType ���������� �������� ������ ��� ��������.
*/
enum SOUND_TYPE	 
{
	OGG,
	WAV,
	MP3,
};

/*
	�������� �������� ������.
	- ��� �������� ������
	- ������
	- ������ �������
	- ���. �� ���������.
*/
struct	SoundDataDesc
{
	SOUND_TYPE	type;
	void*		buffer;
	size_t		size;
	bool		isStreamSound;
};

class	SoundSourceInterface;

/*
	��������� � �������.
*/
class	SoundNodeListenerInterface
{
public:
	virtual bool listenRecycled(SoundSourceInterface*	_sn) = 0;
	virtual void listenStoped(SoundSourceInterface*	_sn) = 0;
};

/*
	�������� �����.
	����������� �������� SoundSystemInterface::loadSoundNode � ��������� SoundDataDesc
	� ���������.
	�������� ������� ��� ��������� � �.�.
	������� update ���������� true, ���� �������� ����� ������ �� �����.
*/

class	SoundSourceInterface
{
public:
	virtual void			play() = 0; 
	virtual void			pause() = 0;
	virtual void			stop() = 0;
	virtual	bool			updateSoundBuffer() = 0;
	virtual void			setLooped(bool _flag) = 0;
	virtual bool			getLooped() const = 0;
	virtual void			setVolume(float _value) = 0;
	virtual float			getVolume() const = 0;
	virtual void			setPosition(const float* _position) = 0;
	virtual const float*	getPosition() const = 0;
	virtual void			setDistance(float _dist) = 0;
	virtual float			getDistance() const = 0;
	virtual void			setHeadMode(bool _flag) = 0;
	virtual bool			getHeadMode() const = 0;
};

/*
	������� �����.
		- ������������� ��������� ���������.
		- ��������� �������� �����.
		- ������� �������� �����.
*/
class SoundSystemInterface
{
public:
	virtual void						setListenerOrient(const float* _position, const float* _updir) = 0;
	virtual SoundSourceInterface*		loadSoundNode(const SoundDataDesc& _desc, SoundNodeListenerInterface*	_listener = 0) = 0;
	virtual void						releaseSoundNode(SoundSourceInterface* _sn) = 0;
};

bool	initSoundSystem(SoundSystemInterface** );
void	releaseSoundSystem(SoundSystemInterface* );
