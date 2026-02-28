#include "flash.h"

/**
 * 擦除 Flash 页
 */
FlashStatus_t Flash_ErasePage(uint32_t pageAddress) {
    // 越界检查
    if(pageAddress < FLASH_USER_START_ADDR || pageAddress > FLASH_USER_END_ADDR) {
        return FLASH_ERROR_PARAM;
    }

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef EraseInitStruct = {0};
    uint32_t PageError = 0;

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = pageAddress;
    EraseInitStruct.NbPages = 1;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
        HAL_FLASH_Lock();
        return FLASH_ERROR_ERASE;
    }

    HAL_FLASH_Lock();
    return FLASH_OK;
}

/**
 * 获取页起始地址
 */
uint32_t Flash_GetPageStart(uint32_t address) {
    return FLASH_USER_START_ADDR + ((address - FLASH_USER_START_ADDR) / FLASH_USER_PAGE_SIZE) * FLASH_USER_PAGE_SIZE;
}

/**
 * 写任意字节（自动跨页 + 可选写前比对）
 */
FlashStatus_t Flash_WriteBytes(uint32_t address, const uint8_t *data, uint32_t length) {
    if (!data || length == 0) return FLASH_ERROR_PARAM;

    // 越界检查
    if(address < FLASH_USER_START_ADDR || (address + length - 1) > FLASH_USER_END_ADDR) {
        return FLASH_ERROR_PARAM;
    }

    HAL_FLASH_Unlock();

    FlashStatus_t status = FLASH_OK;
    uint32_t addr = address;
    uint32_t endAddr = address + length;

    while (addr < endAddr) {
        uint16_t halfWord = 0xFFFF;
        uint32_t index = addr - address;

        if ((addr % 2 == 0) && ((endAddr - addr) >= 2)) {
            halfWord = data[index] | (data[index + 1] << 8);
        } else if ((addr % 2 == 0) && ((endAddr - addr) == 1)) {
            halfWord = data[index] | 0xFF00;
        } else {
            uint16_t prevHalf = *(uint16_t*)(addr - 1);
            halfWord = (prevHalf & 0x00FF) | (data[index] << 8);
            addr--;
        }

#if FLASH_ENABLE_CMPWRITE
        // 写前比对，如果数据相同则跳过写入
        if (*(uint16_t*)addr == halfWord) {
            addr += 2;
            continue;
        }
#endif

        // 如果当前半字不是 0xFFFF，说明未擦除，需要擦除页
        if (*(uint16_t*)addr != 0xFFFF) {
            uint32_t pageStart = Flash_GetPageStart(addr);
            status = Flash_ErasePage(pageStart);
            if (status != FLASH_OK) {
                HAL_FLASH_Lock();
                return status;
            }
        }

        // 写半字
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, halfWord) != HAL_OK) {
            HAL_FLASH_Lock();
            return FLASH_ERROR_PROGRAM;
        }

        addr += 2;
    }

    HAL_FLASH_Lock();
    return status;
}

/**
 * 读取字节数组
 */
FlashStatus_t Flash_ReadBytes(uint32_t address, uint8_t *buffer, uint32_t length) {
    if (!buffer || length == 0) return FLASH_ERROR_PARAM;

    // 越界检查
    if(address < FLASH_USER_START_ADDR || (address + length - 1) > FLASH_USER_END_ADDR) {
        return FLASH_ERROR_PARAM;
    }

    for (uint32_t i = 0; i < length; i++) {
        buffer[i] = *(uint8_t*)(address + i);
    }

    return FLASH_OK;
}

