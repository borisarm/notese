add_executable(app_tests
	NavigationServiceProducesExpectedActionsTests.cpp
	NavigationServiceCoversAllBranchesTests.cpp
	ActionExecutorExecutesVariantActionsTests.cpp
	ActionExecutorCoversRemainingVariantsTests.cpp
	NoteWorkflowServicePersistsAndRefreshesStateTests.cpp
	ReminderWorkflowServicePersistsAndRefreshesStateTests.cpp
	WorkflowServicesEditPathAndErrorsTests.cpp
	ViewRouterHandlesConfirmEmptyStateTests.cpp
	TuiStateEnforceInvariantsTests.cpp
	RenderViewsForAllModesTests.cpp
	CliRunHandlesAllCommandsTests.cpp)

target_include_directories(app_tests PRIVATE
	${CMAKE_SOURCE_DIR}/src/app/cli
	${CMAKE_SOURCE_DIR}/src/app/tui
	${CMAKE_SOURCE_DIR}/src/domain/include
	${CMAKE_SOURCE_DIR}/src/infrastructure/include)

target_link_libraries(app_tests PRIVATE
	notese_domain
	notese_infrastructure
	GTest::gtest_main
	ftxui::dom
	ftxui::screen
	ftxui::component)

include(GoogleTest)
gtest_discover_tests(app_tests)
