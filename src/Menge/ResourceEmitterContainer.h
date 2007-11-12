#	pragma once

#	include "ResourceReference.h"

#	include <vector>

class EmitterContainerInterface;
class FileDataInterface;
class RenderImageInterface;

namespace Menge
{
	class ResourceEmitterContainer
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceEmitterContainer )

	public:
		ResourceEmitterContainer( const std::string & _name );

	public:
		void loader( TiXmlElement * _xml ) override;

	public:

		const EmitterContainerInterface * getContainer() const;

		RenderImageInterface * getImage() const;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		std::string m_filename;
		std::string m_fileImage;

		RenderImageInterface * m_image;

		EmitterContainerInterface * m_container;
	};
}