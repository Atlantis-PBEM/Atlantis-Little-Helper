
CXX      = g++
CXXFLAGS = -g -O2
LDFLAGS  = 
DEFS     = -DHAVE_CONFIG_H
LIBS     =  -L/usr/lib/arm-linux-gnueabihf -pthread   -L/usr/lib/arm-linux-gnueabihf   -lwx_gtk2u_richtext-2.8 -lwx_gtk2u_aui-2.8 -lwx_gtk2u_xrc-2.8 -lwx_gtk2u_qa-2.8 -lwx_gtk2u_html-2.8 -lwx_gtk2u_adv-2.8 -lwx_gtk2u_core-2.8 -lwx_baseu_xml-2.8 -lwx_baseu_net-2.8 -lwx_baseu-2.8 
CPPFLAGS =  -I/usr/lib/arm-linux-gnueabihf/wx/include/gtk2-unicode-release-2.8 -I/usr/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread


#CC          = c++
#CXXFLAGS    = -O2  -Wall
#CXXFLAGS_WX = $(shell wx-config --cflags)
#LIBS_WX     = $(shell wx-config --libs)
#CXXFLAGS_PYTHON =-I/usr/include/python2.2 -DALH_PYTHON_EXTEND
#LIBS_PYTHON =-lpython2.2


OBJECTS     = collection.o cfgfile.o cstr.o files.o hash.o objs.o\
              data.o errs.o atlaparser.o \
              ahapp.o ahframe.o consts_ah.o editpane.o \
              editsframe.o extend.o extend_no.o \
			  flagsdlg.o hexfilterdlg.o listcoledit.o \
              listpane.o mapframe.o \
              mappane.o msgframe.o optionsdlg.o \
              unitfilterdlg.o unitframe.o unitframefltr.o unitpane.o \
              unitpanefltr.o unitsplitdlg.o utildlgs.o

TARGETS     = ah

all: $(TARGETS)

ah: $(patsubst %.o,obj/%.o,$(OBJECTS))
	$(CXX) -s $(LDFLAGS) -o $@ $(patsubst %.o,obj/%.o,$(OBJECTS)) $(LIBS)

clean:
	rm -f $(patsubst %.o,obj/%.o,$(OBJECTS)) $(TARGETS)

$(patsubst %.o,obj/%.o,$(OBJECTS)): obj/%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(DEFS) -o $@ $<
