add_executable(SpRead
App.cpp
App.hpp
DataCache.hpp
ImageFile.cpp
ImageFile.hpp
ImageViewer.cpp
ImageViewer.hpp
MainWindow.cpp
MainWindow.hpp
Playlist.cpp
Playlist.hpp
Point.cpp
Point.hpp
Prefetcher.cpp
Prefetcher.hpp
Viewer.cpp
Viewer.hpp
for_windows_env.hpp
image.cpp
image.hpp
BasicImage.cpp
BasicImage.hpp
MagDialog.cpp
MagDialog.hpp
SettingDialog.cpp
SettingDialog.hpp
main.cpp)

target_compile_features(SpRead PRIVATE cxx_range_for)

target_link_libraries(SpRead ${FLTK_LIBRARIES})
target_link_libraries(SpRead ${LIBARCHIVE_LIBRARIES})

