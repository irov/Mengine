#pragma once

#include "Kernel/Factory.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
	class SDLFileGroupDirectoryFactory
		: public Factory
	{
	public:
		SDLFileGroupDirectoryFactory( const FilePath & _relationPath );
		~SDLFileGroupDirectoryFactory() override;

	protected:
		Factorable * _createObject() override;
		void _destroyObject(Factorable * _obj) override;

	protected:
		FilePath m_relationPath;
	};
}