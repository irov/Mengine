#	include "MarmaladeSoundSystem.h"

#	include "MarmaladeSoundConfig.h"
#	include "MarmaladeSoundError.h"

#	include "Logger/Logger.h"

#	include <math.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundSystem, Menge::SoundSystemInterface, Menge::MarmaladeSoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeSoundSystem::MarmaladeSoundSystem()
		: m_serviceProvider(nullptr)
		, m_isDeviceStereo(true)
		, m_soundOutputFrequence(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeSoundSystem::~MarmaladeSoundSystem()
	{     

	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeSoundSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * MarmaladeSoundSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeSoundSystem::initialize()
	{
		LOGGER_INFO(m_serviceProvider)( "Starting Marmalade Sound System..." );

		int32 propertyStereoEnabled = s3eSoundGetInt( S3E_SOUND_STEREO_ENABLED );

		if( propertyStereoEnabled == 1 )
		{
			m_isDeviceStereo = true;
		}
		else if( propertyStereoEnabled == 0 )
		{
			m_isDeviceStereo = false;
		}
		else
		{
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

			m_isDeviceStereo = false;
		}

		m_soundOutputFrequence = s3eSoundGetInt( S3E_SOUND_OUTPUT_FREQ );

		if( m_soundOutputFrequence == -1 )
		{
			MARMALADE_SOUND_CHECK_ERROR(m_serviceProvider);

			m_soundOutputFrequence = 22050;
		}

		int32 numChannels = s3eSoundGetInt( S3E_SOUND_NUM_CHANNELS );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSystem::setupFilter_()
	{
		//int minimumFrequence = MENGINE_MARMALADE_SOUND_INPUT_FREQUENCE < m_soundOutputFrequence ? MENGINE_MARMALADE_SOUND_INPUT_FREQUENCE : m_soundOutputFrequence;
		//double fc = ((double)minimumFrequence / 2.0) / (double)m_soundOutputFrequence; // half the input sample rate (eg nyquist limit of input)

		//// Generate filter coefficients
		//s_wsfirLP( m_filterCoefficients, ESWC_BLACKMAN, fc );

		// Sample rate conversion parameters
		//int gcd = s_GCD( MENGINE_MARMALADE_SOUND_INPUT_FREQUENCE, m_soundOutputFrequence );
		
		//m_W = MENGINE_MARMALADE_SOUND_INPUT_FREQUENCE / gcd;
		//m_L = m_soundOutputFrequence / gcd;		
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeSoundSystem::finalize()
    {       
    }
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeSoundSystem::onTurnSound( bool _turn )
	{
        (void)_turn;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterfacePtr MarmaladeSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _isStream )
	{
		_isStream = false;

		MarmaladeSoundBufferPtr base;

		if( _isStream == false )
		{
			MarmaladeSoundBufferMemory * buffer = m_poolOALSoundBuffer.createObjectT();

			buffer->setServiceProvider( m_serviceProvider );

			base = buffer;
		}
		else
		{
			MarmaladeSoundBufferStream * buffer = m_poolOALSoundBufferStream.createObjectT();

			buffer->setServiceProvider( m_serviceProvider );

			base = buffer;
		}

		if( base->load( _soundDecoder ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "OALSoundSystem: Failed to create sound buffer from stream" 
				);

			return nullptr;
		}

		return base;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundSourceInterfacePtr MarmaladeSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer )
	{
		//OALSoundSource* soundSource = m_soundSources.get();
		MarmaladeSoundSource * soundSource = m_poolOALSoundSource.createObjectT();

        soundSource->initialize( m_serviceProvider, this );
        		
		soundSource->setSoundBuffer( _buffer );
		
		return soundSource;
	}
}	// namespace Menge

