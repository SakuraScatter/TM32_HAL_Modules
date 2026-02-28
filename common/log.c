#include "log.h"
#include <string.h>

#define LOG_QUEUE_SIZE 16       // 队列长度（根据系统内存调整）
#define LOG_QUEUE_BUF_SIZE 128 // 单条日志最大长度

// 环形队列结构体
typedef struct {
    char buf[LOG_QUEUE_SIZE][LOG_QUEUE_BUF_SIZE];
    uint8_t head;  // 读指针
    uint8_t tail;  // 写指针
    uint8_t count; // 队列中日志条数（无符号数）
} LogQueue;

static LogQueue log_queue = {0};
static volatile uint8_t log_queue_lock = 0; // 简易锁（裸机/RTOS通用）

// 默认输出函数
static void default_log_output(const char* str) {
    printf("%s", str);
}

static LogOutputFunc log_output = default_log_output;
static const char* log_level_names[] = {"[DEBUG]","[INFO] ","[WARN] ","[ERROR]","[FATAL]","[NONE] "};
static char log_buf[256];

// 简易加锁/解锁
static void log_lock(void) {
    while (log_queue_lock);
    log_queue_lock = 1;
}

static void log_unlock(void) {
    log_queue_lock = 0;
}

// 日志入队
static int log_enqueue(const char* str) {
    log_lock();
    if (log_queue.count >= LOG_QUEUE_SIZE) {
        log_unlock();
        return -1; // 队列满，丢弃日志
    }
    strncpy(log_queue.buf[log_queue.tail], str, LOG_QUEUE_BUF_SIZE - 1);
    log_queue.buf[log_queue.tail][LOG_QUEUE_BUF_SIZE - 1] = '\0';
    log_queue.tail = (log_queue.tail + 1) % LOG_QUEUE_SIZE;
    log_queue.count++;
    log_unlock();
    return 0;
}

// 日志出队（在中断/线程中调用）
static char* log_dequeue(void) {
    log_lock();
    if (!log_queue.count) { 
        log_unlock();
        return NULL;
    }
    char* str = log_queue.buf[log_queue.head];
    log_queue.head = (log_queue.head + 1) % LOG_QUEUE_SIZE;
    log_queue.count--;
    log_unlock();
    return str;
}

// 异步输出处理函数
void log_process_queue(void) {
    char* str = log_dequeue();
    if (str != NULL && log_output != NULL) {
        log_output(str);
    }
}

// 核心日志输出函数
// 核心日志输出函数
void log_print(LogLevel level, const char* file, int line, const char* fmt, ...) {
    // 级别过滤：逻辑不变
    if ((int)level < LOG_GLOBAL_LEVEL || level >= LOG_LEVEL_NONE) {
        return;
    }

    va_list args;
    int len = 0;
    size_t buf_size = sizeof(log_buf);
    memset(log_buf, 0, buf_size);
    
    // ========== file为NULL时不拼接文件名和行号 ==========
    if (file != NULL) {
        // file非NULL：保留原有文件名/行号拼接逻辑
        const char* short_file = strrchr(file, '/');
        if (short_file == NULL) short_file = strrchr(file, '\\');
        short_file = (short_file == NULL) ? file : (short_file + 1);
        
        // 拼接日志级别 + 文件名 + 行号
        len = snprintf(log_buf, buf_size - 1, 
                       "%s [%s:%d] ", log_level_names[level], short_file, line);
    } else {
        // file为NULL：仅拼接日志级别，不显示文件名和行号
        len = snprintf(log_buf, buf_size - 1, 
                       "%s ", log_level_names[level]);
    }

    // 边界检查：避免缓冲区不足导致异常
    if (len < 0 || len >= (int)(buf_size - 1)) {
        log_buf[buf_size - 1] = '\0';
        log_enqueue(log_buf);
        return;
    }

    // 拼接用户格式化内容：兼容fmt为NULL的容错
    if (fmt != NULL) {
        va_start(args, fmt);
        len += vsnprintf(log_buf + len, buf_size - len - 1, fmt, args);
        va_end(args);
    } else {
        len += snprintf(log_buf + len, buf_size - len - 1, "[fmt is NULL]");
    }

    // 补充换行符，确保缓冲区不越界
    if (len < (int)(buf_size - 1)) {
        log_buf[len] = '\n';
        log_buf[len + 1] = '\0';
    } else {
        log_buf[buf_size - 1] = '\0';
    }

    log_enqueue(log_buf);
}
