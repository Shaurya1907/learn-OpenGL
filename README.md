# My C++ OpenGL 3D Engine 🚀

# 

# A custom real-time 3D rendering engine built from scratch in C++ using modern OpenGL.

# Fly around a small 3D world with dynamic lighting, real-time shadows, a day–night cycle, skybox, and fully textured models.

# 

# ✨ Features

# Modern OpenGL (core profile, VAOs, VBOs, EBOs, depth testing)

# 

# Phong lighting model (ambient, diffuse, specular)

# 

# Directional, point, and spot lights

# 

# Shadow mapping:

# 

# Directional light shadow map

# 

# Omni shadow maps for point/spot lights

# 

# Animated day–night cycle (moving “sun” directional light)

# 

# Skybox rendering with cubemap textures

# 

# Model loading (OBJ + MTL + textures)

# 

# Textured floor and basic meshes

# 

# Material system (specular intensity, shininess control)

# 

# FPS-style camera system (movement + zoom)

# 

# Keyboard \& mouse input handling

# 

# 🎮 Controls

# WASD – Move camera

# 

# Space / Left Shift – Move up / down

# 

# Mouse move – Look around

# 

# Mouse scroll – Zoom / change FOV

# 

# Tab – Switch camera

# 

# L – Toggle spotlight (flashlight)

# 

# T – Toggle wireframe mode

# 

# V – Toggle light-view debug viewport

# 

# Esc – Quit

# 

# 🚧 Getting Started

# Clone the repo:

# git clone https://github.com/Shaurya1907/learn-OpenGL

# 

# Open the project in Visual Studio (or your favorite C++ IDE).

# 

# Make sure these dependencies are available:

# 

# GLFW

# 

# GLAD

# 

# stb\_image

# 

# GLM

# 

# Build \& run:

# 

# Set the executable target as the startup project

# 

# Build in Debug or Release

# 

# Hit Run and explore the scene!

# 

# 🧱 Project Structure (high level)

# src/

# 

# main.cpp – application entry + main loop

# 

# Window.\* – window and OpenGL context

# 

# Shader.\* – shader compilation + uniforms

# 

# Mesh.\* – VAO/VBO/EBO abstraction

# 

# Texture.\* – texture loading (stb\_image)

# 

# DirectionalLight.\* / PointLight.\* / SpotLight.\* – lights + shadow maps

# 

# Material.\* – material parameters

# 

# Model.\* – OBJ model loading \& rendering

# 

# Skybox.\* – cubemap skybox system

# 

# io/keyboard., io/mouse., io/camera.\* – input \& camera

# 

# Shaders/ – all GLSL shaders (lighting, shadows, skybox)

# 

# Textures/ – brick, dirt, plain, skybox cubemap faces

# 

# Models/ – Seahawk, Airplane, old water tower, and other test models

# 

# 📜 License

# This project is licensed under the MIT License – see the LICENSE file for details.

