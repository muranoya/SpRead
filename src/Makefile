CXX      = $(shell fltk-config --cxx)
DEBUG    = -g
CXXFLAGS = $(shell fltk-config --use-images --cxxflags ) -std=c++11 -O2 -Wall -I.
LDFLAGS  = $(shell fltk-config --use-images --ldflags )
LDSTATIC = $(shell fltk-config --use-images --ldstaticflags ) -larchive -lpoppler-cpp
LINK     = $(CXX)
TARGET = SpRead
OBJS = main.o   Size.o   Point.o   App.o \
	   MainWindow.o   ImageFile.o   Viewer.o \
	   Playlist.o   ImageViewer.o   image.o \
	   BasicImage.o Prefetcher.o \
	   SettingDialog.o MagDialog.o
SRCS = main.cpp Size.cpp Point.cpp App.cpp \
	   MainWindow.cpp ImageFile.cpp Viewer.cpp \
	   Playlist.cpp ImageViewer.cpp image.cpp \
	   BasicImage.cpp Prefetcher.cpp \
	   SettingDialog.cpp MagDialog.cpp

.PHONY: all clean

.SUFFIXES: .o .cpp
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEBUG) -c $<
all: $(TARGET)
	$(LINK) -o $(TARGET) $(OBJS) $(LDSTATIC)
	strip $(TARGET)
$(TARGET): $(OBJS)

main.o: main.cpp App.hpp MainWindow.hpp
Size.o: Size.cpp Size.hpp
Point.o: Point.cpp Point.hpp
App.o: App.cpp App.hpp Viewer.hpp
MainWindow.o: MainWindow.cpp MainWindow.hpp ImageViewer.hpp App.hpp
ImageFile.o: ImageFile.cpp ImageFile.hpp
Viewer.o: Viewer.cpp Viewer.hpp image.hpp Size.hpp Point.hpp for_windows_env.hpp
ImageViewer.o: ImageViewer.cpp ImageViewer.hpp Viewer.hpp Playlist.hpp
Playlist.o: Playlist.cpp Playlist.hpp ImageFile.hpp for_windows_env.hpp
image.o: image.cpp image.hpp for_windows_env.hpp
BasicImage.o: BasicImage.cpp BasicImage.hpp
Prefetcher.o: Prefetcher.cpp Prefetcher.hpp DataCache.hpp ImageFile.hpp
SettingDialog.o: SettingDialog.cpp SettingDialog.hpp App.hpp Viewer.hpp
MagDialog.o: MagDialog.cpp MagDialog.hpp

clean: $(TARGET) $(OBJS)
	rm -f *.o
	rm -f $(TARGET)

