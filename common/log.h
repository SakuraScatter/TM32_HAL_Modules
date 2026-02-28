#ifndef __LOG_H__
#define __LOG_H__

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

// ========== 编译期日志裁剪开关 ==========
// 定义此宏则仅保留ERROR/FATAL级别，其他级别直接编译为空
#ifdef LOG_RELEASE_MODE
#define LOG_GLOBAL_LEVEL LOG_LEVEL_ERROR
// 编译期直接删除DEBUG/INFO/WARN日志（无函数调用开销）
#define LOG_DEBUG(...) ((void)0)
#define LOG_INFO(...)  ((void)0)
#define LOG_WARN(...)  ((void)0)
// 仅保留ERROR/FATAL和通用LOG（通用LOG映射为ERROR）
#define LOG(...)       log_print(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#else
// 调试模式：保留所有级别
typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_NONE
} LogLevel;

#ifndef LOG_GLOBAL_LEVEL
#define LOG_GLOBAL_LEVEL LOG_LEVEL_DEBUG
#endif

typedef void (*LogOutputFunc)(const char* str);
void log_set_output(LogOutputFunc func);
void log_print(LogLevel level, const char* file, int line, const char* fmt, ...);

#define LOG_DEBUG(fmt, ...)  log_print(LOG_LEVEL_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)   log_print(LOG_LEVEL_INFO,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)   log_print(LOG_LEVEL_WARN,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)  log_print(LOG_LEVEL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...)  log_print(LOG_LEVEL_FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG(fmt, ...)        log_print(LOG_LEVEL_INFO,  NULL, __LINE__, fmt, ##__VA_ARGS__)
#endif // LOG_RELEASE_MODE

void log_process_queue(void);		// 异步输出处理函数
	
#endif // __LOG_H__

