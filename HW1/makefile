SRC_DIR = src
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=%.o)
EXEC = hw
COMPILER = clang++
DELETE = rm

$(EXEC) : $(OBJECTS)
	$(COMPILER) $(OBJECTS) -o $(EXEC)

%.o : $(SRC_DIR)/%.cpp
	$(COMPILER) -c $<

.PHONY : all
all : $(EXEC)

.PHONY : clean
clean :
	-$(DELETE) $(OBJECTS) core
