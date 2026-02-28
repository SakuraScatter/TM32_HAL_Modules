#ifndef __FLASH_H__
#define __FLASH_H__

#include "stm32f1xx_hal.h"
#include <stdint.h>

/**
 * STM32F103 Flash 操作库（安全版）
 * 文件名: flash.h / flash.c
 *
 * 功能：
 * - 任意长度字节写入
 * - 自动半字对齐
 * - 自动跨页擦除
 * - 可选写前比对减少擦写次数
 * - 防止越界
 */

//---------------------- 配置宏 ----------------------
#define FLASH_USER_START_ADDR      0x0800FC00   // 用户 Flash 起始地址
#define FLASH_USER_END_ADDR        0x0800FFFF   // 用户 Flash 结束地址（根据芯片 Flash 大小设置）
#define FLASH_USER_PAGE_SIZE       1024         // 用户页大小（C8T6 1KB, RB 2KB）
#define FLASH_ENABLE_CMPWRITE      1            // 1 开启写前比对，0 关闭
//------------------------------------------------------

typedef enum {
    FLASH_OK = 0,
    FLASH_ERROR_ERASE,
    FLASH_ERROR_PROGRAM,
    FLASH_ERROR_PARAM  // 参数错误或越界
} FlashStatus_t;

/**
 * 擦除 Flash 页
 * @param pageAddress 页起始地址
 */
FlashStatus_t Flash_ErasePage(uint32_t pageAddress);

/**
 * 写任意字节到 Flash（自动跨页 + 可选写前比对）
 * @param address Flash 起始地址
 * @param data 待写入数据
 * @param length 数据长度（字节）
 */
FlashStatus_t Flash_WriteBytes(uint32_t address, const uint8_t *data, uint32_t length);

/**
 * 读取字节数组
 * @param address Flash 起始地址
 * @param buffer 存放读取数据
 * @param length 字节长度
 */
FlashStatus_t Flash_ReadBytes(uint32_t address, uint8_t *buffer, uint32_t length);

/**
 * 获取页起始地址
 */
uint32_t Flash_GetPageStart(uint32_t address);

#endif /* __FLASH_H__ */

