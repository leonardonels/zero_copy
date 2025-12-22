#ifndef DEMO_NODES_CPP__TALKER_LOANED_MESSAGE_HPP_
#define DEMO_NODES_CPP__TALKER_LOANED_MESSAGE_HPP_

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
#include "std_msgs/msg/string.hpp"
#include "demo_nodes_cpp/visibility_control.h"

namespace demo_nodes_cpp
{

class LoanedMessageTalker : public rclcpp::Node
{
public:
  DEMO_NODES_CPP_PUBLIC
  explicit LoanedMessageTalker(const rclcpp::NodeOptions & options);

private:
  size_t count_ = 1;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pod_pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace demo_nodes_cpp

#endif  // DEMO_NODES_CPP__TALKER_LOANED_MESSAGE_HPP_
