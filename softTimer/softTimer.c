#include "softtimer.h"

// 最大软定时器数量
#define SOFT_TIMER_MAX  8

// 全局定时器数组（静态，仅本文件可见）
static soft_timer_t g_timers[SOFT_TIMER_MAX];

/**
 * @brief  软定时器框架初始化
 * @note   需在系统初始化时调用，清空所有定时器状态
 */
void SoftTimer_Init(void)
{
    for (int i = 0; i < SOFT_TIMER_MAX; i++) {
        g_timers[i].state     = TIMER_UNUSED;
        g_timers[i].type      = TIMER_ONESHOT;
        g_timers[i].period_ms = 0;
        g_timers[i].remain_ms = 0;
        g_timers[i].callback  = NULL;
    }
}

/**
 * @brief  启动一个新的软定时器
 * @param  period_ms: 定时周期(ms)
 * @param  type: 定时器类型（TIMER_ONESHOT/TIMER_PERIODIC）
 * @param  cb: 超时回调函数（NULL则无回调）
 * @retval 成功返回定时器ID（0~SOFT_TIMER_MAX-1），失败返回-1（无空闲定时器）
 */
int SoftTimer_Start(uint32_t period_ms,
                    soft_timer_type_t type,
                    void (*cb)(void))
{
    if (period_ms == 0) {		    // 入参合法性检查：周期不能为0
        return -1;
    }

    for (int i = 0; i < SOFT_TIMER_MAX; i++) {
        if (g_timers[i].state == TIMER_UNUSED) {
            g_timers[i].state     = TIMER_RUNNING;
            g_timers[i].type      = type;
            g_timers[i].period_ms = period_ms;
            g_timers[i].remain_ms = period_ms;
            g_timers[i].callback  = cb;
            return i;
        }
    }
    return -1; // 无空闲定时器
}

/**
 * @brief  停止指定ID的定时器（释放资源，变为UNUSED状态）
 * @param  id: 定时器ID（由SoftTimer_Start返回）
 */
void SoftTimer_Stop(int id)
{
    if (id < 0 || id >= SOFT_TIMER_MAX) {
        return; // ID越界，直接返回
    }
    g_timers[id].state     = TIMER_UNUSED;
    g_timers[id].type      = TIMER_ONESHOT;
    g_timers[id].period_ms = 0;
    g_timers[id].remain_ms = 0;
    g_timers[id].callback  = NULL;
}

/**
 * @brief  暂停指定ID的定时器（保留剩余时间，暂停计数）
 * @param  id: 定时器ID
 */
void SoftTimer_Pause(int id)
{
    if (id < 0 || id >= SOFT_TIMER_MAX) {
        return;
    }
    if (g_timers[id].state == TIMER_RUNNING) {
        g_timers[id].state = TIMER_PAUSED;
    }
}

/**
 * @brief  恢复指定ID的定时器（从暂停状态继续计数）
 * @param  id: 定时器ID
 */
void SoftTimer_Resume(int id)
{
    if (id < 0 || id >= SOFT_TIMER_MAX) {
        return;
    }
    if (g_timers[id].state == TIMER_PAUSED) {
        g_timers[id].state = TIMER_RUNNING;
    }
}

/**
 * @brief  修改指定定时器的周期
 * @param  id: 定时器ID
 * @param  new_period_ms: 新的定时周期(ms)
 * @note   若定时器正在运行，剩余时间会立即更新为新周期
 */
void SoftTimer_ChangePeriod(int id, uint32_t new_period_ms)
{
    if (id < 0 || id >= SOFT_TIMER_MAX || new_period_ms == 0) {
        return; // 入参非法，直接返回
    }

    g_timers[id].period_ms = new_period_ms;

    // 如果正在运行，立即更新剩余时间
    if (g_timers[id].state == TIMER_RUNNING) {
        g_timers[id].remain_ms = new_period_ms;
    }
}

/**
 * @brief  1ms计时滴答函数（必须在定时器中断中调用）
 * @note   仅更新剩余时间，不执行回调，保证中断耗时最短
 */
void SoftTimer_Tick1ms(void)
{
    for (int i = 0; i < SOFT_TIMER_MAX; i++) {
        if (g_timers[i].state == TIMER_RUNNING) {
            if (g_timers[i].remain_ms > 0) {
                g_timers[i].remain_ms--;
                if (g_timers[i].remain_ms == 0) {
                    g_timers[i].state = TIMER_EXPIRED;
                }
            }
        }
    }
}

/**
 * @brief  定时器回调处理函数（必须在主循环中调用）
 * @note   检查超时的定时器，执行回调，并处理周期/单次逻辑
 */
void SoftTimer_Run(void)
{
    for (int i = 0; i < SOFT_TIMER_MAX; i++) {
        if (g_timers[i].state == TIMER_EXPIRED) {
            if (g_timers[i].callback != NULL) {
                g_timers[i].callback();
            }

            if (g_timers[i].type == TIMER_PERIODIC) {
                g_timers[i].remain_ms = g_timers[i].period_ms;
                g_timers[i].state     = TIMER_RUNNING;
            } else {
                SoftTimer_Stop(i);
            }
        }
    }
}

// 是否可以进入睡眠
// uint8_t SoftTimer_IsIdle(void)
// {
//     for (int i = 0; i < SOFT_TIMER_MAX; i++) {
//         if (g_timers[i].state == TIMER_RUNNING ||
//             g_timers[i].state == TIMER_EXPIRED) {
//             return 0;
//         }
//     }
//     return 1;
// }


// 获取最近一次到期时间
// uint32_t SoftTimer_GetNextTimeout(void)
// {
//     uint32_t min = 0xFFFFFFFF;

//     for (int i = 0; i < SOFT_TIMER_MAX; i++) {
//         if (g_timers[i].state == TIMER_RUNNING) {
//             if (g_timers[i].remain_ms < min) {
//                 min = g_timers[i].remain_ms;
//             }
//         }
//     }

//     return (min == 0xFFFFFFFF) ? 0 : min;
// }

