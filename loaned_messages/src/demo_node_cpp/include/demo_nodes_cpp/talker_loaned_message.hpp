#ifndef DEMO_NODES_CPP__TALKER_LOANED_MESSAGE_HPP_
#define DEMO_NODES_CPP__TALKER_LOANED_MESSAGE_HPP_

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
#include "std_msgs/msg/string.hpp"
#include "custom_msgs/msg/loaned_message.hpp"
#include "demo_nodes_cpp/visibility_control.h"

namespace demo_nodes_cpp
{

class LoanedMessageTalker : public rclcpp::Node
{
public:
  DEMO_NODES_CPP_PUBLIC
  explicit LoanedMessageTalker(const rclcpp::NodeOptions & options);

private:
  void on_message(const custom_msgs::msg::LoanedMessage::SharedPtr msg);
  void mean_latency_writer();

  int id_ = 0;
  double total_latency_ = 0.0;
  size_t received_count_ = 0;
  double min_latency_ = 0.0;
  double max_latency_ = 0.0;

  size_t count_ = 1;
  rclcpp::Publisher<custom_msgs::msg::LoanedMessage>::SharedPtr pod_pub_;
  rclcpp::Subscription<custom_msgs::msg::LoanedMessage>::SharedPtr sub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace demo_nodes_cpp

#endif  // DEMO_NODES_CPP__TALKER_LOANED_MESSAGE_HPP_
