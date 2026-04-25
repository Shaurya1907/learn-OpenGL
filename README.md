# 🚀 Learn-OpenGL — Custom C++ 3D Rendering Engine

A **custom real-time 3D rendering engine** built from scratch in **C++ using Modern OpenGL**.

Explore a small interactive 3D world featuring **dynamic lighting**, **real-time shadows**, a **day–night cycle**, **skybox rendering**, and **fully textured 3D models**.

---

# 🖼️ Screenshots

## 🌅 Day–Night Cycle

<p align="center">
  <img src="OpenGL/assets/Day.png" width="45%" />
  <img src="OpenGL/assets/Night.png" width="45%" />
</p>

---

## 💡 Lighting & Shadows

<p align="center">
  <img src="OpenGL/assets/Lighting.png" width="45%" />
  <img src="OpenGL/assets/Shadow.png" width="45%" />
</p>

---

## 🔍 Shadow Debug View

<p align="center">
  <img src="OpenGL/assets/Light_View.png" width="60%" />
</p>

---

## 🌌 Skybox Rendering

<p align="center">
  <img src="OpenGL/assets/GreenSkybox.png" width="45%" />
  <img src="OpenGL/assets/BlueSkybox.png" width="45%" />
</p>

---

# ✨ Features

## 🎮 Rendering

* Modern **OpenGL Core Profile**
* VAO, VBO, EBO abstraction
* Depth testing
* Wireframe debug mode

## 💡 Lighting System

* **Phong Lighting Model**

  * Ambient
  * Diffuse
  * Specular
* Light types:

  * Directional
  * Point
  * Spot (flashlight)

## 🌑 Shadow Mapping

* Directional shadow mapping
* Omni-directional shadows
* Real-time shadow rendering
* Debug light-space visualization

## 🌅 Environment

* Animated **Day–Night Cycle**
* Moving sun (directional light)
* Skybox using cubemaps

## 🎥 Camera System

* FPS-style movement
* Mouse look
* Zoom (FOV control)
* Camera switching

## 🧱 Models & Materials

* OBJ + MTL loading
* Textured meshes
* Material system (shininess, specular control)

## 🎮 Input System

* Keyboard and mouse handling
* Debug toggles
* Camera switching support

---

# 🎮 Controls

| Key               | Action                  |
| ----------------- | ----------------------- |
| **W A S D**       | Move camera             |
| **Space / Shift** | Move up / down          |
| **Mouse Move**    | Look around             |
| **Mouse Scroll**  | Zoom                    |
| **Tab**           | Switch camera           |
| **L**             | Toggle flashlight       |
| **T**             | Toggle wireframe mode   |
| **V**             | Toggle light debug view |
| **Esc**           | Quit                    |

---

# 🧰 Dependencies

This project uses the following libraries:

* **GLFW** — Window creation and input handling
* **GLAD** — OpenGL function loader
* **GLM** — Mathematics library
* **stb_image** — Texture loading

---

# 🖥️ Build Requirements

* Windows 10 / 11
* Visual Studio 2022
* OpenGL 3.3 or higher
* x64 architecture

---

# 🚧 Getting Started

## Clone Repository

```bash
git clone https://github.com/Shaurya1907/learn-OpenGL
cd learn-OpenGL
```

## Build & Run

1. Open **OpenGL.sln** in **Visual Studio 2022**
2. Set the project as **Startup Project**
3. Build using **Debug** or **Release**
4. Click **Run** to explore the scene

---

# 🧠 Engine Architecture Overview

This engine follows a modular architecture design:

* **Rendering System** — Handles OpenGL pipeline setup and rendering
* **Lighting System** — Manages directional, point, and spot lights
* **Shadow System** — Implements shadow mapping techniques
* **Material System** — Controls surface properties and lighting response
* **Model Loader** — Imports external OBJ models with textures
* **Camera System** — Provides FPS-style movement and view control

---

# 🧱 Project Structure

```
learn-OpenGL/

├── OpenGL/                         # Main Visual Studio project
│
│   ├── assets/                    # Textures, skyboxes, screenshots
│   │   ├── Day.png
│   │   ├── Night.png
│   │   ├── Lighting.png
│   │   ├── Shadow.png
│   │   ├── Light_View.png
│   │   ├── GreenSkybox.png
│   │   └── BlueSkybox.png
│
│   ├── Models/                    # 3D models (OBJ files)
│
│   ├── Shaders/                   # GLSL shader programs
│
│   ├── Source/                    # Core engine source code
│   │
│   │   ├── main.cpp               # Application entry point
│   │   ├── Window.*               # Window and OpenGL context
│   │   ├── Shader.*               # Shader compilation system
│   │   ├── Mesh.*                 # VAO / VBO / EBO handling
│   │   ├── Texture.*              # Texture loading
│   │   ├── Material.*             # Material properties
│   │   ├── Model.*                # Model loading system
│   │   ├── Skybox.*               # Cubemap rendering
│   │
│   │   ├── DirectionalLight.*     # Directional lighting
│   │   ├── PointLight.*           # Point lighting
│   │   ├── SpotLight.*            # Spotlight system
│   │
│   │   └── io/                    # Input and camera handling
│   │        ├── keyboard.*
│   │        ├── mouse.*
│   │        └── camera.*
│
├── dependencies/                  # External libraries
│   ├── GLFW/
│   ├── GLAD/
│   ├── GLM/
│   └── stb_image/
│
├── glsl OpenGL/                   # Additional GLSL resources
│
├── OpenGL.sln                     # Visual Studio solution
├── Licence.txt                    # MIT License
├── README.md                      # Project documentation
├── .gitignore
└── .gitattributes
```

---

# 🔍 Technical Highlights

* Real-time **shadow mapping**
* Multiple light types working simultaneously
* **Skybox rendering** using cubemaps
* Modular **lighting architecture**
* Efficient **camera and input system**
* Real-time **day–night cycle simulation**

---

# 📜 License

This project is licensed under the **MIT License** — see the
[Licence.txt](Licence.txt) file for details.

