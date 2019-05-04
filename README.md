# Freecell68k
Freecell68k is an implementation of FreeCell for Classic Mac OS. It has been tested on a Mac SE running both System 6.0.8 and System 4.1, but should theoretically run on any version.

## Usage
When the program is opened, a game is automatically started. A new game can be started from File > New (Cmd-N), or can be opened with File > Open (Cmd-O) if the game number is known. Game numbers are the same as in Microsoft FreeCell. The current game can be restarted with File > Restart (Cmd-R). The program can be quit from File > Quit (Cmd-Q). If mistake is made during play, Edit > Undo (Cmd-Z) will undo the last move.

![FreeCell Image](https://github.com/StevenMcLeod/Freecell68k/blob/master/FreeCell.png)

## Installing & Building
Installing the application only requires downloading the FreeCell.bin file and unencoding it (MacBinary II).

To build the application from source, download the Freecell.sit.bin, which includes the source and resource files. A ThinkC (or compatible) compiler is required to build the application. The project file Freecell.proj is included in the archive (ThinkC 5 Format).

## Credits

(C) Steven McLeod, 2019. All rights reserved.
https://github.com/StevenMcLeod/Freecell68k
