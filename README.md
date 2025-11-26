# Zero Copy Experiments

This repository contains experiments and references for implementing Zero Copy in ROS 2, specifically using Fast DDS.

## References

*   [DemoZeroCopyRos2 by MatteGombia](https://github.com/MatteGombia/DemoZeroCopyRos2)
*   [RMW Fast RTPS: Enable Zero Copy Data Sharing](https://github.com/ros2/rmw_fastrtps?tab=readme-ov-file#enable-zero-copy-data-sharing)
*   [ros2_shm_msgs by ZhenshengLee](https://github.com/ZhenshengLee/ros2_shm_msgs)
*   [ROS 2 Design: Zero Copy](https://design.ros2.org/articles/zero_copy.html)
*   [Fast DDS Data Sharing](https://fast-dds.docs.eprosima.com/en/latest/fastdds/transport/datasharing.html)
*   [ROS 2 Guide: Configure ZeroCopy Loaned Messages](https://docs.ros.org/en/humble/How-To-Guides/Configure-ZeroCopy-loaned-messages.html)

## Notes

### Fast DDS Data Sharing Configuration

According to the [Fast DDS documentation](https://fast-dds.docs.eprosima.com/en/latest/fastdds/transport/datasharing.html):
> "ON: Like AUTO, but the creation of the entity will fail if the requirements are not met."

**Issue:** Setting it to `ON` caused a crash.

### TODO

1.  **Verify Address Sharing:**
    Check if the address where the publisher reads matches where the listener receives.
    
    Code snippet for verification:
    ```cpp
    [] (std_msgs::msg::Int32::UniquePtr msg) {
        printf("Received message with value: %d, and address: 0x%" PRIXPTR "\n", 
        msg->data, 
        reinterpret_cast<std::uintptr_t>(msg.get()));
    }
    ```
    *Expectation:* If Zero Copy is working, the addresses should be the same (or related to the same shared memory segment).

2.  **Configuration Fix:**
    If the addresses are different, the configuration might be incorrect.
    *   Action: Modify the XML configuration file to correctly configure data sharing.
    *   Reference: Check the configuration used in the linked video.
