# Makefile for cryptodb-c

CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2 -I./include -I/usr/include/postgresql
LDFLAGS = -lpq
TARGET = cryptodb
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

# Source files
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/cipher.c $(SRC_DIR)/crypto_utils.c $(SRC_DIR)/database.c
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Default target
all: $(BUILD_DIR) $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build target executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)
	@echo "Clean complete"

# Install dependencies (Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install -y postgresql postgresql-contrib libpq-dev

# Setup database
setup-db:
	@echo "Setting up database..."
	@echo "Please ensure PostgreSQL is running and you have appropriate permissions."
	sudo -u postgres psql -c "CREATE DATABASE cryptodb;" || true
	sudo -u postgres psql cryptodb -f sql/schema.sql

# Run the application
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean install-deps setup-db run
