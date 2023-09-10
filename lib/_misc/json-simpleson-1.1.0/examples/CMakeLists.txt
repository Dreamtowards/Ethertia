include_directories(../)

file(GLOB examples
    "*.cpp"
)

foreach(example ${examples})
    string(REGEX REPLACE ".*/" "" example_name "${example}")
    string(REGEX REPLACE ".cpp$" "" example_name "${example_name}")
    add_executable ("${example_name}" ${example})
    target_link_libraries("${example_name}" simpleson)
	if(MSVC)
		set_property(TARGET "${example_name}" PROPERTY _CRT_SECURE_NO_WARNINGS)
	endif()
endforeach()