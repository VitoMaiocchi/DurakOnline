#!/bin/bash

Networking="$(pwd)/Networking"  # Or provide the absolute path

echo "Setting up your environment for the Durak game..."

# Create the Networking directory if it doesn't exist
if [ ! -d "$Networking" ]; then
    echo "Networking directory not found. Creating it..."
    mkdir -p "$Networking"
fi

# Navigate to the Networking directory
cd "$Networking" || { echo "Failed to navigate to $Networking"; exit 1; }

# Clone the sockpp repository if not already cloned
if [ ! -d "sockpp" ]; then
    echo "Getting the sockpp repo..."
    git clone https://github.com/fpagliughi/sockpp || { echo "Failed to clone sockpp"; exit 1; }
else
    echo "sockpp repo already exists. Skipping cloning."
fi

# Clone the rapidjson repository if not already cloned
if [ ! -d "rapidjson" ]; then
    echo "Getting the rapidjson repo..."
    git clone https://github.com/Tencent/rapidjson || { echo "Failed to clone rapidjson"; exit 1; }
else
    echo "rapidjson repo already exists. Skipping cloning."
fi

# Navigate to Client/libs directory
ClientLibs="$(pwd)/../Client/libs"
if [ ! -d "$ClientLibs" ]; then
    echo "Client/libs directory not found. Creating it..."
    mkdir -p "$ClientLibs"
fi

cd "$ClientLibs" || { echo "Failed to navigate to $ClientLibs"; exit 1; }

# Clone the GLFW repository with submodules if not already cloned
if [ ! -d "glfw" ]; then
    echo "Getting the glfw repo..."
    git clone --recurse-submodules https://github.com/glfw/glfw.git || { echo "Failed to clone glfw"; exit 1; }
else
    echo "glfw repo already exists. Skipping cloning."
    # Ensure submodules are initialized
    cd glfw
    git submodule update --init --recursive || { echo "Failed to update glfw submodules"; exit 1; }
    cd ..
fi

echo "Environment setup complete."
