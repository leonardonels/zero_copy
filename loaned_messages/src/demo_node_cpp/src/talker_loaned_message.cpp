#include <chrono>
#include <cstdio>
#include <memory>
#include <utility>
#include <vector>
#include <cstring>
#include <limits>

#include "demo_nodes_cpp/talker_loaned_message.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_components/register_node_macro.hpp"

#include "std_msgs/msg/float64.hpp"
#include "std_msgs/msg/string.hpp"
#include "custom_msgs/msg/loaned_message.hpp"

#include "demo_nodes_cpp/visibility_control.h"

#include "demo_nodes_cpp/sim.hpp"

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
        thread_ = std::thread([this]() {
            // SIM
            SIM::Reader reader("/sim_channel", 1024 * 1024 * 100); // 100 MB

            while (rclcpp::ok() && !reader.init()) {
                RCLCPP_WARN(this->get_logger(), "Waiting for SIM writer...");
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            
            if (!rclcpp::ok()) return;

            RCLCPP_INFO(this->get_logger(), "SIM reader initialized on /sim_channel");
            
            uint64_t count = 0;
            uint64_t avg_count = 0;
            double total_latency = 0.0;
            double min_latency = std::numeric_limits<double>::max();
            double max_latency = 0.0;

            while (rclcpp::ok()) {
                size_t size = 0;
            
                // Zero-copy read - get direct pointer to shared memory
                const void* data = reader.readZeroCopy(size);
            
                if (data && size >= sizeof(int64_t)) {
                    int64_t sent_time;
                    std::memcpy(&sent_time, data, sizeof(int64_t));
                    
                    int64_t now_ns = this->now().nanoseconds();;
                    double latency_ms = (now_ns - sent_time);
                    
                    total_latency += latency_ms;
                    if (latency_ms < min_latency) min_latency = latency_ms;
                    if (latency_ms > max_latency) max_latency = latency_ms;

                    count++;
                    avg_count++;
                    if (count % 100 == 0) {
                        RCLCPP_INFO(this->get_logger(), 
                            "Stats (last 100): Avg: %.5f ms, Min: %.5f ms, Max: %.5f ms",
                            (total_latency / avg_count) / 1000000.0, min_latency / 1000000.0, max_latency / 1000000.0);
                        
                        // Reset stats
                        avg_count = 0;
                        total_latency = 0.0;
                        min_latency = std::numeric_limits<double>::max();
                        max_latency = 0.0;
                    }else{
                        RCLCPP_INFO(this->get_logger(), "Frame %lu latency: %.5f ms", count, latency_ms / 1000000.0);
                    }
                }

                // Check if writer is still alive
                if (!reader.isWriterAlive(2000)) {
                    RCLCPP_WARN(this->get_logger(), "Writer disconnected.");
                    break;
                }

                std::this_thread::yield();
            }
        });
    } else {
        thread_ = std::thread([this]() {
            //SIM
            SIM::Writer writer("/sim_channel", 1024 * 1024 * 100); // 100 MB
            if (!writer.init()) {
                RCLCPP_ERROR(this->get_logger(), "Failed to initialize SIM writer");
                return;
            }
            RCLCPP_INFO(this->get_logger(), "SIM writer initialized on /sim_channel");
            
            uint64_t frame_number = 0;
            std::vector<char> buffer(5529600); // ~5.5 MB buffer

            while (rclcpp::ok()) {
                int64_t now_ns = this->now().nanoseconds();
                
                // Copy timestamp to beginning of buffer
                std::memcpy(buffer.data(), &now_ns, sizeof(int64_t));
                
                // Add frame info text after timestamp
                snprintf(buffer.data() + sizeof(int64_t), buffer.size() - sizeof(int64_t), 
                        " - Frame %lu", frame_number);

                // Write data (timestamp + string + null terminator)
                size_t payload_len = strlen(buffer.data() + sizeof(int64_t));
                writer.write(buffer.data(), sizeof(int64_t) + payload_len + 1);
                
                // printf("Published frame %lu\n", frame_number);
                ++frame_number;
                
                std::this_thread::sleep_for(std::chrono::milliseconds(10));  // ~100 Hz
            }

            writer.destroy();
        });
    }
}

}  // namespace demo_nodes_cpp

RCLCPP_COMPONENTS_REGISTER_NODE(demo_nodes_cpp::LoanedMessageTalker)