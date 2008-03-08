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
	bool setBlow( bool _active );
	float getBlow();

public:
	void update( float _timing );

protected:
	bool m_active;
	bool m_initialize;

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
	ALchar * m_buffer;
};