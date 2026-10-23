/* This is labrary Header for module DS3231 (real time module) */
/* Author: Trần Lê Nhật Đoan */
/*------------------------------------------*/

# --- Cấu hình trình biên dịch ---
CC = gcc
CFLAGS = -Wall -std=c99 -I.

# --- Tên file chạy đầu ra ---
TARGET = ds3231_app

# --- Danh sách các file mã nguồn (.c) ---
SRCS = main.c ds3231.c

# Tự động chuyển đổi danh sách file .c thành .o (object files)
OBJS = $(SRCS:.c=.o)

# --- Thư viện liên kết ngoài (ví dụ: -lwiringPi cho Raspberry Pi) ---
LIBS = -lwiringPi

# --- Quy tắc mặc định khi gõ lệnh 'make' ---
all: $(TARGET)

# Liên kết các file object thành file chạy hoàn chỉnh (kèm theo thư viện wiringPi)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Quy tắc biên dịch từng file .c thành .o
%.o: %.c ds3231.h
	$(CC) $(CFLAGS) -c $< -o $@

# --- Dọn dẹp các file rác sau khi biên dịch ---
clean:
	rm -f $(OBJS) $(TARGET)