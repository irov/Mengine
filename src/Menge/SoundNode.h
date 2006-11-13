#	pragma once

#	include "Allocator.h"

#	include "NodeImpl.h"

class SoundSystemInterface;
class SoundSourceInterface;

namespace Menge
{
	class SoundNode
		: public Allocator
	{
		OBJECT_DECLARE(SoundNode);
	public:
		SoundNode();
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
	protected:
		void			_update(float _timing) override;
		bool			_compile() override;
		void			_release() override;
		void			loader(TiXmlElement *xml) override;
	private:
		std::string		m_filename;
		SoundSourceInterface* m_interface;
	};
}