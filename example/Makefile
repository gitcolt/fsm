TARGET = main
SRCS = main.c

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -g -o $@ $^

.PHONY: clean
clean:
	rm -f $(TARGET)
