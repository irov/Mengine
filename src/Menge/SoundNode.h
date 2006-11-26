#	pragma once

#	include "Allocator.h"

#	include "NodeImpl.h"

class SoundSystemInterface;
class SoundSourceInterface;
// ��� ������ � �������� ����������? ��� �� ���������?
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

		void			setPosition(const mt::vec2f& _position);
		mt::vec2f		getPosition() const;

		void			setMaxDistance(float dist);
		float			getMaxDistance() const;
		void			setHeadMode(bool flag);
		bool			getHeadMode() const;

		bool			isPlaying() const;

		double			getTotalSize()	const;
		double			getPos()	const;
		void			release();
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