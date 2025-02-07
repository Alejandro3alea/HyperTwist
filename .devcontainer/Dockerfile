FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    # OpenGL + GLEW
    libglew-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    # SFML (Simple and Fast Multimedia Library)
    libsfml-dev \
    # Freetype
    libfreetype6-dev \
    # GLM
    libglm-dev \
    # SDL2
    libsdl2-dev \
    # JSON
    nlohmann-json3-dev \
    # X11 (for SFML/OpenGL)
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    # Free package lists from the directory, as we no longer need them
    && rm -rf /var/lib/apt/lists/*


RUN git clone https://github.com/ocornut/imgui.git /usr/local/src/imgui

RUN git clone https://github.com/syoyo/tinygltf.git /usr/local/src/tinygltf 

RUN git clone https://github.com/nothings/stb.git /usr/local/stc/stb