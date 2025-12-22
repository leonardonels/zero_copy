cmake_minimum_required(VERSION 3.8)
project(loaned_message_demo)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(custom_msgs REQUIRED)
find_package(rclcpp_components REQUIRED)

include_directories(include)

add_library(loaned_message_demo SHARED
  src/talker_loaned_message.cpp
)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

ament_target_dependencies(loaned_message_demo
  rclcpp
  std_msgs
  custom_msgs
  rclcpp_components
)

add_executable(talker_loaned_message_node src/talker_loaned_message_node.cpp)
target_link_libraries(talker_loaned_message_node loaned_message_demo)

ament_target_dependencies(talker_loaned_message_node
  rclcpp
  std_msgs
  custom_msgs
  rclcpp_components
)

install(TARGETS loaned_message_demo
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(TARGETS talker_loaned_message_node
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
