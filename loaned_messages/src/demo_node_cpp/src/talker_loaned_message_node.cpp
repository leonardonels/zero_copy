#include "demo_nodes_cpp/talker_loaned_message.hpp"
#include "demo_nodes_cpp/visibility_control.h"
#include <rclcpp/rclcpp.hpp>
#include <vector>
#include <string>

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);

  // Create a temporary node to read the 'nodes' parameter
  // Users can pass it via: --ros-args -p nodes:=10
  // ros2 run loaned_message_demo talker_loaned_message_node --ros-args -p nodes:=10
  auto param_node = std::make_shared<rclcpp::Node>("loaned_message_launcher");
  param_node->declare_parameter("nodes", 5);
  int num_nodes = param_node->get_parameter("nodes").as_int();

  ++num_nodes;

  RCLCPP_INFO(rclcpp::get_logger("loaned_message_demo"), "Starting %d nodes...", num_nodes);

  rclcpp::executors::MultiThreadedExecutor executor;
  std::vector<std::shared_ptr<demo_nodes_cpp::LoanedMessageTalker>> nodes;

  for (int i = 0; i < num_nodes; ++i) {
    rclcpp::NodeOptions options;
    options.append_parameter_override("id", i);
    
    // Remap node name to ensure uniqueness
    std::string node_name = "loaned_message_talker_" + std::to_string(i);
    std::vector<std::string> remap_args = {"--ros-args", "-r", "__node:=" + node_name};
    options.arguments(remap_args);

    auto node = std::make_shared<demo_nodes_cpp::LoanedMessageTalker>(options);
    nodes.push_back(node);
    executor.add_node(node);
  }

  executor.spin();
  rclcpp::shutdown();
  return 0;
}