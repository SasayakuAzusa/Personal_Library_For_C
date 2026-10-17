/* This is library Header for module MAX7219 (led matrix and 7-digit led) */
/* Author: Trần Lê Nhật Đoan */
/*------------------------------------------*/

# --- Cấu hình trình biên dịch ---
CC = gcc
CFLAGS = -Wall -std=c99 -I.

# --- Tên file chạy đầu ra ---
TARGET = max7219_app

# --- Danh sách các file mã nguồn (.c) ---
SRCS = main.c max7219.c

# Tự động chuyển đổi danh sách file .c thành .o (object files)
OBJS = $(SRCS:.c=.o)

# --- Quy tắc mặc định khi gõ lệnh 'make' ---
all: $(TARGET)

# Liên kết các file object thành file chạy hoàn chỉnh
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Quy tắc biên dịch từng file .c thành .o
%.o: %.c max7219.h
	$(CC) $(CFLAGS) -c $< -o $@

# --- Dọn dẹp các file rác sau khi biên dịch ---
clean:
	rm -f $(OBJS) $(TARGET)