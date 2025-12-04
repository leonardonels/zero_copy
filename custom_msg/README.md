# Create Custom Messages

## Message Structure
We will define a custom message (e.g., `Test.msg`) with the following fields:

```text
int64 count
int64 size
int8[2764800] data
```

### Field Description
- **count**: Sequence number or identifier.
- **size**: Metadata indicating the size of useful data.
- **data**: A fixed-size array of **2,764,800 bytes** (approx. 2.64 MB).
  - This specific size corresponds to a raw **720p RGB image** ($1280 \times 720 \times 3$).
  - It is large enough (larger than many typical PointCloud2 messages) to make memory copy overhead noticeable (eg. with fast_dds_monitor), making it an excellent candidate for testing Zero Copy.

## Implementation Plan
1.  **Define the Interface**: Create the `.msg` file in this package.
2.  **Build the Package**: Configure `CMakeLists.txt` and `package.xml` to generate the C++ interfaces.
3.  **Create a Subscriber**: Implement a `minimal_subscriber` (similar to the ROS 2 tutorials) that listens to this topic.
4.  **Integration**: Use this subscriber to verify the `loaned_messages` talker node.

## References
- [ROS 2 Custom Messages Tutorial](https://docs.ros.org/en/humble/Tutorials/Beginner-Client-Libraries/Custom-ROS2-Interfaces.html)
- [rclcpp LoanedMessage Documentation](https://docs.ros.org/en/humble/p/rclcpp/generated/classrclcpp_1_1LoanedMessage.html)
- [Writing a Simple Publisher and Subscriber (C++)](https://docs.ros.org/en/humble/Tutorials/Beginner-Client-Libraries/Writing-A-Simple-Cpp-Publisher-And-Subscriber.html)
- [sensor_msgs/PointCloud2 Documentation](https://docs.ros.org/en/noetic/api/sensor_msgs/html/msg/PointCloud2.html)