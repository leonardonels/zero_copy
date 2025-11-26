# Fast DDS Monitor Setup Guide

Fast DDS Monitor is a graphical desktop application that provides real-time monitoring of Fast DDS environments.

## Prerequisites

To use Fast DDS Monitor with ROS 2, you need to build ROS 2 from source with specific CMake arguments (`-DFASTDDS_STATISTICS=ON`).

### 1. Uninstall Existing ROS 2 (if applicable)

If you have previously installed ROS 2 from binaries (e.g., `ros-humble`), you must uninstall it to avoid conflicts.

```bash
sudo apt remove ~nros-humble-* && sudo apt autoremove
```

Remove the apt source if needed:
```bash
sudo apt remove ros2-apt-source
sudo apt update
sudo apt autoremove
sudo apt upgrade # Optional: upgrade shadowed packages
```

### 2. System Setup

Ensure your locale supports UTF-8:

```bash
locale  # check for UTF-8

sudo apt update && sudo apt install locales
sudo locale-gen en_US en_US.UTF-8
sudo update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
export LANG=en_US.UTF-8

locale  # verify settings
```

Add the ROS 2 apt repository:

```bash
sudo apt install software-properties-common
sudo add-apt-repository universe
```

Configure ROS 2 repositories:

```bash
sudo apt update && sudo apt install curl -y
export ROS_APT_SOURCE_VERSION=$(curl -s https://api.github.com/repos/ros-infrastructure/ros-apt-source/releases/latest | grep -F "tag_name" | awk -F\" '{print $4}')
curl -L -o /tmp/ros2-apt-source.deb "https://github.com/ros-infrastructure/ros-apt-source/releases/download/${ROS_APT_SOURCE_VERSION}/ros2-apt-source_${ROS_APT_SOURCE_VERSION}.$(. /etc/os-release && echo ${UBUNTU_CODENAME:-${VERSION_CODENAME}})_all.deb"
sudo dpkg -i /tmp/ros2-apt-source.deb
```

Install development tools and dependencies:

```bash
sudo apt update && sudo apt install -y \
  python3-flake8-docstrings \
  python3-pip \
  python3-pytest-cov \
  ros-dev-tools
```

For Ubuntu 22.04 LTS and later:

```bash
sudo apt install -y \
   python3-flake8-blind-except \
   python3-flake8-builtins \
   python3-flake8-class-newline \
   python3-flake8-comprehensions \
   python3-flake8-deprecated \
   python3-flake8-import-order \
   python3-flake8-quotes \
   python3-pytest-repeat \
   python3-pytest-rerunfailures
```

## Installation

### 1. Build ROS 2 from Source

Create a workspace and clone the repositories:

```bash
mkdir -p ~/ros2_humble/src
cd ~/ros2_humble
vcs import --input https://raw.githubusercontent.com/ros2/ros2/humble/ros2.repos src
```

Install dependencies using `rosdep`:

```bash
sudo apt upgrade
sudo rosdep init
rosdep update
rosdep install --from-paths src --ignore-src -y --skip-keys "fastcdr rti-connext-dds-6.0.1 urdfdom_headers"
```

Build the workspace with Fast DDS Statistics enabled:

> **Note:** Ensure no other ROS 2 installation is sourced (`printenv | grep -i ROS` should be empty).

```bash
cd ~/ros2_humble/
colcon build --symlink-install --parallel-workers 4 --cmake-args -DFASTDDS_STATISTICS=ON
```

Source the environment:

```bash
. ~/ros2_humble/install/local_setup.bash
```

### 2. Install Fast DDS Monitor

Download the installer from the [eProsima website](https://www.eprosima.com/component/ars/releases/eprosima-fast-dds-monitor?Itemid=0).

Run the installer:

```bash
chmod +x eProsima_Fast-DDS-Monitor-<FastDDS-Version>-Linux.run
./eProsima_Fast-DDS-Monitor-<FastDDS-Version>-Linux.run
```

Follow the on-screen instructions.

## Usage

To enable statistics reporting, you must export the `FASTDDS_STATISTICS` environment variable before running your ROS 2 nodes.

```bash
export FASTDDS_STATISTICS="HISTORY_LATENCY_TOPIC;NETWORK_LATENCY_TOPIC;\
PUBLICATION_THROUGHPUT_TOPIC;SUBSCRIPTION_THROUGHPUT_TOPIC;RTPS_SENT_TOPIC;\
RTPS_LOST_TOPIC;HEARTBEAT_COUNT_TOPIC;ACKNACK_COUNT_TOPIC;NACKFRAG_COUNT_TOPIC;\
GAP_COUNT_TOPIC;DATA_COUNT_TOPIC;RESENT_DATAS_TOPIC;SAMPLE_DATAS_TOPIC;\
PDP_PACKETS_TOPIC;EDP_PACKETS_TOPIC;DISCOVERY_TOPIC;PHYSICAL_DATA_TOPIC;\
MONITOR_SERVICE_TOPIC"
```

## Resources

- [Uninstall ROS Humble (Debs)](https://docs.ros.org/en/humble/Installation/Ubuntu-Install-Debs.html#uninstall)
- [Build ROS Humble from Source](https://docs.ros.org/en/humble/Installation/Alternatives/Ubuntu-Development-Setup.html)
- [ROS Humble repo source](https://raw.githubusercontent.com/ros2/ros2/humble/ros2.repos)
- [Fast DDS Monitor Product Page](https://www.eprosima.com/middleware/tools/fast-dds-monitor)
- [Fast DDS Monitor Releases](https://www.eprosima.com/component/ars/releases/eprosima-fast-dds-monitor?Itemid=0)
- [Installation Manual (Linux)](https://fast-dds-monitor.readthedocs.io/en/latest/rst/installation/linux.html#installation-manual-linux)
- [Getting Started Tutorial](https://fast-dds-monitor.readthedocs.io/en/latest/rst/getting_started/tutorial.html)
- [Video Tutorial](https://www.youtube.com/watch?v=_hWfRUmmw1I)
