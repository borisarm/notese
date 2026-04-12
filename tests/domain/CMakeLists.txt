add_executable(notese_domain_tests
	NoteTests.cpp
)

target_link_libraries(notese_domain_tests
	PRIVATE
		notese_domain
		GTest::gtest_main
)

add_test(NAME domain_tests COMMAND notese_domain_tests)
