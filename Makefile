# Derleyici ve bayraklar (flags)
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Çıktı dosyası adı
TARGET = karekod_uretici

# Derlenecek kaynak dosyalar
SRCS = main.c karekod.c

# Kaynak dosyalardan üretilecek obje dosyaları
OBJS = $(SRCS:.c=.o)

# Varsayılan hedef (Sadece 'make' yazıldığında çalışır)
all: $(TARGET)

# Programı derleme kuralı
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# .c dosyalarından .o dosyalarını üretme kuralı
%.o: %.c karekod.h
	$(CC) $(CFLAGS) -c $< -o $@

# Üretilen dosyaları temizleme (make clean)
clean:
	rm -f $(OBJS) $(TARGET) *.exe

.PHONY: all clean
