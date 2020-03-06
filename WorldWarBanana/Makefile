.SUFFIXES:
.SUFFIXES:      .cc .o .h

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

FMOD		= $(LIB)/fmod/4.3
MFMOD		= $(LIB)/fmod/Developer/FMOD\ Programmers\ API\ Mac/api/inc
LIBS		= $(FMOD)/libfmodex64.so -lglut -lGLU -lGL $(LIB)/jpeg/libjpeg.a -lm
MLIBS		= -framework GLUT -framework OpenGL -framework Foundation $(LIB)/jpeg/libmacjpeg.a \
			  /Developer/FMOD\ Programmers\ API\ Mac/api/lib/libfmodex.dylib

CXX			= g++
CXXFLAGS	= $(INCLUDEPATH) -O3 -Wall -Werror
DEPFLAGS 	= -MT $@ -MMD -MP -MF $(DEPENDSDIR)$*.d
MKDIR 		= mkdir -p

.PHONY: list clean

$(BUILDDIR)%.o: $(SRCDIR)%.cc
$(BUILDDIR)%.o: $(SRCDIR)%.cc $(DEPENDSDIR)%.d | $(DEPENDSDIR)
	$(MKDIR) $(@D:$(BUILDDIR)%=$(DEPENDSDIR)%)
	$(MKDIR) $(@D)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

labh:	$(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LIB)DebOpenGL.o $(LIBS)

maclabh:	$(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(LIB)MacOpenGL.o $(OBJ) $(MLIBS)
	install_name_tool -change ./libfmodex.dylib /Developer/FMOD\ Programmers\ API\ Mac/api/lib/libfmodex.dylib $@

$(DEPENDS):

include $(wildcard $(DEPENDS))

list:
	@echo "src:\n$(SRC)\n"
	@echo "include:\n$(INCLUDE)\n"
	@echo "obj:\n$(OBJ)\n"
	@echo "depends:\n$(DEPENDS)\n"

clean:
	@rm -f labh maclabh $(OBJ)
	@rm -rf $(BUILDDIR)
	@rm -rf $(DEPENDSDIR)