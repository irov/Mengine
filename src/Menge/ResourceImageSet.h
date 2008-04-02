#	pragma once

#	include "ResourceImageCell.h"

#	include <vector>

namespace Menge
{
	//! ResourceImageSet - ресурс-файл, который содержит список uv - областей. 

	/*! xml - файл имеет следующую структуру:
	* <Resource Name = "имя_ресурса" Type = "ResourceImageSet" >
	*	<Frame UV = "left;top;right;bottom"/>
	*    ...
	*   <Frame UV = "left;top;right;bottom"/>
	* </Resource>
	*/

	class ResourceImageSet
		: public ResourceImageCell
	{
		RESOURCE_DECLARE( ResourceImageSet )

	public:
		ResourceImageSet( const ResourceFactoryParam & _params );

	public:
		void addFrameUV( const mt::vec4f & _uv );

	public:
		const mt::vec2f & getMaxSize( unsigned int _frame ) const override;
		const mt::vec2f & getSize( unsigned int _frame ) const override;

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;

	public:
		typedef std::vector<mt::vec2f> TVectorSizes;
		TVectorSizes m_sizes;
	};
}