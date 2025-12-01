cmake_minimum_required(VERSION 3.8)
project(demo_nodes_cpp)

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
find_package(rclcpp_components REQUIRED)

include_directories(include)

add_library(demo_nodes_cpp SHARED
  src/talker_loaned_message.cpp
)

ament_target_dependencies(demo_nodes_cpp
  rclcpp
  std_msgs
  rclcpp_components
)

add_executable(talker_loaned_message_node src/talker_loaned_message_node.cpp)
target_link_libraries(talker_loaned_message_node demo_nodes_cpp)

ament_target_dependencies(talker_loaned_message_node
  rclcpp
  std_msgs
  rclcpp_components
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
