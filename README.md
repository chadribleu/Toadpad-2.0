```cpp
void ShowTitle() {
  std::cout << "Welcome to Toadpad!\n";
}
```
## Do you remember the good-old Windows Notepad ?
This project aims to recreate the original Windows Notepad, but with some enhancements such as tabs or recent documents list.
Toadpad was programmed entirely in C++, with the OOP design and the "raw" win32 API to ensure consistency with Windows's native UI. If you have any questions or suggestions, feel free to ask!

<p align="center">
<img src="https://user-images.githubusercontent.com/46863870/177409183-501e596b-92d4-4cd4-9f0d-9ae81776c334.png"/>
</p>

## Installation
Currently, no executable files are provided for an apparent reason.
However, if you want to test the app as-is, it is all good! :)

0. Install Visual Studio Community 2022 with C++ components
1. Clone the whole repository
2. Launch the ``.sln``
3. Select ``Debug x64`` for the build output
4. Build the solution
5. Go to your output directory (You can check it via ``Project | Properties | General | "Output Directory"``)
6. Launch ``Toadpad.exe``

## What can I do with the current build ?
Here are the current features that are implemented within this app:
- Basic raw text functions
- Save, create and load files (only UTF8 encoding is supported for the moment)
- Determine if the file is dirty or not
- Enable or disable word-wraping
- Tabs
- Resizeable Window with "adaptive UI"
- Shortcuts (You cannot add custom ones yet)
- Custom font (well, you currently can't save it :trollface:)
- Line and column counter
- Status bar
- App icon

## What's next?
- Add a ``'\*'`` character when the document was modified
- Zoom
- Home when no tabs are opened
- Recent documents
- "About" menu (I'm so lazyyyyyyyy)
- Dark theme
- Save settings

# Chadribleu.Dialogs
## What is that?
``Chadribleu.Dialogs`` is a tiny library to encapsulate dialogs like ```FilePicker```, ```ChooseFont```, ```PickColor``` and ```CustomDialog```. The library is still using a deprecated way to create the dialogs (``OPENFILENAME`` for example). Additionally, if you're working on an app that is using ANSI functions, my library is not for you, because it's using ``wchar_t`` to store characters, but the "macro version" of all functions though.
