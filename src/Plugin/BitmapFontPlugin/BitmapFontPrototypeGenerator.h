#pragma once

#include "Interface/PrototypeManagerInterface.h"

#include "Core/ServantBase.h"

#include "Factory/Factory.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class BitmapFontPrototypeGenerator
		: public ServantBase<PrototypeGeneratorInterface>
	{
	public:
		bool initialize( const ConstString & _category, const ConstString & _prototype ) override;

	public:
		PointerFactorable generate() override;

	public:
		uint32_t count() const override;

	protected:
		FactoryPtr m_factoryFont;
	};
}