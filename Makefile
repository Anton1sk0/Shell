# Compiler and compiler flags
CC = g++
CFLAGS = -Wall -std=c++11

# Source files
SRCS = AliasManager.cpp HistoryManager.cpp JobManager.cpp main.cpp Shell.cpp WildcardEvaluator.cpp
HDRS = AliasManager.h HistoryManager.h JobsManager.h List.h Shell.h WildcardEvaluator.h

# Object files
OBJS = $(SRCS:.cpp=.o)

# Name of the executable
TARGET = out

# Rule to build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to build object files
%.o: %.cpp $(HDRS)
	$(CC) $(CFLAGS) -c -o $@ $<

# Rule to clean up object files and executable
clean:
	rm -f $(OBJS) $(TARGET)

# Rule to run the executable
run: $(TARGET)
	./$(TARGET)
