#!/usr/bin/env bash
# Installs zephyr on your fedora PC
# Creates directories `~/zephyrproject` and `~/zephyr-sdk`
#
# Based on the Zephyr getting started guide
# https://docs.zephyrproject.org/latest/develop/getting_started/

# Install dependencies
if grep -q Fedora /etc/os-release; then
    echo "Installing dependencies for Fedora"
    sudo dnf update
    sudo dnf install git cmake ninja-build gperf ccache dfu-util dtc wget python3-devel python3-pip python3-setuptools python3-tkinter python3-wheel xz file make gcc glibc-devel glibc-static gcc-c++ libstdc++-devel libstdc++-static SDL2-devel file-devel python3-virtualenv
else
    echo "Looks like this PC doesn't run on Fedora! If you are running Ubuntu, use the zephyr-install-ubunto.sh script".
    echo "If you are on any other Linux distribution -\n Please make sure the following is installed: git cmake ninja-build gperf   ccache dfu-util device-tree-compiler wget   python3-dev python3-pip python3-setuptools python3-tk python3-wheel python3-venv xz-utils file   make gcc gcc-multilib g++-multilib libsdl2-dev libmagic1"
    exit 1
fi

# Get Zephyr and download python dependencies
mkdir -p ~/zephyrproject &&
git clone --depth=1 https://github.com/zephyrproject-rtos/zephyr ~/zephyrproject &&
python3 -m venv ~/zephyrproject/.venv &&
source ~/zephyrproject/.venv/bin/activate &&
pip install west &&
west init ~/zephyrproject/ &&
cd ~/zephyrproject/ &&
west update &&
west zephyr-export &&
pip install -r ~/zephyrproject/zephyr/scripts/requirements.txt &&

# Get Zephyr SDK
cd ~ &&
wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.16.1/zephyr-sdk-0.16.1_linux-x86_64.tar.xz &&
tar xvf zephyr-sdk-0.16.1_linux-x86_64.tar.xz &&
cd zephyr-sdk-0.16.1 &&
./setup.sh &&
sudo cp ~/zephyr-sdk-0.16.1/sysroots/x86_64-pokysdk-linux/usr/share/openocd/contrib/60-openocd.rules /etc/udev/rules.d &&
sudo udevadm control --reload &&

# Build example sketch to see if everything works
cd ~/zephyrproject/zephyr &&
west build -p always -b stm32f4_disco samples/basic/blinky
