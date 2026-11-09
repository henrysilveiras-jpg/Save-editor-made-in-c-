<div align="center">

# 🛒 Supermarket Simulator — Universal Save Editor

[![Platform](https://img.shields.io/badge/Platform-Windows-blue?style=flat-square&logo=windows)](https://microsoft.com)
[![Language](https://img.shields.io/badge/Language-C++%20(Win32%20API)-orange?style=flat-square&logo=cplusplus)](https://isocpp.org)
[![Game](https://img.shields.io/badge/Game-Supermarket%20Simulator-green?style=flat-square&logo=steam)](https://store.steampowered.com/app/2670630/Supermarket_Simulator/)
[![License](https://img.shields.io/badge/License-MIT-purple?style=flat-square)](LICENSE)

*A lightweight, native, and high-performance desktop application built in pure C++ to safely edit save files for **Supermarket Simulator** across Steam and Xbox (Game Pass) versions without triggering false antivirus positives.*

</div>

---

## 📖 Table of Contents
* [✨ Features](#-features)
* [📂 Where to Find Your Save Files](#-where-to-find-your-save-files)
  * [Steam Version](#1-steam-version)
  * [Xbox / PC Game Pass Version](#2-xbox--pc-game-pass-version)
* [🚀 How to Use](#-how-to-use)
* [⚙️ How to Compile from Source (Visual Studio)](#️-how-to-compile-from-source-visual-studio)
* [🛡️ Security & Antivirus Reputation](#️-security--antivirus-reputation)
* [🤝 Contributing & Support](#-contributing--support)

---

## ✨ Features

* **Universal Platform Detection:** Automatically identifies whether your save file belongs to **Steam (`.es3` plain text)** or **Xbox / PC Game Pass (binary container)**.
* **Smart Byte Compensation (Xbox):** Performs surgical binary container adjustments to account for length changes without breaking .NET/Unity LEB128 sizing formats.
* **Direct Writing (Steam):** Fast, clean, direct JSON string replacement for Steam saves.
* **Auto-Reading Statistics:** Instantly parses and displays your current **Money**, **Store Level**, and **Store Points** upon opening a file.
* **Automated Secure Backups:** Automatically generates a timestamped backup (`.backup`) in your `Documents\Supermarket Simulator Backups` folder before applying any changes, ensuring you never lose your progress.
* **Zero False Positives:** Developed using native Windows Win32 API controls (`user32`, `commdlg`), resulting in a clean reputation across antivirus scanners.

---

## 📂 Where to Find Your Save Files

Depending on where you purchased or play the game, your save location differs:

### 1. Steam Version
1. Press `Win + R`, type `%appdata%` and hit **Enter**.
2. Go back one folder from `Roaming` into **`LocalLow`** (or paste this path directly into Explorer: `C:\Users\<YourUsername>\AppData\LocalLow\NK Games\Supermarket Simulator`).
3. Locate your save files ending with the **`.es3`** extension.

### 2. Xbox / PC Game Pass Version (Microsoft Store)
1. Press `Win + R`, type `%localappdata%` and hit **Enter**.
2. Open the **`Packages`** folder and search for the folder starting with `NKGames.SupermarketSimulator...`.
3. Inside, navigate to `LocalCache\Local\` to access your binary container save files.

---

## 🚀 How to Use

1. Launch **`SaveEditor.exe`**.
2. Click the **"Open Save"** button and select your game save file.
3. Check the version label to verify that the app correctly identified your platform (**STEAM** or **XBOX / GAME PASS**).
4. Review your current stats and type your desired values into the entry fields:
   * **Money:** Supports decimals (e.g., `50000.50`).
   * **Store Level & Store Points:** Integers only (e.g., `50`).
5. Click **"Save Edited File"**.
6. A timestamped backup will instantly be secured in your `Documents\Supermarket Simulator Backups` folder, and your original file will be updated successfully!

---

## ⚙️ How to Compile from Source (Visual Studio)

If you want to build the standalone `.exe` yourself, follow these detailed steps:

### Prerequisites
* **Microsoft Visual Studio** (Community, Professional, or Enterprise) installed on Windows.
* Ensure the workload **"Desktop development with C++"** is checked and installed via the Visual Studio Installer.

### Step-by-Step Build Guide

1. **Clone or Download:**
   * Download the repository source files to your computer, ensuring you have **`SaveEditor.cpp`**.

2. **Create a New Project:**
   * Open **Visual Studio** and click on **Create a new project**.
   * Search for **C++ Windows Desktop Application** (or choose *Empty Project*). 
   * Name your project (e.g., `SupermarketSaveEditor`) and click **Create**.

3. **Add the Source File:**
   * Look at the **Solution Explorer** panel on the right side of the IDE.
   * Right-click on **Source Files** -> **Add** -> **Existing Item...**
   * Select the **`SaveEditor.cpp`** file and click **Add**.

4. **Configure Build Optimization (Release & x64):**
   * At the top toolbar, locate the configuration dropdowns (next to the green "Play" debug button).
   * Change **Debug** to **`Release`** (this strips unnecessary debugging symbols and optimizes performance).
   * Change **x86** to **`x64`** (standard 64-bit architecture for modern Windows PCs).

5. **Build the Solution:**
   * Go to the top menu bar and select **Build** -> **Build Solution** (or press `Ctrl + Shift + B`).
   * Check the output console at the bottom to verify that the build completed with **0 errors**.

6. **Find Your Executable:**
   * Open your project folder on disk.
   * Navigate to `x64\Release\`, and you will find your compiled, ready-to-distribute **`SaveEditor.exe`**.

---

## 🛡️ Security & Antivirus Reputation

Because this tool is written in **native C++ using standard Windows Win32 APIs** without heavy third-party GUI frameworks or interpreted runtimes, it compiles down to clean, transparent machine code. This guarantees a pristine antivirus reputation with **zero false-positive flags** on Windows Defender and VirusTotal.

---

## 🤝 Contributing & Support

Contributions, bug reports, and feature requests are welcome! Feel free to open an issue or submit a pull request. 

*If this tool saved your game progress, consider giving the repository a ⭐ star!*
