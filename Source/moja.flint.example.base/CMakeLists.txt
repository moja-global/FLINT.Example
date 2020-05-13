set(PACKAGE "base")
set(LIBNAME "moja.flint.example.${PACKAGE}")
string(REPLACE "." "_" NEW_PACKAGE "${PACKAGE}")
string(TOUPPER "${NEW_PACKAGE}" LIBNAME_EXPORT)

find_package(Poco REQUIRED Foundation JSON Data DataSQLite)
find_package(Boost COMPONENTS log log_setup REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(moja REQUIRED COMPONENTS moja.flint)
#find_package(GDAL REQUIRED)

find_package(OpenSSL REQUIRED)

# HEADERS AND SOURCE

configure_file(../templates/exports.h ${CMAKE_CURRENT_SOURCE_DIR}/include/moja/flint/example/${PACKAGE}/_modules.${PACKAGE}_exports.h)

set(PROJECT_HEADERS
    include/moja/flint/example/${PACKAGE}/_modules.${PACKAGE}_exports.h
    include/moja/flint/example/${PACKAGE}/libraryfactory.h
)

set(PROJECT_SOURCES
    src/libraryfactory.cpp
)

set(PROJECT_MODULE_HEADERS
	#include/moja/flint/example/${PACKAGE}/xxx.h
	include/moja/flint/example/${PACKAGE}/errorscreenwriter.h
)
 
set(PROJECT_MODULE_SOURCES
	#src/xxx.cpp
	src/errorscreenwriter.cpp
)
 
set(PROJECT_TRANSFORM_HEADERS
	#include/moja/flint/example/${PACKAGE}/xxx.h
)

set(PROJECT_TRANSFORM_SOURCES
	#src/xxx.cpp
)
 
set(PROJECT_FLINTDATA_HEADERS
	#include/moja/flint/example/${PACKAGE}/xxx.h
)

set(PROJECT_FLINTDATA_SOURCES
	#src/xxx.cpp
)
 
set(PROJECT_PROVIDER_HEADERS
)

set(PROJECT_PROVIDER_SOURCES
)

source_group("header files\\other"					FILES ${PROJECT_HEADERS})
source_group("source files\\other"					FILES ${PROJECT_SOURCES})
source_group("header files\\modules"				FILES ${PROJECT_MODULE_HEADERS})
source_group("source files\\modules"				FILES ${PROJECT_MODULE_SOURCES})
source_group("header files\\transforms"				FILES ${PROJECT_TRANSFORM_HEADERS})
source_group("source files\\transforms"				FILES ${PROJECT_TRANSFORM_SOURCES})
source_group("header files\\flintdata"				FILES ${PROJECT_FLINTDATA_HEADERS})
source_group("source files\\flintdata"				FILES ${PROJECT_FLINTDATA_SOURCES})
source_group("header files\\providers"				FILES ${PROJECT_PROVIDER_HEADERS})
source_group("source files\\providers"				FILES ${PROJECT_PROVIDER_SOURCES})

set(SRCS 
	${PROJECT_SOURCES} ${PROJECT_HEADERS}
	${PROJECT_MODULE_SOURCES} ${PROJECT_MODULE_HEADERS}
	${PROJECT_TRANSFORM_SOURCES} ${PROJECT_TRANSFORM_HEADERS}
	${PROJECT_FLINTDATA_SOURCES} ${PROJECT_FLINTDATA_HEADERS}
    ${PROJECT_PROVIDER_HEADERS} ${PROJECT_PROVIDER_SOURCES}
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

target_link_libraries(${LIBNAME} 
	PUBLIC
		moja::moja.flint PostgreSQL::PostgreSQL Poco::Foundation Poco::JSON Poco::Data Poco::DataSQLite
		OpenSSL::SSL OpenSSL::Crypto
	PRIVATE
#		${GDAL_LIBRARY}
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
