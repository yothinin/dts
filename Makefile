CC = gcc
CFLAGS = -g -Wall
TARGET = dts-simple
INCLUDE = dts_functions.c
LIBRARY = `pkg-config --libs --cflags gtk+-2.0` `mysql_config --cflags --libs`  -lcurl 
OPTIONS = -DGTK_DISABLE_DEPRECATE

all: $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(INCLUDE) $(LIBRARY) $(OPTIONS)

clean:
	$(RM) $(TARGET)

