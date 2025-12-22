#include <chrono>
#include <cstdio>
#include <memory>
#include <utility>

#include "demo_nodes_cpp/talker_loaned_message.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_components/register_node_macro.hpp"

#include "std_msgs/msg/float64.hpp"
#include "std_msgs/msg/string.hpp"
#include "custom_msgs/msg/loaned_message.hpp"

#include "demo_nodes_cpp/visibility_control.h"

using namespace std::chrono_literals;

namespace demo_nodes_cpp
{

LoanedMessageTalker::LoanedMessageTalker(const rclcpp::NodeOptions & options):Node("loaned_message_talker", options)
{
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);

    this->declare_parameter("id", 0);
    id_ = this->get_parameter("id").as_int();

    rclcpp::QoS qos(rclcpp::KeepLast(1));

    if (id_ == 0) {
        // Father node
        sub_ = this->create_subscription<custom_msgs::msg::LoanedMessage>(
            "chatter_pod", qos, std::bind(&LoanedMessageTalker::on_message, this, std::placeholders::_1));
        RCLCPP_INFO(this->get_logger(), "Father node started (ID: 0), listening on 'chatter_pod'");
    } else {
        // Child node
        auto publish_message =
          [this]() -> void
          {
            auto pod_loaned_msg = pod_pub_->borrow_loaned_message();
            
            // Data is fixed size [2764800]
            pod_loaned_msg.get().data[0] = static_cast<uint8_t>(id_);
            
            // Populate timestamp
            pod_loaned_msg.get().timestamp = this->now().nanoseconds();
            
            // RCLCPP_INFO(this->get_logger(), "Child %d Publishing message", id_);
            pod_pub_->publish(std::move(pod_loaned_msg));
          };

        // Create a publisher with a custom Quality of Service profile.
        pod_pub_ = this->create_publisher<custom_msgs::msg::LoanedMessage>("chatter_pod", qos);

        // Use a timer to schedule periodic message publishing.
        timer_ = this->create_wall_timer(0.1s, publish_message);
        RCLCPP_INFO(this->get_logger(), "Child node %d started, publishing to 'chatter_pod'", id_);
    }
}

void LoanedMessageTalker::on_message(const custom_msgs::msg::LoanedMessage::SharedPtr msg)
{
    double current_time = this->now().nanoseconds();
    double sent_time = msg->timestamp;
    double latency = current_time - sent_time;
    
    total_latency_ += latency;
    received_count_++;

    if (received_count_ % 100 == 0) {
        double average_latency = total_latency_ / received_count_;
        RCLCPP_INFO(this->get_logger(), 
            "Received %zu messages. Latest Latency: %f ms. Average Latency: %f ms", 
            received_count_, latency / 1000000, average_latency / 1000000);
        min_latency_ = 0.0;
        max_latency_ = 0.0;
    }

    if (min_latency_ == 0.0 && max_latency_ == 0.0) {
        min_latency_ = latency;
        max_latency_ = latency;
    } else {
        min_latency_ = std::min(min_latency_, latency);
        max_latency_ = std::max(max_latency_, latency);
    }
    RCLCPP_INFO(this->get_logger(),
        "Message from ID: %d | Latency: %f ms | Min Latency: %f ms | Max Latency: %f ms",
        static_cast<int>(msg->data[0]),
        latency / 1000000,
        min_latency_ / 1000000,
        max_latency_ / 1000000);
}

}  // namespace demo_nodes_cpp

RCLCPP_COMPONENTS_REGISTER_NODE(demo_nodes_cpp::LoanedMessageTalker)