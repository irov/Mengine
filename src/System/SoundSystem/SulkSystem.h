#	pragma once

#	include "al.h"
#	include "alc.h"

class SulkSystem
{
public:
	SulkSystem( float _enoughBlow );
	~SulkSystem();

public:
	bool initialize();

public:
	void update( float _timing );
	float getBlow();

protected:
	float m_enoughBlow;
	float m_dispersion;
	float m_average;

	ALCcontext * m_pContext;
	ALCdevice * m_pDevice;
	const ALCchar * m_szDefaultCaptureDevice;
	ALCdevice * m_pCaptureDevice;

	ALCint m_iSamplesAvailable;
	ALCsizei m_bufferSize;
	ALCsizei m_blockAlign;
	ALCint * m_buffer;
};