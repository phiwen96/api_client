CXX := clang++
CXX_FLAGS = -std=c++2a -stdlib=libc++ -fmodules-ts -fmodules -fbuiltin-module-map -fimplicit-modules -fimplicit-module-maps -fprebuilt-module-path=.
INCLUDE_NLOHMANN := -I/opt/homebrew/Cellar/nlohmann-json/3.10.5/include

PROJ_DIR := $(CURDIR)
BUILD_DIR := $(PROJ_DIR)/build
OBJ_DIR := $(BUILD_DIR)/obj
MODULES_DIR := $(PROJ_DIR)/modules

MAIN := $(BUILD_DIR)/main

MODULES := $(wildcard $(MODULES_DIR)/*.cpp)

__OBJ := $(subst .cpp,.pcm,$(MODULES))
_OBJ := $(foreach F,$(__OBJ),$(word $(words $(subst /, ,$F)),$(subst /, ,$F)))
OBJ := $(foreach name, $(_OBJ), $(addprefix $(OBJ_DIR)/, $(name)))

_BUILD_DIRS := libs obj docs tests
BUILD_DIRS := $(foreach dir, $(_BUILD_DIRS), $(addprefix $(BUILD_DIR)/, $(dir)))


$(MAIN): $(OBJ_DIR)/main.o $(MODULES)
	$(CXX) $(CXX_FLAGS) $(OBJ_DIR)/main.o -o $@

$(OBJ_DIR)/main.o: $(PROJ_DIR)/main.cpp $(OBJ_DIR)/Client.pcm
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -o $@ $(INCLUDE_NLOHMANN)

$(OBJ_DIR)/Client.pcm: $(MODULES_DIR)/Client.cpp
	$(CXX) $(CXX_FLAGS) $(addprefix -fmodule-file=, $(filter-out $<, $^)) -c $< -Xclang -emit-module-interface -o $@


directories := $(foreach dir, $(BUILD_DIRS), $(shell [ -d $(dir) ] || mkdir -p $(dir)))

clean:
	rm -rf $(BUILD_DIR)/*