# Configure Zero Copy Loaned Messages

ROS 2 loaned messages and zero copy data sharing are mechanisms designed to improve performance by minimizing data copying. When using loaned messages, the RMW middleware can allocate and manage message memory, allowing publishers and subscribers to share data buffers directly. This reduces the overhead associated with memory allocation and data copying, leading to lower latency and higher throughput. Zero copy data sharing is particularly beneficial in high-performance applications where large amounts of data need to be transmitted efficiently.

## Prerequisites

If you’ve installed ROS 2 from packages, ensure that you have `ros-humble-demo-nodes-cpp` installed. If you downloaded the archive or built ROS 2 from source, it will already be part of the installation.

## Running the Demo

To test the loaned messages feature, you can run the following demo node:

```bash
ros2 ros2 run loaned_message_demo talker_loaned_message_node --ros-args -p nodes:=10
```

### Expected Behavior (Without Configuration)

If the environment is not yet configured for zero copy, you will see the following log message indicating that the local allocator is being used instead of loaned messages:

```text
[INFO] [1741064109.676860153] [rclcpp]: Currently used middleware cannot loan messages. Local allocator will be used.
```

## Configuration

To enable zero copy loaned messages, you need to configure the environment variables and the Fast DDS XML profile.

### 1. Environment Variables

Set the following environment variables to enable the feature and point to the configuration file:

```bash
export RMW_FASTRTPS_USE_QOS_FROM_XML=1
export ROS_DISABLE_LOANED_MESSAGE=0
export RMW_IMPLEMENTATION=rmw_fastrtps_cpp
export ROS_DOMAIN_ID=0
export FASTRTPS_DEFAULT_PROFILES_FILE=path_to_fastDDS_setup.xml
```

> **Note**: Ensure `FASTRTPS_DEFAULT_PROFILES_FILE` points to the correct path of your `fastDDS_setup.xml`.

### 2. Fast DDS XML Configuration

Create a file named `fastDDS_setup.xml` with the following content. This configures Fast DDS to publish synchronously, use a pre-allocated history, and enable Data Sharing (Zero Copy) where possible.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<dds xmlns="http://www.eprosima.com/XMLSchemas/fastRTPS_Profiles">
    <profiles>
        <!-- Default publisher profile -->
        <publisher profile_name="default publisher profile" is_default_profile="true">
            <qos>
                <publishMode>
                    <kind>SYNCHRONOUS</kind>
                </publishMode>
                <data_sharing>
                    <kind>AUTOMATIC</kind>
                </data_sharing>
            </qos>
            <historyMemoryPolicy>PREALLOCATED_WITH_REALLOC</historyMemoryPolicy>
        </publisher>

        <!-- Default subscriber profile -->
        <subscriber profile_name="default subscriber profile" is_default_profile="true">
            <qos>
                <data_sharing>
                    <kind>AUTOMATIC</kind>
                </data_sharing>
            </qos>
            <historyMemoryPolicy>PREALLOCATED_WITH_REALLOC</historyMemoryPolicy>
        </subscriber>

        <!-- Publisher profile for topic helloworld -->
        <publisher profile_name="helloworld">
            <qos>
                <publishMode>
                    <kind>SYNCHRONOUS</kind>
                </publishMode>
                <data_sharing>
                    <kind>AUTOMATIC</kind>
                </data_sharing>
            </qos>
        </publisher>

        <!-- Request subscriber profile for services -->
        <subscriber profile_name="service">
            <qos>
                <data_sharing>
                    <kind>AUTOMATIC</kind>
                </data_sharing>
            </qos>
            <historyMemoryPolicy>PREALLOCATED_WITH_REALLOC</historyMemoryPolicy>
        </subscriber>

        <!-- Request publisher profile for clients -->
        <publisher profile_name="client">
            <qos>
                <publishMode>
                    <kind>ASYNCHRONOUS</kind>
                </publishMode>
                <data_sharing>
                    <kind>AUTOMATIC</kind>
                </data_sharing>
            </qos>
        </publisher>

        <!-- Request subscriber profile for server of service "add_two_ints" -->
        <subscriber profile_name="rq/add_two_intsRequest">
            <qos>
                <data_sharing>
                    <kind>AUTOMATIC</kind>
                </data_sharing>
            </qos>
            <historyMemoryPolicy>PREALLOCATED_WITH_REALLOC</historyMemoryPolicy>
        </subscriber>

        <!-- Reply subscriber profile for client of service "add_two_ints" -->
        <subscriber profile_name="rr/add_two_intsReply">
            <qos>
                <data_sharing>
                    <kind>AUTOMATIC</kind>
                </data_sharing>
            </qos>
            <historyMemoryPolicy>PREALLOCATED_WITH_REALLOC</historyMemoryPolicy>
        </subscriber>
    </profiles>
</dds>
```

## Example Code: `talker_loaned_message`

Here is the source code for the `talker_loaned_message` demo node, showing how to use the `borrow_loaned_message()` API.

```cpp
#include <chrono>
#include <cstdio>
#include <memory>
#include <utility>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_components/register_node_macro.hpp"

#include "std_msgs/msg/float64.hpp"
#include "std_msgs/msg/string.hpp"

#include "demo_nodes_cpp/visibility_control.h"

using namespace std::chrono_literals;

namespace demo_nodes_cpp
{
// Create a Talker class that subclasses the generic rclcpp::Node base class.
// The main function below will instantiate the class as a ROS node.
class LoanedMessageTalker : public rclcpp::Node
{
public:
  DEMO_NODES_CPP_PUBLIC
  explicit LoanedMessageTalker(const rclcpp::NodeOptions & options)
  : Node("loaned_message_talker", options)
  {
    // Create a function for when messages are to be sent.
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);

    // We differentiate in this demo between two fundamental message types - POD and non-POD
    // PODs are plain old data types, meaning all the data of its type is encapsulated within
    // the structure and does not require any heap allocation or dynamic resizing.
    // non-PODs are essentially the opposite where the data size changes during runtime.
    // All containers (including Strings) are such non-PODs.
    // Most middlewares won't be able to loan non-POD datatypes.
    // We thus feature two publishers in this demo where both, a POD and non-POD message
    // will be used to publish data.
    // The take-away for this is that the rclcpp API for message loaning can cope with
    // either POD and non-POD transparently.
    auto publish_message =
      [this]() -> void
      {
        // We loan a message here and don't allocate the memory on the stack.
        // For middlewares which support message loaning, this means the middleware
        // completely owns the memory for this message.
        // This enables a zero-copy message transport for middlewares with shared memory
        // capabilities.
        // If the middleware doesn't support this, the loaned message will be allocated
        // with the allocator instance provided by the publisher.
        auto pod_loaned_msg = pod_pub_->borrow_loaned_message();
        auto pod_msg_data = static_cast<double>(count_);
        pod_loaned_msg.get().data = pod_msg_data;
        RCLCPP_INFO(this->get_logger(), "Publishing: '%f'", pod_msg_data);
        // As the middleware might own the memory allocated for this message,
        // a call to publish explicitly transfers ownership back to the middleware.
        // The loaned message instance is thus no longer valid after a call to publish.
        pod_pub_->publish(std::move(pod_loaned_msg));

        // Similar as in the above case, we ask the middleware to loan a message.
        // As most likely the middleware won't be able to loan a message for a non-POD
        // data type, the memory for the message will be allocated on the heap within
        // the scope of the `LoanedMessage` instance.
        // After the call to `publish()`, the message will be correctly allocated.
        auto non_pod_loaned_msg = non_pod_pub_->borrow_loaned_message();
        auto non_pod_msg_data = "Hello World: " + std::to_string(count_);
        non_pod_loaned_msg.get().data = non_pod_msg_data;
        RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", non_pod_msg_data.c_str());
        non_pod_pub_->publish(std::move(non_pod_loaned_msg));
        count_++;
      };

    // Create a publisher with a custom Quality of Service profile.
    rclcpp::QoS qos(rclcpp::KeepLast(7));
    pod_pub_ = this->create_publisher<std_msgs::msg::Float64>("chatter_pod", qos);
    non_pod_pub_ = this->create_publisher<std_msgs::msg::String>("chatter", qos);

    // Use a timer to schedule periodic message publishing.
    timer_ = this->create_wall_timer(1s, publish_message);
  }

private:
  size_t count_ = 1;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pod_pub_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr non_pod_pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace demo_nodes_cpp
```

## Resources

- [ROS Humble zero copy loaned messages configuration](https://docs.ros.org/en/humble/How-To-Guides/Configure-ZeroCopy-loaned-messages.html)
- [Enable zero copy Data Sharing](https://github.com/ros2/rmw_fastrtps?tab=readme-ov-file#enable-zero-copy-data-sharing)
- [ROS 2 Design: Zero Copy](https://design.ros2.org/articles/zero_copy.html)
- [ROS 2 Demos](https://github.com/ros2/demos.git) Remember to select the branch called Humble
- [ros2_shm_msgs by ZhenshengLee](https://github.com/ZhenshengLee/ros2_shm_msgs)
- [Fast DDS Data Sharing](https://fast-dds.docs.eprosima.com/en/latest/fastdds/transport/datasharing.html)

