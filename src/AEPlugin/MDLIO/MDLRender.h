#	pragma once

#	include "Render.h"

#	include "Model.h"

#	include <vector>

namespace Menge
{
	class MDLRender
		: public Render
	{
	public:
		MDLRender();

	public:
		bool initialize() override;
		void finalize() override;

	public:
		Model * createModel( const std::wstring & _ptc );
		bool destroyModel( Model * _model );

	public:
		bool renderModel( Model * _model );
		
	protected:
		void releaseResource_() override;
		bool resetDevice_() override;

	protected:
		typedef std::vector<Model *> TVectorModels;
		TVectorModels m_models;		
	};
}