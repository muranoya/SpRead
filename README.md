SpRead
==========

SpRead is a image viewer for ebook that is composed non-DRM PDF or standard image format.
It can read archive files that are included images.
It is written in C++ using FLTK. So, SpRead can run on Windows, OS X and *NIX.

## Features
* Supports ZIP, TAR, 7Z, CAB, RAR, LHA and LZH, and some compression format
* Supports most major common popular image format(JPEG, PNG, WebP)
* Supports PDF format
* Reads directories which are included images
* Spread view mode (double page mode)
 * Both left-to-right and right-to-left reading modes
 * Auto control of spread view mode
* Prefetch of images

## Requirement
* C++ Compiler which supports C++11 or above
* libarchive >= 3.1.2
* FLTK 1.x
* Poppler >= 0.41 (optional)
* libwebp (optional)

## Build Instructions
If you want to add PDF, WebP support, you will add macro definition of SUPPORT_PDF, SUPPORT_WEBP, respectively.

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

## Hacks
### How to support new image format
 * WebPFile.cpp and WebPFile.hpp are samples.
 1. Creates class which inherits `ImageFile`.
 2. New class must implement five methods at least below.
    * `const std::string &path() const;`
    * `BasicImage *loadImage(int index);`
    * `bool isOpenable(const std::string&);`
    * `bool open(const std::string&, const RawData&, std::vector<ImageItem*>&);`
    * `const StringVec &enum_exts();`
    * These methods must be thread-safe.
    * `path` method returnes a file path.
    * `loadImage` method returns a raw image.
    * `isOpenable` method decides whether to open given file name.
    * `open method` adds entry to argument vector.
    * `enum_exts` method returns the extensions that support by its class.
 3. Adds three functors(`isOpenable`, `open`, `enum_exts`) to the either vectors of `imgs`(image such as jpeg, png), `docs`(document such as pdf), `archs`(archive such as zip).
 4. Finally, edits Makefile.

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
