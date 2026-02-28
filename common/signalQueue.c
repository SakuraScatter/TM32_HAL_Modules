#include "signalQueue.h"

/**
 * @brief 初始化信号队列
 * @param queue 指向信号队列控制块的指针
 * @return SIGNALQUEUE_TRUE-成功，SIGNALQUEUE_FALSE-失败（参数无效）
 */
SignalQueue_Bool signalQueue_Init(SignalQueue_t* queue)
{
    if (queue == NULL)
    {
        return SIGNALQUEUE_FALSE;
    }

    // 重置索引和计数，缓冲区无需清零（入队时覆盖）
    queue->readIdx = 0;
    queue->writeIdx = 0;
    queue->count = 0;

    return SIGNALQUEUE_TRUE;
}

/**
 * @brief 信号入队（FIFO）
 * @param queue 指向信号队列控制块的指针
 * @param signalId 要入队的信号ID（非0）
 * @return SIGNALQUEUE_TRUE-成功，SIGNALQUEUE_FALSE-失败（队列满/参数无效/信号ID为0）
 */
SignalQueue_Bool signalQueue_Push(SignalQueue_t* queue, SignalId_t signalId)
{
    // 参数校验：空指针/无效信号ID直接返回失败
    if (queue == NULL || signalId == SIGNALQUEUE_NONE)
    {
        return SIGNALQUEUE_FALSE;
    }

    // 队列满则入队失败
    if (signalQueue_IsFull(queue) == SIGNALQUEUE_TRUE)
    {
        return SIGNALQUEUE_FALSE;
    }

    // 存储信号ID到缓冲区
    queue->buffer[queue->writeIdx] = signalId;

    // 更新写索引（环形缓冲区：取模实现循环）
    queue->writeIdx = (queue->writeIdx + 1) % SIGNALQUEUE_MAX_CAPACITY;
    // 信号计数+1
    queue->count++;

    return SIGNALQUEUE_TRUE;
}

/**
 * @brief 信号出队（FIFO）
 * @param queue 指向信号队列控制块的指针
 * @param signalId 输出参数：存储出队的信号ID
 * @return SIGNALQUEUE_TRUE-成功，SIGNALQUEUE_FALSE-失败（队列空/参数无效）
 */
SignalQueue_Bool signalQueue_Pop(SignalQueue_t* queue, SignalId_t* signalId)
{
    // 参数校验：空指针直接返回失败
    if (queue == NULL || signalId == NULL)
    {
        return SIGNALQUEUE_FALSE;
    }

    // 队列空则出队失败
    if (signalQueue_IsEmpty(queue) == SIGNALQUEUE_TRUE)
    {
        return SIGNALQUEUE_FALSE;
    }

    // 读取信号ID
    *signalId = queue->buffer[queue->readIdx];

    // 更新读索引（环形缓冲区：取模实现循环）
    queue->readIdx = (queue->readIdx + 1) % SIGNALQUEUE_MAX_CAPACITY;
    // 信号计数-1
    queue->count--;

    return SIGNALQUEUE_TRUE;
}

/**
 * @brief 检查队列是否为空
 * @param queue 指向信号队列控制块的指针
 * @return SIGNALQUEUE_TRUE-空，SIGNALQUEUE_FALSE-非空
 */
SignalQueue_Bool signalQueue_IsEmpty(SignalQueue_t* queue)
{
    if (queue == NULL)
    {
        return SIGNALQUEUE_TRUE; // 参数无效视为空
    }
    return (queue->count == 0) ? SIGNALQUEUE_TRUE : SIGNALQUEUE_FALSE;
}

/**
 * @brief 检查队列是否为满
 * @param queue 指向信号队列控制块的指针
 * @return SIGNALQUEUE_TRUE-满，SIGNALQUEUE_FALSE-非满
 */
SignalQueue_Bool signalQueue_IsFull(SignalQueue_t* queue)
{
    if (queue == NULL)
    {
        return SIGNALQUEUE_TRUE; // 参数无效视为满
    }
    return (queue->count == SIGNALQUEUE_MAX_CAPACITY) ? SIGNALQUEUE_TRUE : SIGNALQUEUE_FALSE;
}

/**
 * @brief 获取队列中当前信号数量
 * @param queue 指向信号队列控制块的指针
 * @return 当前信号数（0~SIGNALQUEUE_MAX_CAPACITY）
 */
uint8_t signalQueue_GetCount(SignalQueue_t* queue)
{
    if (queue == NULL)
    {
        return 0;
    }
    return queue->count;
}

/**
 * @brief 清空信号队列
 * @param queue 指向信号队列控制块的指针
 * @return SIGNALQUEUE_TRUE-成功，SIGNALQUEUE_FALSE-失败（参数无效）
 */
SignalQueue_Bool signalQueue_Clear(SignalQueue_t* queue)
{
    if (queue == NULL)
    {
        return SIGNALQUEUE_FALSE;
    }

    // 重置索引和计数即可（无需清空缓冲区）
    queue->readIdx = 0;
    queue->writeIdx = 0;
    queue->count = 0;

    return SIGNALQUEUE_TRUE;
}


