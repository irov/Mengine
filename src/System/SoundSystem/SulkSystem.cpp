#include "SulkSystem.h"
#include "../../Interface/SoundSystemInterface.h"

#include <math.h>

//////////////////////////////////////////////////////////////////////////
SulkSystem::SulkSystem()
	: m_enoughBlow( 0.f )
	, m_dispersion( 0.f )
	, m_average( 0.f )
	, m_pContext(0)
	, m_pDevice(0)
	, m_szDefaultCaptureDevice(0)
	, m_pCaptureDevice(0)
	, m_iSamplesAvailable(0)
	, m_bufferSize(4410)
	, m_blockAlign(2)
	, m_sulkcallback(0)
	, m_active(false)
	, m_initialize(false)
{
	m_buffer = new ALchar [ m_bufferSize ];
}
//////////////////////////////////////////////////////////////////////////
SulkSystem::~SulkSystem()
{
	// Stop capture
	alcCaptureStop( m_pCaptureDevice );

	// Close the Capture Device
	alcCaptureCloseDevice( m_pCaptureDevice );

	delete [] m_buffer;
}
//////////////////////////////////////////////////////////////////////////
bool SulkSystem::initialize()
{
	// Check for Capture Extension support
	m_pContext = alcGetCurrentContext();
	m_pDevice = alcGetContextsDevice( m_pContext );
	if ( alcIsExtensionPresent( m_pDevice, "ALC_EXT_CAPTURE" ) == ALC_FALSE )
	{
		// TODO : throw error here!
	}

	// Get the name of the 'default' capture device
	m_szDefaultCaptureDevice = alcGetString( 0, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);

	// Open the default Capture device to record a 22050Hz 16bit Mono Stream using an internal buffer
	// of BUFFERSIZE Samples (== BUFFERSIZE * 2 bytes)
	m_pCaptureDevice = alcCaptureOpenDevice( m_szDefaultCaptureDevice, 22050, AL_FORMAT_MONO16, 22050 );
	if ( !m_pCaptureDevice )
	{
		return false;
	}

	// Start audio capture
	alcCaptureStart( m_pCaptureDevice );

	return true;
}
//////////////////////////////////////////////////////////////////////////
void SulkSystem::setEnoughBlow( float _enoughBlow )
{
	m_enoughBlow = _enoughBlow;
}
//////////////////////////////////////////////////////////////////////////
float SulkSystem::getEnoughBlow() const
{
	return m_enoughBlow;
}
//////////////////////////////////////////////////////////////////////////
void SulkSystem::update()
{
	if( m_active == false )
	{
		return;
	}

	// Find out how many samples have been captured
	alcGetIntegerv( m_pCaptureDevice, ALC_CAPTURE_SAMPLES, 1, &m_iSamplesAvailable);

	// When we have enough data to fill our BUFFERSIZE byte buffer, grab the samples
	if ( m_iSamplesAvailable > (m_bufferSize / m_blockAlign) )
	{
		// Consume Samples
		alcCaptureSamples( m_pCaptureDevice, m_buffer, m_bufferSize / m_blockAlign );

		// find average
		float average = 0.f;
		for (int x = 0; x < m_bufferSize; ++x)
		{
			average += (int)m_buffer[x];
		}
		average /= m_bufferSize;

		m_average = average;

		// find dispersion
		float disp = 0.f;
		for (int x = 0; x < m_bufferSize - 1; ++x)
		{
			disp += powf( (int)m_buffer[x] - average, 2 );
			//disp += powf( (int)Buffer_[x] - (int)Buffer_[x+1], 2 );
		}
		disp /= m_bufferSize;

		m_dispersion = sqrtf(disp);
	}

	if( m_sulkcallback )
	{
		float blow = getBlow();

		if( blow > 0.f )
		{	
			m_sulkcallback->blow( blow );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void SulkSystem::setCallback( Menge::SoundSulkCallbackInterface * _callback )
{
	m_sulkcallback = _callback;
}
//////////////////////////////////////////////////////////////////////////
bool SulkSystem::activate( bool _activate )
{
	if( _activate == true && m_initialize == false )
	{
		m_active = initialize();
		return m_active;
	}

	m_active = _activate;
	return true;
}
//////////////////////////////////////////////////////////////////////////
float SulkSystem::getBlow() const
{
	float blow = 0.f;

	if ( m_dispersion >= m_enoughBlow )
	{
		blow = ( m_dispersion - m_enoughBlow ) * 0.1f;
	}

	return blow;
}