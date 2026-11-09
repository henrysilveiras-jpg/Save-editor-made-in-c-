Supermarket Simulator — Native C++ Universal Save Editor

A lightweight, high-performance, native Windows desktop application written in C++ (Win32 API) to safely edit save files for Supermarket Simulator. It provides automatic platform detection for both Steam (.es3) and Xbox / PC Game Pass versions, allowing you to modify your Money, Store Level, and Store Points without corrupting the underlying save structure.

✨ Features

Universal Platform Support: Automatically detects whether the selected save file belongs to Steam or Xbox (Game Pass).

Smart Byte Compensation (Xbox): Safely adjusts binary container structures to account for length changes without breaking LEB128 sizing formats.

Direct Writing (Steam): Fast, clean, direct substitution for Steam .es3 plain-text JSON files.

Auto-Reading: Instantly reads and populates your current Money, Store Level, and Store Points upon opening a file.

Secure Automated Backups: Automatically generates a timestamped backup (.backup) in your Documents\Supermarket Simulator Backups folder before applying any changes.

Zero False Positives: Built using pure native Windows APIs (user32, commdlg), resulting in a pristine antivirus reputation.

📂 Where to Find Your Save Files

Depending on the version of the game you are playing, your save files are located in the following directories:

1. Steam Version

Press Win + R, type %appdata% and press Enter.

Navigate backwards from Roaming to LocalLow (or go directly to: C:\Users\<YourUsername>\AppData\LocalLow\NK Games\Supermarket Simulator).

Look for save files ending with the .es3 extension.

2. Xbox / PC Game Pass Version (Microsoft Store)

Press Win + R, type %localappdata% and press Enter.

Open the Packages folder and search for the game's package folder (usually starts with NKGames.SupermarketSimulator...).

Inside, navigate to LocalCache\Local\ to find the serialized container save files.

⚙️ How to Compile from Source (Visual Studio)

If you wish to build the standalone executable (.exe) yourself from the source code, follow this step-by-step guide:

Prerequisites

Microsoft Visual Studio installed on Windows (ensure the "Desktop development with C++" workload is selected during installation).

Step-by-Step Compilation Guide

Get the Source Code:

Download or clone this repository, ensuring you have the SaveEditor.cpp file ready.

Create a New Project:

Open Visual Studio.

Click on Create a new project.

Search for and select C++ Windows Desktop Application (or create an Empty Project). Name your project (e.g., SaveEditor) and click Create.

Add the Code File:

Look at the Solution Explorer panel on the right-hand side.

Right-click on Source Files -> Add -> Existing Item...

Select the SaveEditor.cpp file from your repository and click Add.

Configure Build Settings (Release & x64):

At the top toolbar of Visual Studio, locate the solution configuration dropdowns.

Change Debug to Release (this optimizes the code and removes debugging bloat).

Change x86 to x64 (standard architecture for modern 64-bit Windows PCs).

Build the Solution:

Go to the top menu bar and click Build -> Build Solution (or use the shortcut Ctrl + Shift + B).

Check the Output window at the bottom to ensure the build succeeded with 0 errors.

Locate Your Executable:

Navigate to your project directory on your computer.

Go to x64\Release\ and you will find your standalone, ready-to-use SaveEditor.exe.

🎮 How to Use

Launch SaveEditor.exe.

Click the "Open Save" button and browse to select your game save file.

The application will analyze the file and automatically display the detected platform version (STEAM or XBOX / GAME PASS).

Review your current stats displayed in the interface and type your desired values into the entry fields.

Click "Save Edited File".

The application will instantly secure a timestamped backup in your Documents\Supermarket Simulator Backups folder and safely overwrite your original save with your new values.
