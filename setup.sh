#!/bin/bash

echo $'Setting up your environment for the Durak game...\n'

cd "$Networking"

echo $'\nCloning git repos: sockpp & rapidjson\n'

# Navigate to the Networking directory
cd "$Networking" || { echo "Failed to navigate to Networking"; exit 1; }

# Clone the sockpp repository if not already cloned
if [ ! -d "sockpp" ]; then
    git clone https://github.com/fpagliughi/sockpp || { echo "Failed to clone sockpp"; exit 1; }
else
    echo "sockpp repo already exists. Skipping cloning. Updating..."
    cd "$sockpp" || { echo "Failed to navigate to sockpp"; exit 1;}
    git pull || { echo "Failed to pull from git sockpp"; exit 1;}
    cd .. #back to networking
fi

# Clone the rapidjson repository if not already cloned
if [ ! -d "rapidjson" ]; then
    git clone https://github.com/Tencent/rapidjson || { echo "Failed to clone rapidjson"; exit 1; }
else
    echo "rapidjson repo already exists. Skipping cloning. Updating..."
    cd "$rapidjson" || { echo "Failed to navigate to rapidjson"; exit 1;}
    git pull || { echo "Failed to pull from git rapidjson"; exit 1;}
    cd .. #back to networking
fi

cd .. #back to durak main parent dir

echo $'\nCloning git repo: glfw...\n' 

cd "$Client" || { echo "Failed to navigate to Client"; exit 1; }
cd "$libs" || { echo "Failed to navigate to libs"; exit 1; }

# Clone the GLFW repository with submodules if not already cloned
if [ ! -d "glfw" ]; then
    git clone --recurse-submodules https://github.com/glfw/glfw.git || { echo "Failed to clone glfw"; exit 1; }
else
    echo "glfw repo already exists. Skipping cloning."
fi

cd .. #back to Client dir
cd .. #back to durak dir

mkdir -p build
cd "$build"

cmake ..
make clean
make

echo $'\nSetting up your environment complete\n'

echo $'\nALWAYS RUN EVERYTHING IN THE "build" DIRECTORY\n'

echo $'\nTO RUN THE SERVER: \n\n ./Server/DurakServer\n\n'

echo $'\nTO RUN THE CLIENT: \n\n ./Client/DurakClient\n\n'

echo $'\nIF SERVER HOSTET LOCALLY, JUST PRESS CONNECT\n'

echo $'\nTO FIND OUT THE IP: ask chat gpt lol\n'