#pragma once

#include "Interface/RenderSystemInterface.h"

#include "Core/ServantBase.h"

#include "Core/ConstString.h"
#include "Core/FilePath.h"

namespace Mengine
{
	class DecoderRenderImageProvider
		: public ServantBase<RenderImageProviderInterface>
	{
	public:
        DecoderRenderImageProvider();
		~DecoderRenderImageProvider() override;

    public:
        void initialize( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codecName );

	public:
        RenderImageLoaderInterfacePtr getLoader() const override;

	protected:        
        ConstString m_pakName;
        FilePath m_fileName;
        ConstString m_codecName;
    };
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<DecoderRenderImageProvider> DecoderRenderImageProviderPtr;
	//////////////////////////////////////////////////////////////////////////
}	
