# IMGV - Image Viewer

IMGV is a simple image viewer written in C using [raylib](https://github.com/raysan5/raylib).

> [!WARNING]
> - This repository doesn't include [raylib](https://github.com/raysan5/raylib), so you will need to download/install and build it based on your system.
> - IMGV only supports `png` images at the moment.

IMGV Primarily focuses on keyboard based movement (specifically VIM movements) to pan around image.

### Usage:
At the moment IMGV can only be used through command line.
```Bash
$ make
$ ./imgv ~/path/to/image.png
```
