//#	pragma once
//
//#	include "ResourceImpl.h"
//
//#	include "Math/vec4.h"
//
//#	include <vector>
//
//#	include <cal3d\cal3d.h>
//
//namespace Menge
//{
//	struct AnimInfo
//	{
//		int		index;
//		float	delay;
//		float	blend;
//	};
//
//	typedef std::vector<std::string> TVecHardPoints;
//
//	class ResourceAnimationCal3d
//		: public ResourceImpl
//	{
//		RESOURCE_DECLARE( ResourceAnimationCal3d )
//
//	public:
//		ResourceAnimationCal3d( const std::string & _name );
//	
//	public:
//		void loader( TiXmlElement * _xml );
//
//	protected:
//		bool _compile();
//		void _release();
//
//	public:
//		int		getAnimationCount() const;
//		int		getMeshCount() const;
//		int		getAnimationId( const std::string& _name ) const;
//		int		getBoneIndex( const std::string& _name ) const;
//
//		const	TVecHardPoints & getHardPoints() const; 
//		const   AnimInfo & getAnimationInfo( const std::string& _name ) const;
//	public:
//		CalModel * getNewInstance();
//		CalCoreAnimation * getCoreAnimation( int _index );
//	private:
//		typedef std::vector<AnimInfo> TVectorAnimInfo;
//		TVectorAnimInfo m_animInfos;
//		TVecHardPoints m_hardPoints;
//
//		std::string m_folder;
//		std::string m_filename;
//
//		CalCoreModel * m_calCoreModel;
//
//		void	_createMaterials();
//		void	_freeMaterials();
//	};
//}