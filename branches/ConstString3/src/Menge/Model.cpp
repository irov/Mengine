#	include "Model.h"

#	include "ResourceManager.h"
#	include "ResourceModel.h"

#	include "BinParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Model::Model()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Model::~Model()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Model::setModelResource( const ConstString & _resourceName )
	{
		m_resourceName = _resourceName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Model::getModelResource() const
	{
		return m_resourceName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Model::loader( BinParser * _parser )
	{
		Node::loader(_parser);

		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Model_Name, &Model::setModelResource );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Model::_compile()
	{
		m_resourceModel = ResourceManager::get()
			->getResourceT<ResourceModel>(m_resourceName);

		if( m_resourceModel == 0 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Model::_release()
	{

	}
}