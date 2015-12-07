#	pragma once

#	include "Interface/Model3DInterface.h"

#	include "Kernel/ResourceReference.h"

#	include "Kernel/ResourceImage.h"

namespace Menge
{
	class ResourceModel3D
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceModel3D );

	public:
		ResourceModel3D();

	public:
		void setDataflow( const ConstString & _dataflow );
		const ConstString & getDataflow() const;

	public:
		bool _loader( const Metabuf::Metadata * _parser ) override;
		bool _convert() override;

	public:
		bool _compile() override;
		void _release() override;

	protected:
		bool _isValid() const override;

	public:
		inline const Model3DInterfacePtr & getModel() const;
		inline const ResourceImagePtr & getResourceImage() const;

	protected:
		FilePath m_path;
		ConstString m_dataflow;
		ConstString m_converterType;

		Model3DInterfacePtr m_model;

		ConstString m_imageResourceName;
		ResourceImagePtr m_imageResource;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourceModel3D> ResourceModel3DPtr;
	//////////////////////////////////////////////////////////////////////////
	const Model3DInterfacePtr & ResourceModel3D::getModel() const
	{
		return m_model;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const ResourceImagePtr & ResourceModel3D::getResourceImage() const
	{
		return m_imageResource;
	}
}
