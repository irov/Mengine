#	include "ResourceMessageStorage.h"
#	include "ResourceImplement.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceMessageStorage )
	//////////////////////////////////////////////////////////////////////////
	ResourceMessageStorage::ResourceMessageStorage( const std::string & _name )
	: ResourceImpl(_name)
	{}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMessageStorage::loader( TiXmlElement * _xml )
	{
		int			localId = -1;
		float		localWidthText = -1;
		std::string localText;		
		std::string localSoundname;

		localText.reserve(1000);
		localSoundname.reserve(1000);

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE_FOR_EACH("Messages")
			{
				XML_CHECK_VALUE_NODE("Message","id",localId);
				XML_CHECK_VALUE_NODE("Message","text",localText);
				XML_CHECK_VALUE_NODE("Message","sound",localSoundname);
				XML_CHECK_VALUE_NODE("Message","width",localWidthText);

				m_messages.insert(
					std::make_pair(localId,
						new Message(localId,localText,localSoundname,localWidthText)
					)
				);
			}
		}

		ResourceImpl::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMessageStorage::_compile()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMessageStorage::_release()
	{
		for(TMapMessage::iterator it = m_messages.begin(); it != m_messages.end(); ++it)
		{
			delete it->second;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Message*	ResourceMessageStorage::getMessage(int id)
	{
		TMapMessage::iterator it = m_messages.find(id);

		if(it == m_messages.end())
		{
			return NULL;
		}

		return it->second;
	}
	//////////////////////////////////////////////////////////////////////////
}