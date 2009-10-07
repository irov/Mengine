#	pragma once

#	include "Interface/ApplicationInterface.h"

struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;

namespace Menge
{
	class DynamicLibrary : public DynamicLibraryInterface
	{
	public:
		DynamicLibrary( const String& _name );
		~DynamicLibrary();
	public:
		void load() override;
        void unload() override;
		const String& getName() const override;
        TFunctionPtr getSymbol( const String& _name ) const override;
	private:
		String m_name;
		hInstance m_hInst;
	};
};
