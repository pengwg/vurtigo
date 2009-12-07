
FIND_PACKAGE(Doxygen)


IF (DOXYGEN_FOUND)

  ## SET(HELP_QHG_LOCATION ${CMAKE_CURRENT_SOURCE_DIR})
  
  # we need latex for doxygen because of the formulas
  FIND_PACKAGE(LATEX)
  IF    (NOT LATEX_COMPILER)
    MESSAGE(STATUS "latex command LATEX_COMPILER not found but usually required. You will probably get warnings and user inetraction on doxy run.")
  ENDIF (NOT LATEX_COMPILER)
  IF    (NOT MAKEINDEX_COMPILER)
    MESSAGE(STATUS "makeindex command MAKEINDEX_COMPILER not found but usually required.")
  ENDIF (NOT MAKEINDEX_COMPILER)
  IF    (NOT DVIPS_CONVERTER)
    MESSAGE(STATUS "dvips command DVIPS_CONVERTER not found but usually required.")
  ENDIF (NOT DVIPS_CONVERTER)
  
  IF   (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/doxy.config.in")
    MESSAGE(STATUS "configured ${CMAKE_CURRENT_SOURCE_DIR}/doxy.config.in --> ${CMAKE_CURRENT_BINARY_DIR}/doxy.config")
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/doxy.config.in 
      ${CMAKE_CURRENT_BINARY_DIR}/doxy.config
      @ONLY )
    # use (configured) doxy.config from (out of place) BUILD tree:
    SET(DOXY_CONFIG "${CMAKE_CURRENT_BINARY_DIR}/doxy.config")
  ELSE (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/doxy.config.in")
    # failed ...
    MESSAGE(SEND_ERROR "Failed to Find: ${CMAKE_CURRENT_SOURCE_DIR}/doxy.config.in")
  ENDIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/doxy.config.in")
  
  ADD_CUSTOM_TARGET(doc ${DOXYGEN_EXECUTABLE} ${DOXY_CONFIG})
  ADD_CUSTOM_TARGET(qtdoc
  COMMAND qhelpgenerator ${CMAKE_CURRENT_BINARY_DIR}/Doc/html/index.qhp -o ${CMAKE_CURRENT_BINARY_DIR}/Doc/html/VurtigoSrcHelp.qch
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/rtConfig/VurtigoSrcHelp.qhcp ${CMAKE_CURRENT_BINARY_DIR}/Doc/html/VurtigoSrcHelp.qhcp
  COMMAND qcollectiongenerator ${CMAKE_CURRENT_BINARY_DIR}/Doc/html/VurtigoSrcHelp.qhcp -o ${CMAKE_CURRENT_BINARY_DIR}/Doc/html/VurtigoSrcHelp.qhc)
  
ENDIF(DOXYGEN_FOUND)
