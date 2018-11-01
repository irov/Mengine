#pragma once

#include "Kernel/Mixin.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class Content
		: public Mixin
    {
    public:
		Content();
        ~Content() override;

    public:
        void setFilePath( const FilePath & _filePath );
        const FilePath & getFilePath() const;

		void setCodecType( const ConstString & _codecType );
		const ConstString & getCodecType() const;

		void setConverterType( const ConstString & _converterType );
		const ConstString & getConverterType() const;

	public:
		void setValidNoExist( bool _valid );
		bool isValidNoExist() const;

    protected:
        FilePath m_filePath;
		ConstString m_codecType;
		ConstString m_converterType;

		bool m_validNoExist;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Content> ContentPtr;
	//////////////////////////////////////////////////////////////////////////
}
