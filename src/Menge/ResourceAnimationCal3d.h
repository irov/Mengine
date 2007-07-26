#	pragma once

#	include "ResourceImpl.h"

#	include "Math/vec4.h"

#	include <vector>

#	include <cal3d\cal3d.h>

namespace Menge
{
	struct AnimInfo
	{
		size_t	index;
		float	delay;
		float	blend;
	};

	typedef std::vector<std::string> TVecHardPoints;

	class ResourceAnimationCal3d
		: public ResourceImpl
	{
		RESOURCE_DECLARE( ResourceAnimationCal3d )

	public:
		ResourceAnimationCal3d( const std::string & _name );
	
	public:
		void loader( TiXmlElement *xml );

	protected:
		bool _compile();
		void _release();

	protected:
		std::string m_filename;

	public:
		size_t	getAnimationCount() const;
		size_t  getMeshCount() const;
		int		getAnimationId(const std::string& _name) const;
		int		getBoneIndex(const std::string& _bonename) const;

		const	TVecHardPoints & getHardPoints() const; 
	public:
		AnimInfo * getAnimationInfo(const std::string& _name);
	public:
		// cal3d specs:
		CalModel * getNewInstance();
		CalCoreAnimation * getCoreAnimation(int _index);
	private:
		typedef std::vector<AnimInfo> TVectorAnimInfo;
		TVectorAnimInfo m_animInfos;
		TVecHardPoints m_hardPoints;

		std::string m_folder;

		CalCoreModel * m_calCoreModel;

		void	_createMaterials();
		void	_freeMaterials();
	};
}