#
# Build for web viewer
# Portions (c) copyright 2020 Jan Delgado
#

.PHONY:=all clean

ifndef OBJDIR
$(error need to define OBJDIR)
endif

ifndef OUTPUTFILE
$(error need to define OUTPUTFILE)
endif

SKETCH_DIR=../sketch
FASTLED_DIR=./FastLED/src

CXXFLAGS:=\
	-I${SKETCH_DIR} \
	-I${FASTLED_DIR} \
	-I./fl-headers \
	-D FASTLED_NO_MCU \
	-D ARDUINO \

LDFLAGS+=-sEXPORT_ES6 -sEXPORTED_RUNTIME_METHODS=ccall,cwrap
LDFLAGS+=--js-library shims.js

# wrapper source -> object
OBJECTS+=$(OBJDIR)/vfb_main.o

$(OBJDIR)/vfb_main.o: vfb_main.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $< -c -o $@

# sketch sources -> objects
SKETCH_SOURCES+=$(wildcard ${SKETCH_DIR}/effects/*.cpp)

OBJECTS+=$(SKETCH_SOURCES:${SKETCH_DIR}/%.cpp=${OBJDIR}/%.o)

$(OBJDIR)/%.o: ${SKETCH_DIR}/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $< -c -o $@

# FastLED sources -> objects
FASTLED_SOURCES+=\
	${FASTLED_DIR}/noise.cpp \
	${FASTLED_DIR}/lib8tion.cpp
FASTLED_OBJDIR=${OBJDIR}/FastLED

OBJECTS+=$(FASTLED_SOURCES:${FASTLED_DIR}/%.cpp=${FASTLED_OBJDIR}/%.o)

$(FASTLED_OBJDIR)/%.o: ${FASTLED_DIR}/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# main executable output
$(OBJDIR)/$(OUTPUTFILE): $(OBJECTS)
	mkdir -p $(OBJDIR)
	$(CXX) $(OBJECTS) $(CXXFLAGS) $(LDFLAGS) -o $@

clean:
	@mkdir -p $(OBJDIR)
	rm -f $(OBJDIR)/**/*

all: clean $(OBJDIR)/$(OUTPUTFILE)
