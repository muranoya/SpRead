CXX      = $(shell fltk-config --cxx)
DEBUG    = -g
CXXFLAGS = $(shell fltk-config --use-images --cxxflags ) -std=c++11 -O2 -Wall -I. -DSUPPORT_PDF -DSUPPORT_WEBP
LDFLAGS  = $(shell fltk-config --use-images --ldflags )
LDSTATIC = $(shell fltk-config --use-images --ldstaticflags ) -larchive -lpoppler-cpp -lwebp
LINK     = $(CXX)
TARGET = SpRead
OBJS = main.o \
	   Point.o \
	   Config.o \
	   MainWindow.o \
	   ConfigDialog.o \
	   MagDialog.o \
	   OpenDialog.o \
	   Viewer.o \
	   Playlist.o \
	   ImageViewer.o \
	   image.o \
	   BasicImage.o \
	   ImageFile.o StdImgFile.o WebPFile.o PdfFile.o ArchiveFile.o

SRCS = main.cpp \
	   Point.cpp \
	   Config.cpp \
	   MainWindow.cpp \
	   ConfigDialog.cpp \
	   MagDialog.cpp \
	   OpenDialog.cpp \
	   Viewer.cpp \
	   Playlist.cpp \
	   ImageViewer.cpp \
	   image.cpp \
	   BasicImage.cpp \
	   ImageFile.cpp StdImgFile.cpp WebPFile.cpp PdfFile.cpp ArchiveFile.cpp \

.PHONY: all clean

.SUFFIXES: .o .cpp
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEBUG) -c $<
all: $(TARGET)
	$(LINK) -o $(TARGET) $(OBJS) $(LDSTATIC)
	strip $(TARGET)
$(TARGET): $(OBJS)

main.o: main.cpp \
	Config.hpp \
	MainWindow.hpp
Point.o: Point.cpp Point.hpp
Config.o: Config.cpp Config.hpp \
	Viewer.hpp
MainWindow.o: MainWindow.cpp MainWindow.hpp \
	ImageViewer.hpp \
	Config.hpp \
	ConfigDialog.hpp \
	MagDialog.hpp \
	Uncopyable.hpp
ImageFile.o: ImageFile.cpp ImageFile.hpp \
	ArchiveFile.hpp \
	StdImgFile.hpp \
	WebPFile.hpp \
	PdfFile.hpp \
	BasicImage.hpp \
	Uncopyable.hpp
Viewer.o: Viewer.cpp Viewer.hpp \
	image.hpp \
	Point.hpp \
	BasicImage.hpp \
	Uncopyable.hpp \
	for_windows_env.hpp
ImageViewer.o: ImageViewer.cpp ImageViewer.hpp \
	Viewer.hpp \
	Playlist.hpp \
	BasicImage.hpp \
	Uncopyable.hpp
Playlist.o: Playlist.cpp Playlist.hpp \
	ImageFile.hpp \
	for_windows_env.hpp \
	BasicImage.hpp \
	Uncopyable.hpp
image.o: image.cpp image.hpp \
	BasicImage.hpp \
	for_windows_env.hpp
BasicImage.o: BasicImage.cpp BasicImage.hpp
ConfigDialog.o: ConfigDialog.cpp ConfigDialog.hpp \
	Config.hpp \
	Viewer.hpp \
	Uncopyable.hpp
MagDialog.o: MagDialog.cpp MagDialog.hpp \
	Uncopyable.hpp
OpenDialog.o: OpenDialog.cpp OpenDialog.hpp
ArchiveFile.o: ArchiveFile.cpp ArchiveFile.hpp \
	ImageFile.hpp
StdImgFile.o: StdImgFile.cpp StdImgFile.hpp \
	ImageFile.hpp
WebPFile.o: WebPFile.cpp WebPFile.hpp \
	ImageFile.hpp
PdfFile.o: PdfFile.cpp PdfFile.hpp \
	ImageFile.hpp

clean: $(TARGET) $(OBJS)
	rm -f *.o
	rm -f $(TARGET)

