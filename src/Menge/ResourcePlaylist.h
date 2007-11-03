#	pragma once

#	include "ResourceReference.h"

#	include <vector>

class FileDataInterface;

namespace Menge
{
	class ResourcePlaylist
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourcePlaylist )

	public:
		ResourcePlaylist( const std::string & _name );

	public:
		void loader( TiXmlElement * _xml ) override;

		const std::vector<std::string> & getTracks();

	protected:
		bool _compile() override;
		void _release() override;

	private:
		typedef	std::vector<std::string> TVecTrack;
		TVecTrack m_tracks;

		std::string m_filename;
	};
}