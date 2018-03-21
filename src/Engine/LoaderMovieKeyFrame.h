//#	pragma once
//
//#	include "Loadable.h"
//
//#	include <memory.h>
//
//namespace Menge
//{
//	class MovieFramePack;
//
//	class LoaderMovieKeyFrame
//		: public Loadable
//	{
//	public:
//		LoaderMovieKeyFrame();
//
//	public:
//		MovieFramePack * getFramePack() const;
//
//	public:
//		void loader( const Metabuf::Metadata * _parser ) override;
//
//	protected:
//		void loaderKeyFramesPack_( BinParser * _parser );
//		void loaderKeyFrames2D_( BinParser * _parser, size_t _layerIndex );
//		void loaderKeyFrames3D_( BinParser * _parser, size_t _layerIndex );
//
//	protected:
//		MovieFramePack * m_pack;
//	};
//}