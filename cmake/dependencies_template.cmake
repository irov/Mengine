MACRO(MENGINE_ENABLE_DEPENDENCE NAME DOC ENABLE)
    OPTION(${NAME} ${DOC} ${ENABLE})
    
    IF(MENGINE_ENABLE_DEPENDENCE_ONLY)
        IF("${NAME}" STREQUAL "${MENGINE_ENABLE_DEPENDENCE_ONLY}")
            SET(${NAME} ON CACHE BOOL ${DOC} FORCE)
        ELSE()
            SET(${NAME} OFF CACHE BOOL ${DOC} FORCE)
        ENDIF()
    ENDIF()
    
    MESSAGE_OPTION(${NAME})
ENDMACRO()