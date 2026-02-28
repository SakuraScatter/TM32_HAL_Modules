#ifndef __SOFTTIMER_H__
#define __SOFTTIMER_H__

#include <stdio.h>
#include <stdint.h>

typedef enum {
    TIMER_UNUSED = 0,
    TIMER_RUNNING,
    TIMER_PAUSED,
    TIMER_EXPIRED
} soft_timer_state_t;

typedef enum {
    TIMER_ONESHOT = 0,
    TIMER_PERIODIC
} soft_timer_type_t;

typedef struct {
    soft_timer_state_t state;
    soft_timer_type_t  type;

    uint32_t period_ms;     // 周期
    uint32_t remain_ms;     // 剩余时间

    void (*callback)(void);
} soft_timer_t;



void SoftTimer_Init(void);

int  SoftTimer_Start(uint32_t period_ms,
                     soft_timer_type_t type,
                     void (*cb)(void));

void SoftTimer_Stop(int id);
void SoftTimer_Pause(int id);
void SoftTimer_Resume(int id);

void SoftTimer_ChangePeriod(int id, uint32_t new_period_ms);

void SoftTimer_Tick1ms(void);				// 中断中调用
void SoftTimer_Run(void);						// 主循环中调用

// uint8_t SoftTimer_IsIdle(void);   // 低功耗判断
// uint32_t SoftTimer_GetNextTimeout(void);



#endif

