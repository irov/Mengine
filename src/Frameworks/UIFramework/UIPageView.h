#pragma once

#include "UIControlCallbackInterface.h"

#include "Kernel/Node.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UIPageView
        : public Node
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( UIPageView );
        DECLARE_VISITABLE( Node );
        DECLARE_TRANSFORMABLE();

    public:
        UIPageView();
        ~UIPageView() override;

    public:
        void setLoop( bool _loop );
        bool getLoop() const;

        void addPage( const NodePtr & _page );
        NodePtr removePage( uint32_t _index );
        const NodePtr & getPage( uint32_t _index ) const;
        uint32_t getPageCount() const;

        bool setPageIndex( uint32_t _index );
        uint32_t getPageIndex() const;
        bool nextPage();
        bool previousPage();

        void setChangeCallback( const UIIndexValueCallbackInterfacePtr & _callback );
        const UIIndexValueCallbackInterfacePtr & getChangeCallback() const;

    protected:
        void _dispose() override;

    private:
        void updatePages_();

    private:
        Vector<NodePtr> m_pages;
        UIIndexValueCallbackInterfacePtr m_changeCallback;
        uint32_t m_pageIndex;
        bool m_loop;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<UIPageView> UIPageViewPtr;
    //////////////////////////////////////////////////////////////////////////
}
