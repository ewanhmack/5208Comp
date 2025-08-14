# 5208Comp — Quick Start

This guide explains how to clone, open, build, and run the 5208Comp project on Windows using Visual Studio.

## Requirements

- **Windows 10/11**
- **Visual Studio 2022** (Community edition works)
  - Install the **Desktop development with C++** workload
  - Include **MSVC v143** and **Windows 10/11 SDK** components
- **Git** for Windows

## Get the code

```powershell
cd "E:\My Files\Github"
git clone https://github.com/ewanhmack/5208Comp.git
cd 5208Comp
```

## Open & run in Visual Studio

1. Double‑click `` to open the solution in Visual Studio.
2. In the **Solution Explorer**, right‑click the `` project and choose **Set as Startup Project**.
3. Select **Debug** (development) or **Release** (faster) from the dropdown menu.
4. Build the project: **Build → Build Solution** (`Ctrl+Shift+B`).
5. Run the project:
   - **Start Debugging**: `F5`
   - **Start Without Debugging**: `Ctrl+F5`

## If assets are missing

If the application cannot find textures, audio, or other assets:

- Set the **Working Directory** to the project root or the `Assets` folder:\
  *Project → Properties → Debugging → Working Directory*

## Troubleshooting

- **Windows SDK / toolset not found** → Open *Visual Studio Installer*, click **Modify**, and add Windows 10/11 SDK and MSVC v143.
- **Black or empty window** → Check the working directory and ensure all required assets are present.
- **Linker errors (LNK2019, etc.)** → Ensure required libraries are referenced in the project settings.

## Git hygiene

Add these lines to `.gitignore` to prevent committing cache files:

```gitignore
*.ipch
.ipch/
.vs/
*.VC.db
*.VC.opendb
```

To untrack already staged cache files:

```powershell
git rm -r --cached --ignore-unmatch *.ipch .ipch .vs *.VC.db *.VC.opendb
```

