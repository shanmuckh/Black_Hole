# Contributing to Black_Hole

:+1::tada: First off, thank you for taking the time to contribute! :tada::+1:

The goal of this project is to build a robust and efficient application, and your help makes it better. By participating in this project, you agree to abide by our [Code of Conduct](CODE_OF_CONDUCT.md).

---

## How Can I Contribute?

### Reporting Bugs
If you find a bug, please search the [Issues](https://github.com/shanmuckh/Black_Hole/issues) to see if it has already been reported. If it hasn't, open a new issue and include:
* **A clear title and description.**
* **Steps to reproduce the bug.**
* **Expected vs. Actual behavior.**
* **Screenshots** if applicable.

### Suggesting Enhancements
Feature requests are welcome! Please open an issue and describe the feature you would like to see, why it’s useful, and how it might work.

---

## Pull Request Process

To ensure a smooth review process, please follow these steps:



1.  **Fork the Repo:** Create your own copy of the project.
2.  **Create a Branch:** Use a descriptive name (e.g., `fix-auth-bug` or `feat-add-sidebar`).
3.  **Make Your Changes:** * Ensure your code follows the project's style.
    * Include comments for complex logic.
4.  **Commit with Clarity:** Follow standard commit message conventions (e.g., `feat: add firebase login support`).
5.  **Push and Open a PR:** Submit your pull request to the `main` branch.
6.  **Review:** Wait for feedback. Be prepared to make requested changes if necessary.

---

## Development Setup

# **1. Dependencies (vcpkg)**
- git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
- cd C:\vcpkg && .\bootstrap-vcpkg.bat && .\vcpkg install glfw3 glew glm --triplet x64-windows

### Local Installation
```bash
# Clone the repository
git clone https://github.com/shanmuckh/Black_Hole.git

# Navigate into the directory
cd Black_Hole

mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
