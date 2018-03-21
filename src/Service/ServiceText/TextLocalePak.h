# pragma once

#include "Interface/MemoryInterface.h"

#include "Core/ConstString.h"
#include "Core/String.h"
#include "Core/FilePath.h"

#include "Factory/FactorablePtr.h"

namespace Mengine
{
	class TextLocalePack
		: public FactorablePtr
	{
	public:
		TextLocalePack();
		~TextLocalePack() override;

	public:
		bool initialize( const ConstString & _pakName, const FilePath & _path );
		
	public:
		const ConstString & getPackName() const;
		const FilePath & getPath() const;

	public:
		MemoryInterfacePtr getXmlBuffer() const;

	protected:
		ConstString m_pakName;
		FilePath m_path;

		MemoryBufferInterfacePtr m_memory;		
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<TextLocalePack> TextLocalePackPtr;
}
