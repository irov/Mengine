#	pragma once

#	include "Allocator.h"

#	include "NodeImpl.h"

class SoundSystemInterface;
class SoundSourceInterface;
class FileDataInterface;

namespace Menge
{
	class SoundNode
		: public Allocator
	{
		OBJECT_DECLARE(SoundNode);

	public:
		SoundNode();

	public:

		bool play();
		bool pause();
		bool stop();

		bool isPlaying() const;
		bool isPaused() const;
		bool isStopped() const;
		bool isInitial() const;

		void setPitch(float pitch);
		float getPitch() const;

		void setGain(float gain);
		float getGain() const;

		void setRelativeToListener(bool relative);
		bool isRelativeToListener() const;

		void setPosition(const mt::vec3f& _position);
		mt::vec3f getPosition() const;

		void setLoop(bool loop);
		bool isLooping() const;

		bool process();

		double		getSizeSec()	const;
	public:
		void			loader(TiXmlElement *xml) override;
	
	protected:
		void			_update(float _timing) override;
		bool			_compile() override;
		void			_release() override;
	
	private:
		std::string	m_filename;
		SoundSourceInterface* m_interface;
		FileDataInterface* m_fileData;	
	};
}