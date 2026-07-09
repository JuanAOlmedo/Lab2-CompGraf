CXX := g++
STD := -std=c++23

# --- Directorios ---
SRC_DIR := .
BUILD_DIR := build
TARGET := main

# --- Includes y libs ---
INCLUDES := -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 -I./include
LDFLAGS := -L/opt/homebrew/lib
LIBS := -lFreeImage -lSDL2

# --- Flags de compilación ---
WARNINGS := -Wall -Wextra
CXXFLAGS := $(STD) $(WARNINGS) $(INCLUDES) -MMD -MP

# Modo release por defecto; usar `make BUILD=debug` para debug
BUILD ?= release

ifeq ($(BUILD),debug)
    CXXFLAGS += -O0 -g -fsanitize=address,undefined
    LDFLAGS  += -fsanitize=address,undefined
else
    CXXFLAGS += -O3 -DNDEBUG
endif

# --- Fuentes ---
SRC := main.cpp \
       src/escena.cpp \
       src/luz.cpp \
       src/objeto.cpp \
       src/rayo.cpp \
       src/renderer.cpp \
       src/objetos/cilindro.cpp \
       src/objetos/esfera.cpp \
       src/objetos/malla.cpp \
       src/objetos/plano.cpp \
       src/utils/color.cpp \
       src/utils/imagen.cpp \
       src/utils/material.cpp \
       src/utils/vector.cpp

OBJ := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRC))
DEPS := $(OBJ:.o=.d)

.PHONY: all clean re debug release

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS) $(LIBS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

debug:
	$(MAKE) BUILD=debug

release:
	$(MAKE) BUILD=release

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

re: clean all

-include $(DEPS)
