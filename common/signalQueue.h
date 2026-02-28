#ifndef SIGNALQUEUE_H
#define SIGNALQUEUE_H

#include <stdint.h>
#include <string.h>

/* ===================== 布尔类型替代定义 ===================== */
// 用uint8_t模拟布尔值，兼容无bool类型的编译器
#define SIGNALQUEUE_FALSE    0U
#define SIGNALQUEUE_TRUE     1U
typedef uint8_t SignalQueue_Bool;

#define SIGNALQUEUE_NONE 0		// 非法信号，信号值不可以为0
/* ========================== 配置项 =========================== */
// 信号队列最大容量
#ifndef SIGNALQUEUE_MAX_CAPACITY
#define SIGNALQUEUE_MAX_CAPACITY    16U
#endif

// 信号类型定义
typedef uint8_t SignalId_t;

/* ===================== 信号队列控制块 ===================== */
typedef struct {
    SignalId_t buffer[SIGNALQUEUE_MAX_CAPACITY]; 	 // 信号缓冲区
    uint8_t readIdx;                               // 读索引（出队位置）
    uint8_t writeIdx;                              // 写索引（入队位置）
    uint8_t count;                                 // 当前信号数量
} SignalQueue_t;

/* ===================== 公开接口 ===================== */
/**
 * @brief 初始化信号队列
 * @param queue 指向信号队列控制块的指针
 * @return SIGNALQUEUE_TRUE-成功，SIGNALQUEUE_FALSE-失败（参数无效）
 */
SignalQueue_Bool signalQueue_Init(SignalQueue_t* queue);

/**
 * @brief 信号入队（FIFO）
 * @param queue 指向信号队列控制块的指针
 * @param signalId 要入队的信号ID（非0）
 * @return SIGNALQUEUE_TRUE-成功，SIGNALQUEUE_FALSE-失败（队列满/参数无效/信号ID为0）
 */
SignalQueue_Bool signalQueue_Push(SignalQueue_t* queue, SignalId_t signalId);

/**
 * @brief 信号出队（FIFO）
 * @param queue 指向信号队列控制块的指针
 * @param signalId 输出参数：存储出队的信号ID
 * @return SIGNALQUEUE_TRUE-成功，SIGNALQUEUE_FALSE-失败（队列空/参数无效）
 */
SignalQueue_Bool signalQueue_Pop(SignalQueue_t* queue, SignalId_t* signalId);

/**
 * @brief 检查队列是否为空
 * @param queue 指向信号队列控制块的指针
 * @return SIGNALQUEUE_TRUE-空，SIGNALQUEUE_FALSE-非空
 */
SignalQueue_Bool signalQueue_IsEmpty(SignalQueue_t* queue);

/**
 * @brief 检查队列是否为满
 * @param queue 指向信号队列控制块的指针
 * @return SIGNALQUEUE_TRUE-满，SIGNALQUEUE_FALSE-非满
 */
SignalQueue_Bool signalQueue_IsFull(SignalQueue_t* queue);

/**
 * @brief 获取队列中当前信号数量
 * @param queue 指向信号队列控制块的指针
 * @return 当前信号数（0~SIGNALQUEUE_MAX_CAPACITY）
 */
uint8_t signalQueue_GetCount(SignalQueue_t* queue);

/**
 * @brief 清空信号队列
 * @param queue 指向信号队列控制块的指针
 * @return SIGNALQUEUE_TRUE-成功，SIGNALQUEUE_FALSE-失败（参数无效）
 */
SignalQueue_Bool signalQueue_Clear(SignalQueue_t* queue);

#endif /* SIGNALQUEUE_H */

