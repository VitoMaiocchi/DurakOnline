#!/bin/bash

echo $'Setting up your environment for the Durak game...\n'

# Get the script's directory (resolves symlinks and ensures it's absolute)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

NETWORKING_DIR="$SCRIPT_DIR/Networking"
# Navigate to the Networking directory
cd "$NETWORKING_DIR" || { echo "Failed to navigate to Networking"; exit 1; }

echo $'\nCloning git repos: sockpp & rapidjson\n'

SOCKPP_DIR="$SCRIPT_DIR/Networking/sockpp"
# Clone the sockpp repository if not already cloned
if [ -d "$SOCKPP_DIR" ]; then
    if [ -z "$(ls -A "$SOCKPP_DIR")" ]; then #if empty clone
        git clone https://github.com/fpagliughi/sockpp || { echo "Failed to clone sockpp"; exit 1; }
    else #just pull
        cd "$SOCKPP_DIR" || { echo "Failed to navigate to sockpp"; exit 1;}

        git pull || { echo "Failed to pull from git sockpp"; exit 1;}
        cd .. #back to networking
    fi
else #dir doesnt exits
    git clone https://github.com/fpagliughi/sockpp "$SOCKPP_DIR" || { echo "Failed to clone sockpp"; exit 1; }
fi

# Clone the rapidjson repository if not already cloned
RAPIDJSON_DIR="$SCRIPT_DIR/Networking/rapidjson"
# Clone the sockpp repository if not already cloned
if [ -d "$RAPIDJSON_DIR" ]; then
    if [ -z "$(ls -A "$RAPIDJSON_DIR")" ]; then #if empty clone
        git clone https://github.com/Tencent/rapidjson || { echo "Failed to clone rapidjson"; exit 1; }
    else #just pull
        cd "$RAPIDJSON_DIR" || { echo "Failed to navigate to rapidjson"; exit 1;}

        git pull || { echo "Failed to pull from git rapidjson"; exit 1;}
        cd .. #back to networking
    fi
else #dir doesnt exits
    git clone https://github.com/Tencent/rapidjson "$RAPIDJSON_DIR" || { echo "Failed to clone rapidjson"; exit 1; }
fi

cd .. #back to durak main parent dir

echo $'\nCloning git repo: glfw...\n' 

CLIENTLIBS_DIR="$SCRIPT_DIR/Client/libs"
cd "$CLIENTLIBS_DIR"
GLFW_DIR="$CLIENTLIBS_DIR/glfw"

if [ -d "$GLFW_DIR" ]; then
    if [ -z "$(ls -A "$GLFW_DIR")" ]; then #if empty clone
        git clone https://github.com/glfw/glfw.git || { echo "Failed to clone glfw"; exit 1; }
    else #just pull
        cd "$GLFW_DIR" || { echo "Failed to navigate to glfw"; exit 1; }
        git pull || { echo "Failed to pull from git glfw"; exit 1;}
        cd .. #back in libs
    fi
else #dir doesnt exist
    git clone https://github.com/glfw/glfw.git || { echo "Failed to clone glfw"; exit 1; }
fi

cd .. # back to client
cd .. #back to durak main dir


echo $'\ninstalling dependencies\n'
sudo apt update
sudo apt install doxygen

sudo apt install graphviz

sudo apt install cmake
sudo apt install make
sudo apt install g++
echo $'----------------------------------------------------'
echo $'\ncheck compiler version\n'
gcc --version
g++ --version
echo $'----------------------------------------------------'

sudo apt install wayland-protocols wayland-devel
sudo apt install wayland-protocols libwayland-dev

echo $'----------------------------------------------------'
echo $'\ncheck wayland scanner version\n'
wayland-scanner --version
echo $'----------------------------------------------------'

sudo apt install pkg-config
sudo apt install libx11-dev

echo $'----------------------------------------------------'
echo $'\ncheck pkg-config version\n'
pkg-config --version
echo $'----------------------------------------------------'

sudo apt install libxkbcommon-dev
echo $'----------------------------------------------------'
echo $'\ncheck that xkbcommon is installed correctly\n'
pkg-config --cflags --libs xkbcommon
echo $'----------------------------------------------------'




echo $'\nCreating the Durak documentation with Doxygen\n'
doxygen Doxyfile


mkdir -p build
cd "$SCRIPT_DIR/build"
cmake ..
make clean
make -j4

echo $'\nSetting up your environment complete...\n'

echo $'\n!!!ALWAYS RUN EVERYTHING IN THE "build" DIRECTORY!!!\n'
echo $'----------------------------------------------------'
echo $'\nTO RUN THE SERVER: \n\n ./Server/DurakServer\n'
echo $'----------------------------------------------------'
echo $'\nTO RUN THE CLIENT: \n\n ./Client/DurakClient\n'
echo $'----------------------------------------------------'

echo $'\nIF SERVER HOSTET LOCALLY, JUST PRESS CONNECT\n'

echo $'THE DEFAULT IP IS LOCALHOST\n' 

echo $'----------------------------------------------------'
echo $'\nTO OPEN THE DOCUMENTATION GO TO "docs/html"\n'
echo $'open "index.html" with your preferred browser\n'