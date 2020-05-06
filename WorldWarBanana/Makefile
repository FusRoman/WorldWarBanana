LIB			= lib/
SRCDIR 		= src/
INCLUDEDIR 	= include/
BUILDDIR 	= .build/
DEPENDSDIR  = .depends/

# Dossiers des fichiers à ne pas modifier
DEF 		= $(SRCDIR)definitive/
IDEF 		= $(INCLUDEDIR)definitive/
BDEF 		= $(BUILDDIR)definitive/

# Dossiers des fichiers qu'on peut modifier
CUS 		= $(SRCDIR)custom/
ICUS 		= $(INCLUDEDIR)custom/
BCUS 		= $(BUILDDIR)custom/

INCLUDEPATH = -I $(FMOD) -I $(INCLUDEDIR) -I $(IDEF) -I $(ICUS)

SRC 		:= $(wildcard $(DEF)*.cc $(CUS)*.cc)
INCLUDE 	:= $(wildcard $(INCLUDEDIR)*.h $(IDEF)*.h $(ICUS)*.h)
OBJ 		:= $(SRC:$(SRCDIR)%.cc=$(BUILDDIR)%.o)
DEPENDS 	:= $(SRC:$(SRCDIR)%.cc=$(DEPENDSDIR)%.d)

#FMOD		= $(LIB)/fmod/4.3
#MFMOD		= $(LIB)/fmod/Developer/FMOD\ Programmers\ API\ Mac/api/inc
#LIBS		= $(FMOD)/libfmodex64.so -lglut -lGLU -lGL $(LIB)/jpeg/libjpeg.a -lm
#MLIBS		= -framework GLUT -framework OpenGL -framework Foundation $(LIB)/jpeg/libmacjpeg.a \
			  /Developer/FMOD\ Programmers\ API\ Mac/api/lib/libfmodex.dylib

ifeq '$(OS)' "Windows_NT"
# Compilation pour Windows
EXE			= .exe
FLTK_VERSION = fltk-1.4-windows
OPENGL		= $(LIB)OpenGL-windows.o
#MKDIR       =
#FORMAT		 =
RM          = del
CONSOLE 	= -Wl,--subsystem,console
LINKFLTK_IMG =	$(CONSOLE) -mwindows -L$(FLTK_VERSION) \
				-lfltk_images -lfltk_png -lfltk_z -lfltk_jpeg -lfltk_gl -lfltk \
				-lwinmm -lglu32 -lopengl32 -lole32 -luuid \
				-lcomctl32 -lws2_32 -lwinpthread -static

else
ifeq '$(shell uname -s)' "Darwin"
# Compilation pour MacOSX
FLTK_VERSION = fltk-1.4-MacOSX
OPENGL		= $(LIB)OpenGL-macosx.o
FMOD 		= $(LIB)fmod/Developer/FMOD\ Programmers\ API\ Mac/api
#MKDIR		 =
#FORMAT		 =
#RM			 =
LINKFLTK_IMG =	-L$(FLTK_VERSION) -lfltk_png -lfltk_z -lfltk_jpeg -lfltk_gl \
				-lfltk -framework Cocoa -framework AGL -framework OpenGL \
				-framework ApplicationServices $(FMOD)/lib/libfmodex.dylib
EXTRA_FLAGS = -Wno-invalid-source-encoding -Wno-deprecated-declarations \
			  -I /usr/include/freetype2 -I /usr/X11/include -I $(FMOD)/inc

else
# Compilation pour Linux
FLTK_VERSION = fltk-1.4-linux
OPENGL      = $(LIB)OpenGL-linux.o
FMOD 		= $(LIB)fmod/4.3
MKDIR 		= mkdir -p
FORMAT 		= clang-format-9 -i --verbose -style=file
RM			= rm -rf
LINKFLTK_IMG =	$(FMOD)/libfmodex64.so -L$(FLTK_VERSION) -lfltk_jpeg \
				-lfltk_png -lfltk_z -lfltk_gl -lfltk -lGLU -lGL \
				-lXext -lXft -lXrender -lfontconfig -ldl -lm -lX11 -lXfixes
EXTRA_FLAGS = -I /usr/include/freetype2 -I /usr/X11/include -I $(FMOD) 

endif
endif

CXX         = g++
CXXFLAGS	= -O3 $(INCLUDEPATH) -Wall -Werror $(EXTRA_FLAGS) 
DEPFLAGS 	= -MT $@ -MMD -MP -MF $(DEPENDSDIR)$*.d

.PHONY: list format clean

.SUFFIXES: .cc .o .h $(EXE)

labh$(EXE):	$(OBJ)
	$(CXX) -no-pie $(CXXFLAGS) -o $@ $(OBJ) $(OPENGL) $(LINKFLTK_IMG)
ifeq '$(OS)' "Darwin"
	install_name_tool -change ./libfmodex.dylib fmod/Developer/FMOD\ Programmers\ API\ Mac/api/lib/libfmodex.dylib $@
endif

#maclabh:	$(OBJ)
#	$(CXX) $(CXXFLAGS) -o $@ $(LIB)MacOpenGL.o $(OBJ) $(MLIBS)
#	install_name_tool -change ./libfmodex.dylib /Developer/FMOD\ Programmers\ API\ Mac/api/lib/libfmodex.dylib $@

$(BUILDDIR)%.o: $(SRCDIR)%.cc
$(BUILDDIR)%.o: $(SRCDIR)%.cc $(DEPENDSDIR)%.d | $(DEPENDSDIR)
	@$(MKDIR) $(@D:$(BUILDDIR)%=$(DEPENDSDIR)%)
	@$(MKDIR) $(@D)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

$(DEPENDSDIR):
	$(MKDIR) $@

$(DEPENDS):

include $(wildcard $(DEPENDS))

list:
	@echo "src:\n$(SRC)\n"
	@echo "include:\n$(INCLUDE)\n"
	@echo "obj:\n$(OBJ)\n"
	@echo "depends:\n$(DEPENDS)\n"

format:
	$(FORMAT) $(SRC)
	$(FORMAT) $(INCLUDE)

clean:
	@$(RM) labh maclabh
	@$(RM) $(BUILDDIR)
	@$(RM) $(DEPENDSDIR)