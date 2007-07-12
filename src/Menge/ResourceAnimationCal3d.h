#	pragma once

#	include "ResourceImpl.h"

#	include "Math/vec4.h"

#	include <vector>

class RenderImageInterface;

namespace Menge
{
	class ResourceAnimationCal3d
		: public ResourceImpl
	{
		RESOURCE_DECLARE( ResourceAnimationCal3d )

	public:
		ResourceAnimationCal3d( const std::string & _name );

	public:

		float getScale() const;
		const std::vector<std::string>& getSceletonInfo() const;
		const std::vector<std::string>& getCoreAnimInfo() const;
		const std::vector<std::string>& getCoreMeshInfo() const;
		const std::vector<std::string>& getCoreMatInfo() const;
		const std::string& getFolderPath() const;
	
	public:
		void loader( TiXmlElement *xml );

	protected:
		bool _compile();
		void _release();

	protected:
		std::string m_fileCAL3D;
		std::string m_filePathCAL3D;

	private:
		float m_scale;
		std::vector<std::string>	m_sceletonInfo;
		std::vector<std::string>	m_coreAnimInfo;
		std::vector<std::string>	m_coreMeshInfo;
		std::vector<std::string>	m_coreMatInfo;
	};
}