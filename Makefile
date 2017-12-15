CC=g++
CXXFLAGS=-std=c++11 -ldl -lglfw -lGL -lglut -g -MMD -MP
SRCDIR=src
SRCDIRS=$(shell find $(SRCDIR) -type d)
OBJDIR=obj
OBJDIRS=$(pathsubst $(SRCDIRS)/%,$(OBJDIRS)/%,$(SRCDIRS))
SRC=$(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cpp))
_OBJ=$(SRC:.cpp=.o)
OBJ=$(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(_OBJ))
DEPS = ${OBJ:.o=.d}
LIBS=-lm

.PHONY: clean tutorial

all: tutorial

tutorial: bin/tutorial

bin/tutorial: $(OBJ)
	$(CC) $^ $(CXXFLAGS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@[ -d $(@D) ] || mkdir -p $(@D)
	$(CC) $< $(CXXFLAGS) -c -o $@

clean:
	rm -rf bin/tutorial $(OBJDIR)/*

-include ${DEPS}
