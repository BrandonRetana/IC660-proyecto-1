CC = gcc
CFLAGS = -c
OUT_DIR = output/objects
SRC_DIR = src
DEPS_DIR = $(SRC_DIR)/dependencies
FREQ_DIR = output/frequencies
BIN_DIR = output/bin
TEMP_DIR = output/temp
DECODE_DIR = output/decodes
BOOKS_DIR = output/books

DEPS_OBJS = $(OUT_DIR)/freq.o $(OUT_DIR)/file_utils.o
DEPS_SRCS = $(DEPS_DIR)/huffman/huffmancodes.c $(DEPS_DIR)/huffman/huffmantree.c $(DEPS_DIR)/frequency/frequency.c $(DEPS_DIR)/share/share.c

SERIAL_OBJS = $(OUT_DIR)/serial_compressor.o $(OUT_DIR)/serial_decompressor.o
FORK_OBJS = $(OUT_DIR)/fork_compressor.o $(OUT_DIR)/fork_decompressor.o
PTHREAD_OBJS = $(OUT_DIR)/pthread_compressor.o $(OUT_DIR)/pthread_decompressor.o

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

all: serial fork pthread

serial: $(SERIAL_OBJS)

fork: $(FORK_OBJS)

pthread: $(PTHREAD_OBJS)

$(DEPS_OBJS): $(DEPS_SRCS)
	$(CC) $(CFLAGS) $(DEPS_DIR)/huffman/huffmancodes.c -o $(OUT_DIR)/huffmancodes.o
	$(CC) $(CFLAGS) $(DEPS_DIR)/huffman/huffmantree.c -o $(OUT_DIR)/huffmantree.o
	$(CC) $(CFLAGS) $(DEPS_DIR)/frequency/frequency.c -o $(OUT_DIR)/freq.o
	$(CC) $(CFLAGS) $(DEPS_DIR)/share/share.c -o $(OUT_DIR)/file_utils.o

$(SERIAL_OBJS): $(DEPS_OBJS) $(SRC_DIR)/serial/compressor.c $(SRC_DIR)/serial/decompressor.c
	$(CC) $(SRC_DIR)/serial/compressor.c -o $(OUT_DIR)/serial_compressor
	$(CC) $(SRC_DIR)/serial/decompressor.c -o $(OUT_DIR)/serial_decompressor

$(FORK_OBJS): $(DEPS_OBJS) $(SRC_DIR)/fork/compressor.c $(SRC_DIR)/fork/decompressor.c
	$(CC) $(SRC_DIR)/fork/compressor.c -o $(OUT_DIR)/fork_compressor
	$(CC) $(SRC_DIR)/fork/decompressor.c -o $(OUT_DIR)/fork_decompressor

$(PTHREAD_OBJS): $(DEPS_OBJS) $(SRC_DIR)/pthreads/compressor.c $(SRC_DIR)/pthreads/decompressor.c
	$(CC) $(SRC_DIR)/pthreads/compressor.c -o $(OUT_DIR)/pthread_compressor
	$(CC) $(SRC_DIR)/pthreads/decompressor.c -o $(OUT_DIR)/pthread_decompressor

clean:
	rm -f $(FREQ_DIR)/*
	rm -f $(BIN_DIR)/*
	rm -f $(BOOKS_DIR)/*
	rm -f $(DECODE_DIR)/*
	rm -f $(OUT_DIR)/*
	rm -f $(TEMP_DIR)/*
