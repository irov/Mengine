#	pragma once

#	include "Kernel/Surface.h"
#	include "Kernel/Resource.h"
#	include "Kernel/Soundable.h"

#   include "ResourceSound.h"

#	include "Interface/SoundSystemInterface.h"

namespace Menge
{
	class SurfaceSound
		: public Surface
		, public Resource
		, public SoundListenerInterface
		, public Soundable
	{
	public:
		SurfaceSound();
		~SurfaceSound();

	public:
		void setResourceSound( const ResourceSoundPtr & _resourceSound );
		const ResourceSoundPtr & getResourceSound() const;

	protected:
		bool _play( float _time ) override;
		bool _restart( float _time, uint32_t _enumerator ) override;
		void _pause( uint32_t _enumerator ) override;
		void _resume( float _time, uint32_t _enumerator ) override;
		void _stop( uint32_t _id ) override;
		void _end( uint32_t _id ) override;
		bool _interrupt( uint32_t _id ) override;
		void _setTiming( float _timing ) override;

    protected:
		void _setVolume( float _volume ) override;
		float _getVolume() const override;
	
	public:
		void updateVolume();

		float getDuration() const;

    protected:
		void onSoundPause( uint32_t _soundId ) override;
		void onSoundStop( uint32_t _soundId ) override;
		
	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void _update( float _time, float _timing ) override;

	protected:
		void _setEventListener( const pybind::dict & _listener ) override;

	protected:
		void _setLoop( bool _value ) override;

	protected:
		RenderMaterialInterfacePtr _updateMaterial() const;

	private:
		ResourceHolder<ResourceSound> m_resourceSound;

        SoundBufferInterfacePtr m_soundBuffer;
        
		uint32_t m_sourceID;

		bool m_isHeadMode;

        float m_volume;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<SurfaceSound> SurfaceSoundPtr;
};
