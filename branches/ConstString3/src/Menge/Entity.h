#	pragma once

#	include "Node.h"

namespace Menge
{
	class Entity
		: public Node
	{
    public:
        Entity();
        ~Entity();

        static size_t s_enum;

	public:
		void setPrototype( const ConstString & _prototype );
		const ConstString & getPrototype() const;

	protected:
		bool _activate() override;
		void _afterActivate() override;
		void _afterDeactivate() override;
		bool _compile() override;
		void _release() override;
		
		void _create() override;
		bool _destroy() override;
                
	protected:
		void _embedding( PyObject * _embed ) override;
        
	protected:
		ConstString m_prototype;
	};
}