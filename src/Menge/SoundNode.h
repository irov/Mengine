#	pragma once

#	include "ResourceImpl.h"

class SoundSourceInterface;
class FileDataInterface;

namespace Menge
{
	class ResourceSound
		: public ResourceImpl
	{
		RESOURCE_DECLARE( ResourceSound )

	public:
		ResourceSound( const std::string & _name );

	public:
		void play();
		void pause();
		void stop();

		bool isPlaying() const;

		void setVolume(float vol);
		float getVolume() const;

		void setPosition(float x, float y, float z);
		const float* getPosition();

		void setLoop(bool loop);
		bool isLooping() const;

		double	getLengthS();

	public:
		void loader( TiXmlElement *xml );

	protected:
		bool _compile();
		void _release();
	private:
		std::string	m_filename;
		SoundSourceInterface* m_interface;
		FileDataInterface* m_fileData;	
	};
}