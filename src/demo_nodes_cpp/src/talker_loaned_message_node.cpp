#include "demo_nodes_cpp/talker_loaned_message.hpp"
#include "demo_nodes_cpp/visibility_control.h"
#include <rclcpp/rclcpp.hpp>

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<demo_nodes_cpp::LoanedMessageTalker>(rclcpp::NodeOptions());
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}