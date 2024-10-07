# Compiler and flags
CC = gcc
CFLAGS = -Wall

# Target executable
TARGET = rush

# Source files
SRCS = rush.c rush_utils.c

# Object files
OBJS = $(SRCS:.c=.o)

# Header files
HDRS = rush_utils.h

# Default rule to build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile .c files into .o files
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to remove object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)

# Run the shell
run: $(TARGET)
	./$(TARGET)
