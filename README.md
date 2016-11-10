SpRead
==========

SpRead is a image viewer for ebook that is composed non-DRM PDF or standard image format.
It can read archive files that are included images.
It is written in C++ using FLTK. So, SpRead can run on Windows, OS X and *NIX.

## Features
* Supports ZIP, TAR, 7Z, CAB, RAR, LHA and LZH, and some compression format (using libarchive and zlib)
* Supports most major common popular image format
* Supports PDF format (using Poppler)
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

## Build
  1. $ cd src
  1. $ make

## TODO
* フィルタの実装
 * アンシャープ
 * アンチエイリアス
 * 非線形フィルタ
 * 二値化(RGB) (判別分析法、モード法、3σ法、Kittler法、p-tile法)
 * グレイスケール
 * 平均化
* アプリケーショのアイコンを追加する
* 背景色指定
* サムネイル表示モード
* EXIF情報の処理
* モノクロモード
