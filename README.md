# 🖥️ System Monitor Tool (Assignment 3 - LSP)

## 📌 Objective
Create a **System Monitor Tool** in **C++** that displays real-time information about system processes, memory usage, and CPU load — similar to the **`top`** command in Linux.

---

## 🚀 Features
- Display system processes with CPU and memory usage.
- Sort processes dynamically by CPU or memory usage.
- Kill/terminate processes directly from the UI.
- Real-time data updates every few seconds.
- Clean and user-friendly interface using **ImGui + SFML**.

---

## 🧩 Technologies Used
- **Language:** C++17  
- **Libraries:**  
  - [ImGui](https://github.com/ocornut/imgui) – Immediate Mode GUI  
  - [ImGui-SFML](https://github.com/eliasdaler/imgui-sfml) – SFML integration  
  - [SFML](https://www.sfml-dev.org/) – Rendering and window management  
- **System APIs:** Linux `/proc` filesystem for system and process data  

---

## 🛠️ Build Instructions

### 🔹 Prerequisites
Make sure you have the following installed:
- `g++` (C++17 or later)
- `make`
- `SFML` and `ImGui-SFML` libraries

On Ubuntu:
```bash
sudo apt update
sudo apt install g++ make libsfml-dev

