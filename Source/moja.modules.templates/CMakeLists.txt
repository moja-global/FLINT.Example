set(PACKAGE "templates")
set(LIBNAME "moja.modules.${PACKAGE}")
string(TOUPPER "${PACKAGE}" LIBNAME_EXPORT)

include(${CMAKE_MODULE_PATH}/generate_library_version.cmake) 

find_package(Poco REQUIRED Foundation JSON Data DataSQLite)
find_package(Boost COMPONENTS log log_setup REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(moja REQUIRED COMPONENTS moja.flint)

# Set DLL version info
if (MSVC)
  generate_library_version(ProductVersionFiles
    NAME ${LIBNAME}
    FILE_DESCRIPTION "MOJA Templates"
    VERSION_MAJOR ${CR_VERSION_MAJOR}
    VERSION_MINOR ${CR_VERSION_MINOR}
    VERSION_PATCH ${CR_VERSION_PATCH}
	COMPANY_NAME "moja global"
    VERSION_REVISION ${CR_VERSION_REVISION}
  )
endif ()

configure_file(../templates/exports.h ${CMAKE_CURRENT_SOURCE_DIR}/include/moja/modules/${PACKAGE}/_modules.${PACKAGE}_exports.h)

set(MOJA_TEMPLATES_headers
	include/moja/modules/${PACKAGE}/_modules.${PACKAGE}_exports.h
	include/moja/modules/${PACKAGE}/libraryfactory.h
)

set(MOJA_TEMPLATES_sources
	src/libraryfactory.cpp
)

set(MOJA_TEMPLATES_Modules_headers
)

set(MOJA_TEMPLATES_Modules_sources
)

set(MOJA_TEMPLATES_Transform_Headers
)

set(MOJA_TEMPLATES_Transform_Sources
)

set(MOJA_TEMPLATES_Flintdata_Headers
)

set(MOJA_TEMPLATES_Flintdata_Sources
)

source_group("header files\\other"				FILES ${MOJA_TEMPLATES_headers})
source_group("source files\\other"				FILES ${MOJA_TEMPLATES_sources})
source_group("header files\\modules"			FILES ${MOJA_TEMPLATES_Modules_headers})
source_group("source files\\modules"			FILES ${MOJA_TEMPLATES_Modules_sources})
source_group("header files\\transforms"			FILES ${MOJA_TEMPLATES_Transform_Headers})
source_group("source files\\transforms"			FILES ${MOJA_TEMPLATES_Transform_Sources})
source_group("header files\\flintdata"			FILES ${MOJA_TEMPLATES_Flintdata_Headers})
source_group("source files\\flintdata"			FILES ${MOJA_TEMPLATES_Flintdata_Sources})

set(SRCS
	${MOJA_TEMPLATES_sources} ${MOJA_TEMPLATES_headers}  ${MOJA_TEMPLATES_Transform_Headers} ${MOJA_TEMPLATES_Flintdata_Headers}
	${MOJA_TEMPLATES_Modules_sources} ${MOJA_TEMPLATES_Modules_headers} ${MOJA_TEMPLATES_Transform_Sources} ${MOJA_TEMPLATES_Flintdata_Sources})

add_library(${LIBNAME} ${LIB_MODE} ${SRCS} ${ProductVersionFiles})
add_library(${PROJECT_NAME}::${LIBNAME} ALIAS ${LIBNAME})

target_include_directories(${LIBNAME}
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_target_properties(${LIBNAME} 
    PROPERTIES
    VERSION ${CR_VERSION} 
	SOVERSION ${CR_VERSION_MAJOR}
    DEFINE_SYMBOL ${LIBNAME_EXPORT}_EXPORTS)

target_compile_features(${LIBNAME} PUBLIC cxx_std_17)
set_target_properties(${LIBNAME} PROPERTIES CXX_EXTENSIONS OFF)	

target_link_libraries(${LIBNAME} 
		fmt::fmt-header-only
		moja::moja.flint 
)

if (ENABLE_TESTS)
	add_subdirectory(tests)
endif()

install(TARGETS ${LIBNAME}
		EXPORT ${LIBNAME}Targets
        LIBRARY DESTINATION lib${LIB_SUFFIX}
        ARCHIVE DESTINATION lib${LIB_SUFFIX}
        RUNTIME DESTINATION bin
		INCLUDES DESTINATION include)

if(MSVC)
	INSTALL(
		FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/${LIBNAME}${CMAKE_DEBUG_POSTFIX}.pdb 
		DESTINATION bin 
		CONFIGURATIONS Debug
		)
endif()

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${LIBNAME}ConfigVersion.cmake
    VERSION ${CR_VERSION}
    COMPATIBILITY AnyNewerVersion
    )

install(EXPORT ${LIBNAME}Targets
        FILE ${LIBNAME}Targets.cmake
        NAMESPACE ${LIBNAME}::
        DESTINATION lib/cmake/${LIBNAME}
         )

configure_file(${LIBNAME}Config.cmake.in ${LIBNAME}Config.cmake @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${LIBNAME}Config.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/${LIBNAME}ConfigVersion.cmake"
        DESTINATION lib/cmake/${LIBNAME}
        )