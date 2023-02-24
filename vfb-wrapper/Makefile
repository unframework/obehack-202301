.PHONY:=all clean

ifndef OBJDIR
$(error need to define OBJDIR)
endif

ifndef OUTPUTFILE
$(error need to define OUTPUTFILE)
endif

CXXFLAGS:=
LDFLAGS+=-sEXPORT_ES6 -sEXPORTED_RUNTIME_METHODS=ccall,cwrap

SOURCES=$(wildcard *.cpp)
SOURCES+=$(wildcard effects/*.cpp)

OBJECTS=$(SOURCES:%.cpp=${OBJDIR}/%.o)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $< -c -o $@

$(OBJDIR)/$(OUTPUTFILE): $(OBJECTS)
	mkdir -p $(OBJDIR)
	$(CXX) $(OBJECTS) $(CXXFLAGS) $(LDFLAGS) -o $@

clean:
	@mkdir -p $(OBJDIR)
	rm -f $(OBJDIR)/**/*

all: clean $(OBJDIR)/$(OUTPUTFILE)
