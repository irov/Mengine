set( MY_LIB_NAME Core )

ADD_FILTER(
src
    Assertion.h
    Assertion.cpp

    CRC32.h
    CRC32.cpp
	
	ServantBase.h
	ServiceBase.h
	
	FontBase.h
	FontBase.cpp
    
    Date.h
    Date.cpp
    
    Exception.h
    Visitor.h
    
    Polygon.h
    Polygon.cpp    
    
	Viewport.h
	Viewport.cpp
	
	Resolution.h
	Resolution.cpp	

	ColourValue.h
	ColourValue.cpp
        
    GlyphChar.h
    
    IniUtil.h
    IniUtil.cpp
    
    Hash.h
   
    FilePath.h
    FilePath.cpp
    
    FileLogger.cpp
    FileLogger.h
    
    PixelFormat.h
    PixelFormat.cpp
    
    MemoryAllocator.h
    
    MemoryHelper.h
    MemoryHelper.cpp
	
    ValueInterpolator.h
    ValueFollower.h
    
    String.h
	String.cpp
    
    Tags.h
    Tags.cpp
    
    StringFormat.h
	StringFormat.cpp
    
    Params.h
    
	Rect.h
	Rect.cpp
    
    Stream.h
    Stream.cpp
    
    Pointer.h
	
    ConstString.h
    ConstString.cpp
	ConstStringHolder.h
    ConstStringHolder.cpp
	ConstStringHolderLocalString.h
	ConstStringHolderLocalString.cpp
	ConstStringTypes.h
    
    UID.h
    UID.cpp        
        
    RenderUtils.h
    
    OutputStreamWriter.h   
    
    IntrusiveTree.h
    
    PluginBase.h
    PluginBase.cpp
    
    ModuleBase.h
    ModuleBase.cpp
    
    ModuleFactory.h
    
    Holder.h
    
    RenderVertex2D.h
    RenderIndex.h
   
    Magic.h    
    Mixin.h
    
    MixerValue.h
    MixerValue.cpp
    
    Typename.h
    SHA1.h
    
    IntrusivePtr.h
    IntrusivePtrBase.h
    
    LoggerBase.cpp
    LoggerBase.h
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Utils
)

INCLUDE_DIRECTORIES(    ${Mengine_SOURCE_DIR}/Utils 
						${THIRDPARTY_DIR}/boost/libs/config/include
						${THIRDPARTY_DIR}/boost/libs/assert/include
						${THIRDPARTY_DIR}/boost/libs/preprocessor/include
						${THIRDPARTY_DIR}/boost/libs/predef/include
						${THIRDPARTY_DIR}/boost/libs/numeric/conversion/include
						${THIRDPARTY_DIR}/boost/libs/type_traits/include
						${THIRDPARTY_DIR}/boost/libs/mpl/include
						${THIRDPARTY_DIR}/boost/libs/math/include
						${THIRDPARTY_DIR}/boost/libs/iterator/include
						${THIRDPARTY_DIR}/boost/libs/range/include
						${THIRDPARTY_DIR}/boost/libs/static_assert/include
						${THIRDPARTY_DIR}/boost/libs/throw_exception/include
						${THIRDPARTY_DIR}/boost/libs/throw_detail/include
						${THIRDPARTY_DIR}/boost/libs/core/include
						${THIRDPARTY_DIR}/boost/libs/detail/include
						${THIRDPARTY_DIR}/boost/libs/utility/include
						${THIRDPARTY_DIR}/boost/libs/integer/include
						${THIRDPARTY_DIR}/boost/libs/array/include
						${THIRDPARTY_DIR}/boost/libs/smart_ptr/include
						${THIRDPARTY_DIR}/boost/libs/concept_check/include
						${THIRDPARTY_DIR}/boost/libs/move/include
						${THIRDPARTY_DIR}/boost/libs/format/include
						${THIRDPARTY_DIR}/boost/libs/optional/include
						${THIRDPARTY_DIR}/boost/libs/container/include
						${THIRDPARTY_DIR}/boost/libs/variant/include
						${THIRDPARTY_DIR}/boost/libs/tuple/include
						${THIRDPARTY_DIR}/boost/libs/rational/include
						${THIRDPARTY_DIR}/boost/libs/algorithm/include
						${THIRDPARTY_DIR}/boost/libs/function/include
						${THIRDPARTY_DIR}/boost/libs/type_index/include
						${THIRDPARTY_DIR}/boost/libs/container_hash/include
						${THIRDPARTY_DIR}/boost/libs/bind/include
						${THIRDPARTY_DIR}/boost/libs/function_types/include
						${THIRDPARTY_DIR}/boost/libs/intrusive/include
						${THIRDPARTY_DIR}/boost/libs/tokenizer/include
                        ${THIRDPARTY_DIR}/boost/libs/geometry/include
						${THIRDPARTY_DIR}/boost/libs/lexical_cast/include
						${THIRDPARTY_DIR}/boost/libs/qvm/include
						${THIRDPARTY_DIR}/boost/libs/exception/include
						${THIRDPARTY_DIR}/utf8/source
                        ${THIRDPARTY_DIR}/libmath/include
                        ${THIRDPARTY_DIR}/tinyini/include
                        ) 

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
