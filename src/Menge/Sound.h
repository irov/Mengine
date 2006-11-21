#	pragma once

#	include "Allocator.h"

#	include "NodeImpl.h"

class SoundSystemInterface;
class SoundSourceInterface;

namespace Menge
{
	class Sound
		: public Allocator
	{
		OBJECT_DECLARE(Sound);

	public:
		Sound();

	public:
		void			play(); 
		void			pause();
		void			stop();

		bool			updateSoundBuffer();
		void			setLooped(bool _flag);
		bool			getLooped() const;
		void			setVolume(float _value);
		float			getVolume() const;
		void			setPosition(const float* _position);
		const float*	getPosition() const;
		void			setDistance(float dist);
		float			getDistance() const;
		void			setHeadMode(bool flag);
		bool			getHeadMode() const;

	public:
		void			setSoundSourceInterface(SoundSourceInterface*);
	
	public:
		void			loader(TiXmlElement *xml) override;
	
	protected:
		void			_update(float _timing) override;
		bool			_compile() override;
		void			_release() override;
		
	private:
		std::string		m_filename;
		SoundSourceInterface* m_interface;
	};
}