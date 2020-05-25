### Unit test ###
set(TESTUNIT "${LIBNAME}.test")

find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)

find_package(Turtle)

find_package(Moja)

configure_file(../../templates/unittestdefinition.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/_unittestdefinition.cpp)

set(TEST_SRCS
    src/_unittestdefinition.cpp
    src/growthmoduletests.cpp
	)

add_executable(${TESTUNIT} ${TEST_SRCS})
add_test(NAME ${LIBNAME} 
         WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
         COMMAND ${TESTUNIT} --result_code=yes --report_level=no)

target_compile_features(${TESTUNIT} PUBLIC cxx_std_14)
set_target_properties(${TESTUNIT} PROPERTIES CXX_EXTENSIONS OFF)	

target_compile_definitions(${TESTUNIT} PUBLIC BOOST_TEST_DYN_LINK)

target_include_directories(${TESTUNIT}
	PRIVATE
		${TURTLE_INCLUDE_PATH}
		${Moja_INCLUDE_DIRS})

target_link_libraries(${TESTUNIT}
                      ${LIBNAME}
					  moja::moja.flint 
					  moja.chapman_richards
					  chapman_richards::moja.modules.chapman_richards
					  Boost::unit_test_framework
					  )


IF (RUN_UNIT_TESTS_ON_BUILD)
add_custom_command(TARGET ${TESTUNIT}
                   POST_BUILD COMMAND ${CMAKE_CTEST_COMMAND}
                   ARGS -C $<CONFIGURATION>)
ENDIF ()
