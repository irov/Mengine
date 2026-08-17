#include "UIFrameworkScriptEmbedding.h"

#include "UILayout2D.h"
#include "UIVirtualList.h"
#include "UIVirtualGrid.h"
#include "UIProgressBar.h"
#include "UISlider.h"
#include "UIRadioGroup.h"
#include "UIPageView.h"
#include "UIFocusScope.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonCallbackProvider.h"
#include "Environment/Python/PythonDocument.h"
#include "Environment/Python/PythonScriptWrapper.h"

#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonUILayout2DSizer
            : public LayoutBoxSizerInterface
            , public PythonCallbackProvider
            , public Factorable
        {
            DECLARE_FACTORABLE( PythonUILayout2DSizer );

        protected:
            bool onGetLayoutBoxSize( mt::vec2f * const _size ) const override
            {
                mt::vec2f size = this->call_cb();

                if( *_size == size )
                {
                    return false;
                }

                *_size = size;

                return true;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PythonUILayout2DSizer, LayoutBoxSizerInterface> PythonUILayout2DSizerPtr;
        //////////////////////////////////////////////////////////////////////////
        class PythonUILayout2DItemGetter
            : public UILayout2DItemGetterInterface
            , public PythonCallbackProvider
            , public Factorable
        {
            DECLARE_FACTORABLE( PythonUILayout2DItemGetter );

        protected:
            bool onGetUILayout2DItemSize( mt::vec2f * const _size ) const override
            {
                mt::vec2f size = this->call_cb();

                if( *_size == size )
                {
                    return false;
                }

                *_size = size;

                return true;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PythonUILayout2DItemGetter, UILayout2DItemGetterInterface> PythonUILayout2DItemGetterPtr;
        //////////////////////////////////////////////////////////////////////////
        class PythonUILayout2DItemSetter
            : public LayoutBoxElementSetterInterface
            , public PythonCallbackProvider
            , public Factorable
        {
            DECLARE_FACTORABLE( PythonUILayout2DItemSetter );

        protected:
            void onSetLayoutBoxElementPosition( const mt::vec2f & _offset, const mt::vec2f & _size ) override
            {
                this->call_cb( _offset, _size );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PythonUILayout2DItemSetter, LayoutBoxElementSetterInterface> PythonUILayout2DItemSetterPtr;
        //////////////////////////////////////////////////////////////////////////
        class PythonUIVirtualCollectionProvider
            : public UIVirtualCollectionProviderInterface
            , public PythonCallbackProvider
            , public Factorable
        {
            DECLARE_FACTORABLE( PythonUIVirtualCollectionProvider );

        protected:
            uint32_t onUIVirtualCollectionCount() const override
            {
                return this->call_method( "count" );
            }

            mt::vec2f onUIVirtualCollectionMeasure( uint32_t _index ) const override
            {
                return this->call_method( "measure", _index );
            }

            NodePtr onUIVirtualCollectionCreateOrReuse( const NodePtr & _node ) override
            {
                return this->call_method( "createOrReuse", _node );
            }

            void onUIVirtualCollectionBind( const NodePtr & _node, uint32_t _index ) override
            {
                this->call_method( "bind", _node, _index );
            }

            void onUIVirtualCollectionRecycle( const NodePtr & _node, uint32_t _index ) override
            {
                this->call_method( "recycle", _node, _index );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PythonUIVirtualCollectionProvider, UIVirtualCollectionProviderInterface> PythonUIVirtualCollectionProviderPtr;
        //////////////////////////////////////////////////////////////////////////
        class PythonUIFloatValueCallback
            : public UIFloatValueCallbackInterface
            , public PythonCallbackProvider
            , public Factorable
        {
            DECLARE_FACTORABLE( PythonUIFloatValueCallback );

        protected:
            void onUIFloatValue( float _value ) override
            {
                this->call_cb( _value );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PythonUIFloatValueCallback, UIFloatValueCallbackInterface> PythonUIFloatValueCallbackPtr;
        //////////////////////////////////////////////////////////////////////////
        class PythonUIIndexValueCallback
            : public UIIndexValueCallbackInterface
            , public PythonCallbackProvider
            , public Factorable
        {
            DECLARE_FACTORABLE( PythonUIIndexValueCallback );

        protected:
            void onUIIndexValue( uint32_t _index ) override
            {
                this->call_cb( _index );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PythonUIIndexValueCallback, UIIndexValueCallbackInterface> PythonUIIndexValueCallbackPtr;
        //////////////////////////////////////////////////////////////////////////
        class PythonUIBoolValueCallback
            : public UIBoolValueCallbackInterface
            , public PythonCallbackProvider
            , public Factorable
        {
            DECLARE_FACTORABLE( PythonUIBoolValueCallback );

        protected:
            void onUIBoolValue( bool _value ) override
            {
                this->call_cb( _value );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PythonUIBoolValueCallback, UIBoolValueCallbackInterface> PythonUIBoolValueCallbackPtr;
        //////////////////////////////////////////////////////////////////////////
        class PythonUIActionCallback
            : public UIActionCallbackInterface
            , public PythonCallbackProvider
            , public Factorable
        {
            DECLARE_FACTORABLE( PythonUIActionCallback );

        protected:
            void onUIAction() override
            {
                this->call_cb();
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PythonUIActionCallback, UIActionCallbackInterface> PythonUIActionCallbackPtr;
        //////////////////////////////////////////////////////////////////////////
        class UIFrameworkScriptMethod
            : public Factorable
        {
        public:
            UILayout2DPtr createUILayout2D( const pybind::object & _sizer, const pybind::args & _args )
            {
                PythonUILayout2DSizerPtr sizer = Helper::makeFactorableUnique<PythonUILayout2DSizer>( MENGINE_DOCUMENT_PYTHON );
                sizer->initialize( _sizer, _args );

                return Helper::createUILayout2D( sizer, MENGINE_DOCUMENT_PYTHON );
            }

            void destroyUILayout2D( const UILayout2DPtr & _layout )
            {
                Helper::destroyUILayout2D( _layout );
            }

            UILayout2DElementPtr addItem( UILayout2DContainer * _container, const pybind::object & _getter, const pybind::object & _setter, const pybind::args & _args )
            {
                PythonUILayout2DItemGetterPtr getter;

                if( _getter.is_none() == false )
                {
                    getter = Helper::makeFactorableUnique<PythonUILayout2DItemGetter>( MENGINE_DOCUMENT_PYTHON );
                    getter->initialize( _getter, _args );
                }

                PythonUILayout2DItemSetterPtr setter;

                if( _setter.is_none() == false )
                {
                    setter = Helper::makeFactorableUnique<PythonUILayout2DItemSetter>( MENGINE_DOCUMENT_PYTHON );
                    setter->initialize( _setter, _args );
                }

                return _container->addItem( getter, setter, MENGINE_DOCUMENT_PYTHON );
            }

            UILayout2DContainerPtr addBox( UILayout2DContainer * _container, EUILayout2DMode _mode )
            {
                return _container->addBox( _mode, MENGINE_DOCUMENT_PYTHON );
            }

            void setUIVirtualCollectionProvider( UIVirtualCollection * _collection, const pybind::object & _provider, const pybind::args & _args )
            {
                if( _provider.is_none() == true )
                {
                    _collection->setProvider( nullptr );

                    return;
                }

                PythonUIVirtualCollectionProviderPtr provider = Helper::makeFactorableUnique<PythonUIVirtualCollectionProvider>( MENGINE_DOCUMENT_PYTHON );
                provider->initialize( _provider, _args );

                _collection->setProvider( provider );
            }

            void setUISliderChangeCallback( UISlider * _slider, const pybind::object & _callback, const pybind::args & _args )
            {
                if( _callback.is_none() == true )
                {
                    _slider->setChangeCallback( nullptr );

                    return;
                }

                PythonUIFloatValueCallbackPtr callback = Helper::makeFactorableUnique<PythonUIFloatValueCallback>( MENGINE_DOCUMENT_PYTHON );
                callback->initialize( _callback, _args );

                _slider->setChangeCallback( callback );
            }

            void setUIRadioGroupChangeCallback( UIRadioGroup * _radioGroup, const pybind::object & _callback, const pybind::args & _args )
            {
                if( _callback.is_none() == true )
                {
                    _radioGroup->setChangeCallback( nullptr );

                    return;
                }

                PythonUIIndexValueCallbackPtr callback = Helper::makeFactorableUnique<PythonUIIndexValueCallback>( MENGINE_DOCUMENT_PYTHON );
                callback->initialize( _callback, _args );

                _radioGroup->setChangeCallback( callback );
            }

            void setUIPageViewChangeCallback( UIPageView * _pageView, const pybind::object & _callback, const pybind::args & _args )
            {
                if( _callback.is_none() == true )
                {
                    _pageView->setChangeCallback( nullptr );

                    return;
                }

                PythonUIIndexValueCallbackPtr callback = Helper::makeFactorableUnique<PythonUIIndexValueCallback>( MENGINE_DOCUMENT_PYTHON );
                callback->initialize( _callback, _args );

                _pageView->setChangeCallback( callback );
            }

            void setFocusableFocusCallback( UIFocusable * _focusable, const pybind::object & _callback, const pybind::args & _args )
            {
                if( _callback.is_none() == true )
                {
                    _focusable->setFocusCallback( nullptr );

                    return;
                }

                PythonUIBoolValueCallbackPtr callback = Helper::makeFactorableUnique<PythonUIBoolValueCallback>( MENGINE_DOCUMENT_PYTHON );
                callback->initialize( _callback, _args );

                _focusable->setFocusCallback( callback );
            }

            void setFocusableActivateCallback( UIFocusable * _focusable, const pybind::object & _callback, const pybind::args & _args )
            {
                if( _callback.is_none() == true )
                {
                    _focusable->setActivateCallback( nullptr );

                    return;
                }

                PythonUIActionCallbackPtr callback = Helper::makeFactorableUnique<PythonUIActionCallback>( MENGINE_DOCUMENT_PYTHON );
                callback->initialize( _callback, _args );

                _focusable->setActivateCallback( callback );
            }

            void setFocusScopeBackCallback( UIFocusScope * _focusScope, const pybind::object & _callback, const pybind::args & _args )
            {
                if( _callback.is_none() == true )
                {
                    _focusScope->setBackCallback( nullptr );

                    return;
                }

                PythonUIActionCallbackPtr callback = Helper::makeFactorableUnique<PythonUIActionCallback>( MENGINE_DOCUMENT_PYTHON );
                callback->initialize( _callback, _args );

                _focusScope->setBackCallback( callback );
            }
        };
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    UIFrameworkScriptEmbedding::UIFrameworkScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UIFrameworkScriptEmbedding::~UIFrameworkScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool UIFrameworkScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        IntrusivePtr<Detail::UIFrameworkScriptMethod> scriptMethod = Helper::makeFactorableUnique<Detail::UIFrameworkScriptMethod>( MENGINE_DOCUMENT_FACTORABLE );

        pybind::enum_<EUILayout2DMode>( _kernel, "UILayout2DMode" )
            .def( "UIL2DM_RELATIVE", EUILayout2DMode::UIL2DM_RELATIVE )
            .def( "UIL2DM_HORIZONTAL", EUILayout2DMode::UIL2DM_HORIZONTAL )
            .def( "UIL2DM_VERTICAL", EUILayout2DMode::UIL2DM_VERTICAL )
            ;

        pybind::interface_<UILayout2DElement, pybind::bases<Factorable>>( _kernel, "UILayout2DElement", false )
            .def( "setAnchors", &UILayout2DElement::setAnchors )
            .def( "setPivot", &UILayout2DElement::setPivot )
            .def( "setPosition", &UILayout2DElement::setPosition )
            .def( "setSize", &UILayout2DElement::setSize )
            .def( "setPercentPosition", &UILayout2DElement::setPercentPosition )
            .def( "setPercentSize", &UILayout2DElement::setPercentSize )
            .def( "setMargin", &UILayout2DElement::setMargin )
            .def( "setFlex", &UILayout2DElement::setFlex )
            .def( "getLayoutOffset", &UILayout2DElement::getLayoutOffset )
            .def( "getLayoutSize", &UILayout2DElement::getLayoutSize )
            ;

        pybind::interface_<UILayout2DContainer, pybind::bases<UILayout2DElement>>( _kernel, "UILayout2DContainer", false )
            .def( "setMode", &UILayout2DContainer::setMode )
            .def( "getMode", &UILayout2DContainer::getMode )
            .def( "setPadding", &UILayout2DContainer::setPadding )
            .def( "getPadding", &UILayout2DContainer::getPadding )
            .def( "setSpacing", &UILayout2DContainer::setSpacing )
            .def( "getSpacing", &UILayout2DContainer::getSpacing )
            .def_proxy_static_args( "addItem", scriptMethod.get(), &Detail::UIFrameworkScriptMethod::addItem )
            .def_proxy_static( "addBox", scriptMethod.get(), &Detail::UIFrameworkScriptMethod::addBox )
            ;

        pybind::interface_<UILayout2D, pybind::bases<Factorable>>( _kernel, "UILayout2D", false )
            .def( "getRoot", &UILayout2D::getRoot )
            .def( "setUseSafeArea", &UILayout2D::setUseSafeArea )
            .def( "getUseSafeArea", &UILayout2D::getUseSafeArea )
            .def( "invalidate", &UILayout2D::invalidate )
            .def( "flush", &UILayout2D::flush )
            ;

        pybind::interface_<UIVirtualCollection, pybind::bases<Node>>( _kernel, "UIVirtualCollection", false )
            .def( "setVirtualArea", &UIVirtualCollection::setVirtualArea )
            .def( "getVirtualArea", &UIVirtualCollection::getVirtualArea )
            .def_proxy_static_args( "setProvider", scriptMethod.get(), &Detail::UIFrameworkScriptMethod::setUIVirtualCollectionProvider )
            .def( "setContentOrigin", &UIVirtualCollection::setContentOrigin )
            .def( "getContentOrigin", &UIVirtualCollection::getContentOrigin )
            .def( "setOverscan", &UIVirtualCollection::setOverscan )
            .def( "getOverscan", &UIVirtualCollection::getOverscan )
            .def( "reloadData", &UIVirtualCollection::reloadData )
            .def( "getVisibleBegin", &UIVirtualCollection::getVisibleBegin )
            .def( "getVisibleEnd", &UIVirtualCollection::getVisibleEnd )
            .def( "getActiveCellCount", &UIVirtualCollection::getActiveCellCount )
            .def( "getPooledCellCount", &UIVirtualCollection::getPooledCellCount )
            ;

        pybind::enum_<EUIVirtualListDirection>( _kernel, "UIVirtualListDirection" )
            .def( "EUIVLD_HORIZONTAL", EUIVirtualListDirection::EUIVLD_HORIZONTAL )
            .def( "EUIVLD_VERTICAL", EUIVirtualListDirection::EUIVLD_VERTICAL )
            ;

        pybind::interface_<UIVirtualList, pybind::bases<UIVirtualCollection>>( _kernel, "UIVirtualList", false )
            .def( "setDirection", &UIVirtualList::setDirection )
            .def( "getDirection", &UIVirtualList::getDirection )
            .def( "setSpacing", &UIVirtualList::setSpacing )
            .def( "getSpacing", &UIVirtualList::getSpacing )
            ;

        pybind::interface_<UIVirtualGrid, pybind::bases<UIVirtualCollection>>( _kernel, "UIVirtualGrid", false )
            .def( "setCellSize", &UIVirtualGrid::setCellSize )
            .def( "getCellSize", &UIVirtualGrid::getCellSize )
            .def( "setSpacing", &UIVirtualGrid::setSpacing )
            .def( "getSpacing", &UIVirtualGrid::getSpacing )
            .def( "setColumns", &UIVirtualGrid::setColumns )
            .def( "getColumns", &UIVirtualGrid::getColumns )
            .def( "getResolvedColumns", &UIVirtualGrid::getResolvedColumns )
            ;

        pybind::enum_<EUIProgressBarDirection>( _kernel, "UIProgressBarDirection" )
            .def( "EUIPBD_LEFT_TO_RIGHT", EUIProgressBarDirection::EUIPBD_LEFT_TO_RIGHT )
            .def( "EUIPBD_RIGHT_TO_LEFT", EUIProgressBarDirection::EUIPBD_RIGHT_TO_LEFT )
            .def( "EUIPBD_TOP_TO_BOTTOM", EUIProgressBarDirection::EUIPBD_TOP_TO_BOTTOM )
            .def( "EUIPBD_BOTTOM_TO_TOP", EUIProgressBarDirection::EUIPBD_BOTTOM_TO_TOP )
            ;

        pybind::interface_<UIProgressBar, pybind::bases<Node>>( _kernel, "UIProgressBar", false )
            .def( "setFillNode", &UIProgressBar::setFillNode )
            .def( "getFillNode", &UIProgressBar::getFillNode )
            .def( "setProgress", &UIProgressBar::setProgress )
            .def( "getProgress", &UIProgressBar::getProgress )
            .def( "setDirection", &UIProgressBar::setDirection )
            .def( "getDirection", &UIProgressBar::getDirection )
            ;

        pybind::interface_<UISlider, pybind::bases<Node>>( _kernel, "UISlider", false )
            .def( "setHotSpot", &UISlider::setHotSpot )
            .def( "getHotSpot", &UISlider::getHotSpot )
            .def( "setThumbNode", &UISlider::setThumbNode )
            .def( "getThumbNode", &UISlider::getThumbNode )
            .def( "setProgressBar", &UISlider::setProgressBar )
            .def( "getProgressBar", &UISlider::getProgressBar )
            .def( "setTrackOffset", &UISlider::setTrackOffset )
            .def( "getTrackOffset", &UISlider::getTrackOffset )
            .def( "setTrackSize", &UISlider::setTrackSize )
            .def( "getTrackSize", &UISlider::getTrackSize )
            .def( "setDirection", &UISlider::setDirection )
            .def( "getDirection", &UISlider::getDirection )
            .def( "setRange", &UISlider::setRange )
            .def( "getMinimum", &UISlider::getMinimum )
            .def( "getMaximum", &UISlider::getMaximum )
            .def( "setStep", &UISlider::setStep )
            .def( "getStep", &UISlider::getStep )
            .def( "setValue", &UISlider::setValue )
            .def( "getValue", &UISlider::getValue )
            .def( "getProgress", &UISlider::getProgress )
            .def_proxy_static_args( "setChangeCallback", scriptMethod.get(), &Detail::UIFrameworkScriptMethod::setUISliderChangeCallback )
            ;

        pybind::interface_<UIRadioGroup, pybind::bases<Node>>( _kernel, "UIRadioGroup", false )
            .def( "setRequired", &UIRadioGroup::setRequired )
            .def( "getRequired", &UIRadioGroup::getRequired )
            .def( "addCheckbox", &UIRadioGroup::addCheckbox )
            .def( "removeCheckbox", &UIRadioGroup::removeCheckbox )
            .def( "getCheckbox", &UIRadioGroup::getCheckbox )
            .def( "getCheckboxCount", &UIRadioGroup::getCheckboxCount )
            .def( "select", &UIRadioGroup::select )
            .def( "clearSelection", &UIRadioGroup::clearSelection )
            .def( "getSelectedIndex", &UIRadioGroup::getSelectedIndex )
            .def_proxy_static_args( "setChangeCallback", scriptMethod.get(), &Detail::UIFrameworkScriptMethod::setUIRadioGroupChangeCallback )
            ;

        pybind::interface_<UIPageView, pybind::bases<Node>>( _kernel, "UIPageView", false )
            .def( "setLoop", &UIPageView::setLoop )
            .def( "getLoop", &UIPageView::getLoop )
            .def( "addPage", &UIPageView::addPage )
            .def( "removePage", &UIPageView::removePage )
            .def( "getPage", &UIPageView::getPage )
            .def( "getPageCount", &UIPageView::getPageCount )
            .def( "setPageIndex", &UIPageView::setPageIndex )
            .def( "getPageIndex", &UIPageView::getPageIndex )
            .def( "nextPage", &UIPageView::nextPage )
            .def( "previousPage", &UIPageView::previousPage )
            .def_proxy_static_args( "setChangeCallback", scriptMethod.get(), &Detail::UIFrameworkScriptMethod::setUIPageViewChangeCallback )
            ;

        pybind::enum_<EUIFocusDirection>( _kernel, "UIFocusDirection" )
            .def( "UIFD_LEFT", EUIFocusDirection::UIFD_LEFT )
            .def( "UIFD_RIGHT", EUIFocusDirection::UIFD_RIGHT )
            .def( "UIFD_UP", EUIFocusDirection::UIFD_UP )
            .def( "UIFD_DOWN", EUIFocusDirection::UIFD_DOWN )
            ;

        pybind::interface_<UIFocusable, pybind::bases<Factorable>>( _kernel, "UIFocusable", false )
            .def( "setNode", &UIFocusable::setNode )
            .def( "getNode", &UIFocusable::getNode )
            .def( "setEnabled", &UIFocusable::setEnabled )
            .def( "getEnabled", &UIFocusable::getEnabled )
            .def( "setNeighbor", &UIFocusable::setNeighbor )
            .def( "getNeighbor", &UIFocusable::getNeighbor )
            .def_proxy_static_args( "setFocusCallback", scriptMethod.get(), &Detail::UIFrameworkScriptMethod::setFocusableFocusCallback )
            .def_proxy_static_args( "setActivateCallback", scriptMethod.get(), &Detail::UIFrameworkScriptMethod::setFocusableActivateCallback )
            ;

        pybind::interface_<UIFocusScope, pybind::bases<Node>>( _kernel, "UIFocusScope", false )
            .def( "addFocusable", &UIFocusScope::addFocusable )
            .def( "removeFocusable", &UIFocusScope::removeFocusable )
            .def( "clearFocusables", &UIFocusScope::clearFocusables )
            .def( "getFocusableCount", &UIFocusScope::getFocusableCount )
            .def( "setFocus", &UIFocusScope::setFocus )
            .def( "getFocus", &UIFocusScope::getFocus )
            .def( "clearFocus", &UIFocusScope::clearFocus )
            .def( "moveFocus", &UIFocusScope::moveFocus )
            .def( "focusNext", &UIFocusScope::focusNext )
            .def( "focusPrevious", &UIFocusScope::focusPrevious )
            .def( "activateFocus", &UIFocusScope::activateFocus )
            .def( "back", &UIFocusScope::back )
            .def_proxy_static_args( "setBackCallback", scriptMethod.get(), &Detail::UIFrameworkScriptMethod::setFocusScopeBackCallback )
            ;

        pybind::interface_<UIFocusController, pybind::bases<Node>>( _kernel, "UIFocusController", false )
            .def( "pushScope", &UIFocusController::pushScope )
            .def( "removeScope", &UIFocusController::removeScope )
            .def( "popScope", &UIFocusController::popScope )
            .def( "clearScopes", &UIFocusController::clearScopes )
            .def( "getScopeCount", &UIFocusController::getScopeCount )
            .def( "getActiveScope", &UIFocusController::getActiveScope )
            ;

        Helper::registerScriptWrapping<UIVirtualList>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<UIVirtualGrid>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<UIProgressBar>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<UISlider>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<UIRadioGroup>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<UIPageView>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<UIFocusScope>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<UIFocusController>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

        pybind::def_functor_args( _kernel, "createUILayout2D", scriptMethod.get(), &Detail::UIFrameworkScriptMethod::createUILayout2D );
        pybind::def_functor( _kernel, "destroyUILayout2D", scriptMethod.get(), &Detail::UIFrameworkScriptMethod::destroyUILayout2D );

        m_implement = scriptMethod;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UIFrameworkScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        Helper::unregisterScriptWrapping<UIFocusController>();
        Helper::unregisterScriptWrapping<UIFocusScope>();
        Helper::unregisterScriptWrapping<UIPageView>();
        Helper::unregisterScriptWrapping<UIRadioGroup>();
        Helper::unregisterScriptWrapping<UISlider>();
        Helper::unregisterScriptWrapping<UIProgressBar>();
        Helper::unregisterScriptWrapping<UIVirtualGrid>();
        Helper::unregisterScriptWrapping<UIVirtualList>();

        _kernel->remove_scope<UIFocusController>();
        _kernel->remove_scope<UIFocusScope>();
        _kernel->remove_scope<UIFocusable>();
        _kernel->remove_scope<EUIFocusDirection>();
        _kernel->remove_scope<UIPageView>();
        _kernel->remove_scope<UIRadioGroup>();
        _kernel->remove_scope<UISlider>();
        _kernel->remove_scope<UIProgressBar>();
        _kernel->remove_scope<EUIProgressBarDirection>();
        _kernel->remove_scope<UIVirtualGrid>();
        _kernel->remove_scope<UIVirtualList>();
        _kernel->remove_scope<EUIVirtualListDirection>();
        _kernel->remove_scope<UIVirtualCollection>();
        _kernel->remove_scope<UILayout2D>();
        _kernel->remove_scope<UILayout2DContainer>();
        _kernel->remove_scope<UILayout2DElement>();
        _kernel->remove_scope<EUILayout2DMode>();

        m_implement = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
