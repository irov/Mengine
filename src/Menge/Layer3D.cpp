#	include "Layer3D.h"

#	include "Camera3D.h"

#	include "Player.h"

namespace Menge
{
	void Layer3D::renderLayer()
	{
		Camera3D * camera = 
			Holder<Player>::hostage()
			->getRenderCamera3D();

		const Frustum & cfm = camera->getFrustum();

		const Frustum & frustum = updateFrustum( cfm );

		for each( SceneNode3D * node in m_listChildren )
		{
			const mt::mat4f & wm = node->getWorldMatrix();
			node->render( wm, frustum );
		}
	}
}