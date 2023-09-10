include_directories(../)

file(GLOB tests
    "*.cpp"
)

foreach(test ${tests})
    string(REGEX REPLACE ".*/" "" test_name "${test}")
    string(REGEX REPLACE ".cpp$" "" test_name "${test_name}")
    add_executable ("issue_${test_name}" ${test})
    target_link_libraries("issue_${test_name}" simpleson)
	if(MSVC)
		set_property(TARGET "issue_${test_name}" PROPERTY _CRT_SECURE_NO_WARNINGS)
	endif()
    add_test(NAME ${test_name} COMMAND "issue_${test_name}")
endforeach()