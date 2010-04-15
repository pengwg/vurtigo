# FindDCMTK.cmake - Find Offis DCMTK libraries
#
#  DCMTK_INCLUDE_DIR   - Directories to include to use DCMTK
#  DCMTK_LIBRARIES     - Files to link against to use DCMTK
#  DCMTK_FOUND         - If false, don't try to use DCMTK
#  DCMTK_DIR           - (optional input) Source directory for DCMTK
#
# DCMTK_DIR can be used to make it simpler to find the various include
# directories and compiled libraries if you've just compiled it in the
# source tree. Just set it to the root of the tree where you extracted
# or installed DCMTK.
#
# Written for VXL by Amitha Perera.
# Updated by Jacob Foshee for DCMTK 3.5.4
#

#####################################################################

SET(STD_PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local
    /usr
    /sw          # Fink
    /opt/local   # DarwinPorts
    /opt/csw     # Blastwave
    /opt
)

#####################################################################
MACRO(FIND_INCLUDE_DIR  INCLUDE_DIR_VAR  THIS_DCMTK_LIB  THIS_DCMTK_INC)

# Clear the Variable
SET(${INCLUDE_DIR_VAR} ${INCLUDE_DIR_VAR}-NOTFOUND)

IF (NOT DCMTK_DIR)
    FIND_PATH( ${INCLUDE_DIR_VAR} dcmtk/${THIS_DCMTK_LIB}/${THIS_DCMTK_INC}
        PATHS
            ${STD_PATHS}/
        PATH_SUFFIXES
            include
    )
ELSE (NOT DCMTK_DIR)
    FIND_PATH( ${INCLUDE_DIR_VAR} dcmtk/${THIS_DCMTK_LIB}/${THIS_DCMTK_INC}
        PATHS
            ${DCMTK_DIR}/
            ${DCMTK_DIR}/${THIS_DCMTK_LIB}/
            ${DCMTK_DIR}/include/${THIS_DCMTK_LIB}/
            ${DCMTK_DIR}/include/
            ${DCMTK_DIR}/${THIS_DCMTK_LIB}/include/
        PATH_SUFFIXES
            include
        NO_DEFAULT_PATH
    )
ENDIF(NOT DCMTK_DIR)

MARK_AS_ADVANCED(${INCLUDE_DIR_VAR})

ENDMACRO(FIND_INCLUDE_DIR)

#####################################################################
MACRO(FIND_DCMTK_LIBS LIB_VAR THIS_DCMTK_LIB)

# Clear the vaiables
SET(${LIB_VAR}_RELEASE ${LIB_VAR}_RELEASE-NOTFOUND)
SET(${LIB_VAR}_DEBUG ${LIB_VAR}_DEBUG-NOTFOUND)

IF (NOT DCMTK_DIR)
    FIND_LIBRARY( ${LIB_VAR}_RELEASE
        NAMES
            ${THIS_DCMTK_LIB}_o
            ${THIS_DCMTK_LIB}o
            ${THIS_DCMTK_LIB}
        PATHS
            ${STD_PATHS}/
        PATH_SUFFIXES
            Release
            lib
    )
    FIND_LIBRARY( ${LIB_VAR}_DEBUG
        NAMES
            ${THIS_DCMTK_LIB}_d
            ${THIS_DCMTK_LIB}d
            ${THIS_DCMTK_LIB}
        PATHS
            ${STD_PATHS}/
        PATH_SUFFIXES
            Debug
            lib
    )
ELSE (NOT DCMTK_DIR)
    FIND_LIBRARY( ${LIB_VAR}_RELEASE
        NAMES
            ${THIS_DCMTK_LIB}_o
            ${THIS_DCMTK_LIB}o
            ${THIS_DCMTK_LIB}
        PATHS   
            ${DCMTK_DIR}/
            ${DCMTK_DIR}/${THIS_DCMTK_LIB}/
            ${DCMTK_DIR}/${THIS_DCMTK_LIB}/libsrc/
        PATH_SUFFIXES
            Release
            lib
        NO_DEFAULT_PATH
    )
    FIND_LIBRARY( ${LIB_VAR}_DEBUG
        NAMES
            ${THIS_DCMTK_LIB}_d
            ${THIS_DCMTK_LIB}d
            ${THIS_DCMTK_LIB}
        PATHS  
            ${DCMTK_DIR}/
            ${DCMTK_DIR}/${THIS_DCMTK_LIB}/
            ${DCMTK_DIR}/${THIS_DCMTK_LIB}/libsrc/
        PATH_SUFFIXES
            Debug
            lib
        NO_DEFAULT_PATH
    )
ENDIF(NOT DCMTK_DIR)

SET(${LIB_VAR}
    debug     ${${LIB_VAR}_DEBUG}
    optimized ${${LIB_VAR}_RELEASE}
)
MARK_AS_ADVANCED(${LIB_VAR}_RELEASE ${LIB_VAR}_DEBUG)

ENDMACRO(FIND_DCMTK_LIBS)

#####################################################################
#config
FIND_INCLUDE_DIR(DCMTK_config_INCLUDE_DIR config osconfig.h)

# ofstd
FIND_INCLUDE_DIR(DCMTK_ofstd_INCLUDE_DIR ofstd ofstdinc.h)
FIND_DCMTK_LIBS(DCMTK_ofstd_LIBRARY ofstd)

# dcmdata
FIND_INCLUDE_DIR(DCMTK_dcmdata_INCLUDE_DIR dcmdata dctypes.h)
FIND_DCMTK_LIBS(DCMTK_dcmdata_LIBRARY dcmdata)

# dcmimgle
FIND_INCLUDE_DIR(DCMTK_dcmimgle_INCLUDE_DIR dcmimgle dcmimage.h)
FIND_DCMTK_LIBS(DCMTK_dcmimgle_LIBRARY dcmimgle)

# dcmimage
FIND_INCLUDE_DIR(DCMTK_dcmimage_INCLUDE_DIR dcmimage dicoimg.h)
FIND_DCMTK_LIBS(DCMTK_dcmimage_LIBRARY dcmimage)

# dcmnet
FIND_DCMTK_LIBS(DCMTK_dcmnet_LIBRARY dcmnet)

#####################################################################

IF( DCMTK_config_INCLUDE_DIR )
IF( DCMTK_ofstd_INCLUDE_DIR )
IF( DCMTK_ofstd_LIBRARY )
IF( DCMTK_dcmdata_INCLUDE_DIR )
IF( DCMTK_dcmdata_LIBRARY )
IF( DCMTK_dcmimgle_INCLUDE_DIR )
IF( DCMTK_dcmimgle_LIBRARY )

    SET( DCMTK_FOUND "YES" )
    SET( DCMTK_INCLUDE_DIR
        ${DCMTK_config_INCLUDE_DIR}
        ${DCMTK_ofstd_INCLUDE_DIR}
        ${DCMTK_dcmdata_INCLUDE_DIR}
        ${DCMTK_dcmimgle_INCLUDE_DIR}
        ${DCMTK_dcmimage_INCLUDE_DIR}
    )

    SET( DCMTK_LIBRARIES
        ${DCMTK_dcmimgle_LIBRARY}
        ${DCMTK_dcmdata_LIBRARY}
        ${DCMTK_ofstd_LIBRARY}
        ${DCMTK_config_LIBRARY}
    )

    IF(DCMTK_dcmimage_LIBRARY)
        LIST(APPEND DCMTK_LIBRARIES ${DCMTK_dcmimage_LIBRARY} )
    ENDIF(DCMTK_dcmimage_LIBRARY)

    IF(DCMTK_dcmnet_LIBRARY)
        LIST(APPEND DCMTK_LIBRARIES ${DCMTK_dcmnet_LIBRARY})
    ENDIF(DCMTK_dcmnet_LIBRARY)

    # Optional DCMTK Support:
    # define user configurable options and paths to external libraries which might be used
    # set precompiler switches according to settings
    # libxml support
    OPTION(DCMTK_WITH_LIBXML "Turn support for libxml on/off" OFF)
    FIND_PATH(DCMTK_WITH_LIBXMLINC "include/libxml/parser.h" ${DCMTK_DIR}/../libxml2-2.6.26)
    IF(DCMTK_WITH_LIBXML)
        ADD_DEFINITIONS(-DDCMTK_WITH_LIBXML)
        INCLUDE_DIRECTORIES(${DCMTK_WITH_LIBXMLINC}/include/libxml)
        LINK_DIRECTORIES(${DCMTK_WITH_LIBXMLINC}/lib)
        LIST(APPEND DCMTK_LIBRARIES debug iconv_d optimized iconv_o debug libxml2_d optimized libxml2_o)
    ENDIF(DCMTK_WITH_LIBXML)
    # libpng support
    OPTION(DCMTK_WITH_LIBPNG "Turn support for libpng on/off" OFF)
    FIND_PATH(DCMTK_WITH_LIBPNGINC "include/png.h" ${DCMTK_DIR}/../libpng-1.2.8)
    IF(DCMTK_WITH_LIBPNG)
        ADD_DEFINITIONS(-DDCMTK_WITH_LIBPNG)
        INCLUDE_DIRECTORIES(${DCMTK_WITH_LIBPNGINC}/include)
        LINK_DIRECTORIES(${DCMTK_WITH_LIBPNGINC}/lib)
        LIST(APPEND DCMTK_LIBRARIES debug libpng_d optimized libpng_o)
    ENDIF(DCMTK_WITH_LIBPNG)
    # libtiff support
    OPTION(DCMTK_WITH_LIBTIFF "Turn support for libtiff on/off" OFF)
    FIND_PATH(DCMTK_WITH_LIBTIFFINC "include/tiff.h" ${DCMTK_DIR}/../tiff-3.7.4)
    IF(DCMTK_WITH_LIBTIFF)
        ADD_DEFINITIONS(-DDCMTK_WITH_LIBTIFF)
        INCLUDE_DIRECTORIES(${DCMTK_WITH_LIBTIFFINC}/include)
        LINK_DIRECTORIES(${DCMTK_WITH_LIBTIFFINC}/lib)
        LIST(APPEND DCMTK_LIBRARIES debug libtiff_d optimized libtiff_o)
    ENDIF(DCMTK_WITH_LIBTIFF)
    # openssl support
    OPTION(DCMTK_WITH_OPENSSL "Turn support for openssl on/off" OFF)
    FIND_PATH(DCMTK_WITH_OPENSSLINC "include/openssl/ssl.h" ${DCMTK_DIR}/../openssl-0.9.8a)
    IF(DCMTK_WITH_OPENSSL)
        ADD_DEFINITIONS(-DDCMTK_WITH_OPENSSL)
        INCLUDE_DIRECTORIES(${DCMTK_WITH_OPENSSLINC}/include/openssl)
        LINK_DIRECTORIES(${DCMTK_WITH_OPENSSLINC}/lib)
        LIST(APPEND DCMTK_LIBRARIES debug libeay32_d optimized libeay32_o debug ssleay32_d optimized ssleay32_o)
    ENDIF(DCMTK_WITH_OPENSSL)
    # zlib support
    OPTION(DCMTK_WITH_ZLIB "Turn support for zlib on/off" OFF)
    FIND_PATH(DCMTK_WITH_ZLIBINC "include/zlib.h" ${DCMTK_DIR}/../zlib-1.2.3)
    IF(DCMTK_WITH_ZLIB)
        ADD_DEFINITIONS(-DDCMTK_WITH_ZLIB)
        INCLUDE_DIRECTORIES(${DCMTK_WITH_ZLIBINC}/include)
        LINK_DIRECTORIES(${DCMTK_WITH_ZLIBINC}/lib)
        LIST(APPEND DCMTK_LIBRARIES debug zlib_d optimized zlib_o)
    ENDIF(DCMTK_WITH_ZLIB)

    MARK_AS_ADVANCED(DCMTK_WITH_LIBXMLINC DCMTK_WITH_LIBPNGINC DCMTK_WITH_LIBTIFFINC DCMTK_WITH_OPENSSLINC DCMTK_WITH_ZLIBINC)

    IF( WIN32 )
        LIST(APPEND DCMTK_LIBRARIES netapi32 wsock32)
    ENDIF( WIN32 )

ENDIF( DCMTK_dcmimgle_LIBRARY )
ENDIF( DCMTK_dcmimgle_INCLUDE_DIR )
ENDIF( DCMTK_dcmdata_LIBRARY )
ENDIF( DCMTK_dcmdata_INCLUDE_DIR )
ENDIF( DCMTK_ofstd_LIBRARY )
ENDIF( DCMTK_ofstd_INCLUDE_DIR )
ENDIF( DCMTK_config_INCLUDE_DIR )

