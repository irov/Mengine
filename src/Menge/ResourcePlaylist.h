#	pragma once

#	include "ResourceReference.h"

#	include <vector>

class FileDataInterface;

namespace Menge
{
	typedef	std::vector<std::string> TVecTrack;

	class ResourcePlaylist
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourcePlaylist )

	public:
		ResourcePlaylist( const std::string & _name );

	public:
		void loader( TiXmlElement * _xml ) override;

	public:
		const TVecTrack & getTracks() const;
		bool getLoop() const;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		bool	  m_loop;
		TVecTrack m_tracks;

		std::string m_filename;
	};
}