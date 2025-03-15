CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror
SRC = main.cpp expression.cpp
OBJ = $(SRC:.cpp=.o)
TEST_SRC = test_expression.cpp
TEST_OBJ = $(TEST_SRC:.cpp=.o)
TARGET = differentiator
TEST_TARGET = test_expression
all: $(TARGET)
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)
test: $(TEST_TARGET)
	./$(TEST_TARGET)
$(TEST_TARGET): $(TEST_OBJ) expression.o
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_OBJ) expression.o
clean:
	rm -f $(OBJ) $(TEST_OBJ) $(TARGET) $(TEST_TARGET)
