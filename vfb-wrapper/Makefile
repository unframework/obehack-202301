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

SRCDIR=.
FASTLED_DIR=${SRCDIR}/FastLED/src

CXXFLAGS:=
LDFLAGS+=-sEXPORT_ES6 -sEXPORTED_RUNTIME_METHODS=ccall,cwrap

# main sources -> objects
SOURCES=$(wildcard ${SRCDIR}/*.cpp)
SOURCES+=$(wildcard ${SRCDIR}/effects/*.cpp)

OBJECTS=$(SOURCES:${SRCDIR}/%.cpp=${OBJDIR}/%.o)

$(OBJDIR)/%.o: ${SRCDIR}/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $< -c -o $@

# FastLED sources -> objects
# (using additional options for NO_MCU headers and defines)
FASTLED_SOURCES+=\
	${FASTLED_DIR}/noise.cpp \
	${FASTLED_DIR}/lib8tion.cpp
FASTLED_OBJDIR=${OBJDIR}/FastLED

OBJECTS+=$(FASTLED_SOURCES:${FASTLED_DIR}/%.cpp=${FASTLED_OBJDIR}/%.o)

$(FASTLED_OBJDIR)/%.o: ${FASTLED_DIR}/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) \
		-D FASTLED_NO_MCU \
		-D ARDUINO \
		-I${SRCDIR}/fl-headers \
		-c \
		$< -o $@

# main executable output
$(OBJDIR)/$(OUTPUTFILE): $(OBJECTS)
	mkdir -p $(OBJDIR)
	$(CXX) $(OBJECTS) $(CXXFLAGS) $(LDFLAGS) -o $@

clean:
	@mkdir -p $(OBJDIR)
	rm -f $(OBJDIR)/**/*

all: clean $(OBJDIR)/$(OUTPUTFILE)
