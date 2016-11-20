SpRead
==========

SpRead is a image viewer for ebook that is composed non-DRM PDF or standard image format.
It can read archive files that are included images.
It is written in C++ using FLTK. So, SpRead can run on Windows, OS X and *NIX.

## Features
* Supports ZIP, TAR, 7Z, CAB, RAR, LHA and LZH, and some compression format
* Supports most major common popular image format
* Supports PDF format
* Reads directories which are included images
* Spread view mode (double page mode)
 * Both left-to-right and right-to-left reading modes
 * Auto control of spread view mode
* Prefetch of images

## Requirement
* C++ Compiler which supports C++11 or above
* libarchive >= 3.2.0
* FLTK >= 1.3.3
* Poppler >= 0.41 (optional)

## Build Instructions
If you want to add PDF support, you will add macro definition of SUPPORT_PDF.

### *NIX
  1. $ cd src
  1. $ make

### Windows
#### FLTK1.3.x
FLTK builds easy on Windows using MSVC.
It includes CMakeLists.txt and you can create .sln file using cmake build system.
It can compile using FLTK built-in JPEG, PNG and zlib library.

#### libarchive
See https://github.com/libarchive/libarchive/wiki/BuildInstructions#Building_on_Windows

libarchive builds easy on Windows using MSVC.
libarchive depends on some libraries, but there are pre-compiled binaries which are distributed from GnuWin(http://gnuwin32.sourceforge.net/).

#### Poppler
SpRead uses Poppler to read and rendering PDF.
Poppler is difficult to build on Windows.
Poppler depends on many libraries as follows.
  * zlib
  * libpng
    * zlib
  * freetype
    * harfbuzz
    * bzip2
    * zlib
    * libpng
  * openjpeg
    * libpng
    * zlib
    * lcms2
  * lcms2
  * cairo
    * pixman
    * libpng
    * zlib
    * see cairo/README.win32
  * iconv
    * The latest iconv no longer supports nmake and doesn't have CMakeLists. You should install Cygwin and MinGW.
    * see libiconv/README.woe32
  * Qt4 and Qt5 (if you want to use Qt wrapper library.)

In SpRead using Poppler, Poppler building requires only zlib, libpng, freetype, openjpeg, lcms2 and iconv.
Those libraries are easily compiled using MSVC and CMake, or pre-compiled binaries are exist.

Note: If you use binaries which are written in C++ and compile using MinGW, you should compile all dependent libraries and SpRead using MinGW. See https://en.wikipedia.org/wiki/Name_mangling

## TODO
* フィルタの実装
 * アンシャープ
 * アンチエイリアス
 * 非線形フィルタ
 * グレイスケール
 * 平均化
* アプリケーショのアイコンを追加する
* 背景色指定
* サムネイル表示モード
* EXIF情報の処理
