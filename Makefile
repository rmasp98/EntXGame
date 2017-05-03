CXX       := g++
LD        := g++
FLAGS	  := -std=c++11 -Wall

MODULES   := common main
SRC_DIR   := $(addprefix src/,$(MODULES))
BUILD_DIR := $(addprefix build/,$(MODULES))

SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ       := $(patsubst src/%.cpp,build/%.o,$(SRC))
EXE       := firstProg

INCLUDES   = -I include
LIBS += -lglut -lGL -lGLU -lX11 -lXxf86vm -lpthread -lXrandr -lXi -lglfw -lGLEW -lSOIL -lentityx -lfreetype

vpath %.cpp $(SRC_DIR)

define make-goal
$1/%.o: %.cpp
	$(CXX) $(FLAGS) $(INCLUDES) -c $$< -o $$@
endef

.PHONY: all checkdirs clean

all: checkdirs $(EXE)

$(EXE): $(OBJ)
	$(CXX) $(FLAGS) $^ -o $@ $(LIBS)


checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf build
	@rm -f $(EXE)

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))
