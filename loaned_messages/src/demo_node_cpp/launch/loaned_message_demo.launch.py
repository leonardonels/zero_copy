from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='loaned_message_demo',
            executable='talker_loaned_message_node',
            name='talker_loaned_message',
            output='screen',
        )
    ])