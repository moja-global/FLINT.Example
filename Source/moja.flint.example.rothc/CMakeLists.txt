set(PACKAGE "rothc")
set(LIBNAME "moja.flint.example.${PACKAGE}")
string(REPLACE "." "_" NEW_PACKAGE "${PACKAGE}")
string(TOUPPER "${NEW_PACKAGE}" LIBNAME_EXPORT)

find_package(Poco REQUIRED Foundation JSON Data DataSQLite)
find_package(Boost COMPONENTS log log_setup REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(moja REQUIRED COMPONENTS moja.flint)

configure_file(../templates/exports.h ${CMAKE_CURRENT_SOURCE_DIR}/include/moja/flint/example/${PACKAGE}/_modules.${PACKAGE}_exports.h)

# HEADERS AND SOURCE

set(PROJECT_HEADERS
    include/moja/flint/example/${PACKAGE}/_modules.${PACKAGE}_exports.h
    include/moja/flint/example/${PACKAGE}/libraryfactory.h
)

set(PROJECT_SOURCES
    src/libraryfactory.cpp
)

set(PROJECT_MODULE_HEADERS
	include/moja/flint/example/${PACKAGE}/plantresiduemodule.h
	include/moja/flint/example/${PACKAGE}/rothcmodule.h
	include/moja/flint/example/${PACKAGE}/soilcovermodule.h
)
 
set(PROJECT_MODULE_SOURCES
	src/plantresiduemodule.cpp
	src/rothcmodule.cpp
	src/soilcovermodule.cpp
)
 
source_group("header files\\other"					FILES ${PROJECT_HEADERS})
source_group("source files\\other"					FILES ${PROJECT_SOURCES})
source_group("header files\\modules"				FILES ${PROJECT_MODULE_HEADERS})
source_group("source files\\modules"				FILES ${PROJECT_MODULE_SOURCES})

set(SRCS 
	${PROJECT_SOURCES} ${PROJECT_HEADERS}
	${PROJECT_MODULE_SOURCES} ${PROJECT_MODULE_HEADERS}
)

add_library(${LIBNAME} ${LIB_MODE} ${SRCS})
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
    VERSION ${MOJA_FLINT_EXAMPLE_VERSION} 
	SOVERSION ${MOJA_FLINT_EXAMPLE_VERSION_MAJOR}
    DEFINE_SYMBOL ${LIBNAME_EXPORT}_EXPORTS)

target_compile_features(${LIBNAME} PUBLIC cxx_std_17)
set_target_properties(${LIBNAME} PROPERTIES CXX_EXTENSIONS OFF)	

target_link_libraries(${LIBNAME} 
	PUBLIC
		fmt::fmt-header-only
		moja::moja.flint 
		Poco::Foundation Poco::JSON Poco::Data Poco::DataSQLite
	PRIVATE
	)
if(WIN32)
	# Needed for socket() on Windows
	target_link_libraries(${LIBNAME} PRIVATE ws2_32 Iphlpapi)
endif(WIN32)

install(TARGETS ${LIBNAME}
        LIBRARY DESTINATION lib${LIB_SUFFIX}
        ARCHIVE DESTINATION lib${LIB_SUFFIX}
        RUNTIME DESTINATION bin)

if(MSVC)
	INSTALL(
		FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/${LIBNAME}${CMAKE_DEBUG_POSTFIX}.pdb 
		DESTINATION bin 
		CONFIGURATIONS Debug
		)
endif()

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()
