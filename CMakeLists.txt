cmake_minimum_required (VERSION 2.8)

project(th)

IF(ORIGINALDESTINATION)
    IF("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
        message("Original destination feature available via Netfilter")
        SET(HAS_ORIG_DST TRUE)
        ADD_DEFINITIONS(-DHAS_ORIG_DST_VIA_NETFILTER)
    ELSE()
        error("Original destination feature not available in " "${CMAKE_SYSTEM_NAME}")
    ENDIF()
ENDIF()

SET(SRC loop.c actions.c helpers.c)

add_library(th SHARED ${SRC})
add_library(th-static STATIC ${SRC})

install(TARGETS th th-static 
     LIBRARY DESTINATION lib
     ARCHIVE DESTINATION lib)
     
install(FILES th.h DESTINATION include)