/*
 * Copyright (c) 2024 iSoftStone Education Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "iot_errno.h"
#include "iot_spi.h"
#include "lz_hardware.h"

#define SPI_MAX_SPEED 50000000

struct SpiBusInfo {
  unsigned int id;
  SpiBusIo spi_bus;
};

static struct SpiBusInfo m_spi_bus_info[ESPIDEV_MAX] = 
{
    [ESPI1_M1] = {
        .id = 1,
        .spi_bus = {
            .cs =   {.gpio = GPIO0_PB0, .func = MUX_FUNC4, .type = PULL_UP, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
            .clk =  {.gpio = GPIO0_PB1, .func = MUX_FUNC4, .type = PULL_UP, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
            .mosi = {.gpio = GPIO0_PB2, .func = MUX_FUNC4, .type = PULL_UP, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
            .miso = {.gpio = GPIO0_PB3, .func = MUX_FUNC4, .type = PULL_UP, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
            .id = FUNC_ID_SPI1,
            .mode = FUNC_MODE_M1,
        },
    },
    [ESPI0_M0] = {
        .id = 0,
        .spi_bus = {
            .cs =   {.gpio = GPIO0_PB4, .func = MUX_FUNC3, .type = PULL_UP, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
            .clk =  {.gpio = GPIO0_PB5, .func = MUX_FUNC3, .type = PULL_UP, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
            .mosi = {.gpio = GPIO0_PB6, .func = MUX_FUNC3, .type = PULL_UP, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
            .miso = {.gpio = GPIO0_PB7, .func = MUX_FUNC3, .type = PULL_UP, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
            .id = FUNC_ID_SPI0,
            .mode = FUNC_MODE_M0,
        },
    },
    [ESPI0_M1] = {
        .id = 0,
        .spi_bus = {
            .cs =   {.gpio = GPIO0_PC0, .func = MUX_FUNC4, .type = PULL_UP, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
            .clk =  {.gpio = GPIO0_PC1, .func = MUX_FUNC4, .type = PULL_UP, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
            .mosi = {.gpio = GPIO0_PC2, .func = MUX_FUNC4, .type = PULL_UP, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
            .miso = {.gpio = GPIO0_PC3, .func = MUX_FUNC4, .type = PULL_UP, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
            .id = FUNC_ID_SPI0,
            .mode = FUNC_MODE_M1,
        },
    },
    [ESPI1_M0] = {
        .id = 1,
        .spi_bus = {
            .cs =   {.gpio = GPIO0_PC4, .func = MUX_FUNC4, .type = PULL_UP, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
            .clk =  {.gpio = GPIO0_PC5, .func = MUX_FUNC4, .type = PULL_UP, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
            .mosi = {.gpio = GPIO0_PC6, .func = MUX_FUNC4, .type = PULL_UP, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
            .miso = {.gpio = GPIO0_PC7, .func = MUX_FUNC4, .type = PULL_UP, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
            .id = FUNC_ID_SPI1,
            .mode = FUNC_MODE_M0,
        },
    },
};

unsigned int IoTSpiInit(unsigned int id, IoT_SPI_InitTypeDef *iot_spi)
{
    unsigned int ret = 0;
    LzSpiConfig spi_config = {0};

    if (id >= ESPIDEV_MAX) {
        PRINT_ERR("id(%d) >= ESPIDEV_MAX(%d)\n", id, ESPIDEV_MAX);
        return IOT_FAILURE;
    }    

    if (iot_spi->Mode == SPI_MODE_SLAVE) 
    {
        spi_config.isSlave = false;
    }
    else if (iot_spi->Mode == SPI_MODE_SLAVE)
    {
        spi_config.isSlave = true;
    }

    if (iot_spi->Direction == SPI_DIRECTION_1LINE_TX)
    {
        m_spi_bus_info[id].spi_bus.miso.gpio = INVALID_GPIO;
    }

    if (iot_spi->DataSize == SPI_DATASIZE_8BIT)
    {
        spi_config.bitsPerWord = SPI_PERWORD_8BITS;
    }
    else if (iot_spi->DataSize == SPI_DATASIZE_16BIT)
    {
        spi_config.bitsPerWord = SPI_PERWORD_16BITS;
    }

    if (iot_spi->CLKPolarity == SPI_POLARITY_LOW)
    {
        if (iot_spi->CLKPhase == SPI_PHASE_1EDGE)
        {
            spi_config.mode = SPI_MODE_0;
        }
        else if (iot_spi->CLKPhase == SPI_PHASE_2EDGE)
        {
            spi_config.mode = SPI_MODE_2;
        }
    }
    else if (iot_spi->CLKPolarity == SPI_POLARITY_HIGH)
    {
        if (iot_spi->CLKPhase == SPI_PHASE_1EDGE)
        {
            spi_config.mode = SPI_MODE_1;
        }
        else if (iot_spi->CLKPhase == SPI_PHASE_2EDGE)
        {
            spi_config.mode = SPI_MODE_3;
        }        
    }

    if (iot_spi->BaudRatePrescaler == SPI_BAUDRATEPRESCALER_1)
    {
        spi_config.speed = SPI_MAX_SPEED;
    }
    else if (iot_spi->BaudRatePrescaler == SPI_BAUDRATEPRESCALER_2)
    {
        spi_config.speed = SPI_MAX_SPEED / 2;
    }
    else if (iot_spi->BaudRatePrescaler == SPI_BAUDRATEPRESCALER_4)
    {
        spi_config.speed = SPI_MAX_SPEED / 4;
    }

    if (iot_spi->FirstBit == SPI_FIRSTBIT_MSB)
    {
        spi_config.firstBit = SPI_MSB;
    }
    else if (iot_spi->FirstBit == SPI_FIRSTBIT_LSB)
    {
        spi_config.firstBit = SPI_LSB;
    }

    spi_config.csm = SPI_CMS_ONE_CYCLES;

    if (SpiIoInit(m_spi_bus_info[id].spi_bus) != LZ_HARDWARE_SUCCESS) {
        printf("%s, %d: SpiIoInit failed!\n", __FILE__, __LINE__);
        return IOT_FAILURE;
    }

    if (LzSpiInit(m_spi_bus_info[id].id, spi_config) != LZ_HARDWARE_SUCCESS) {
        printf("%s, %d: LzSpiInit failed!\n", __FILE__, __LINE__);
        return IOT_FAILURE;
    }

    return IOT_SUCCESS;
}

unsigned int IoTSpiDeinit(unsigned int id)
{
    unsigned int ret = 0;

    if (id >= ESPIDEV_MAX) {
        PRINT_ERR("id(%d) >= ESPIDEV_MAX(%d)\n", id, ESPIDEV_MAX);
        return IOT_FAILURE;
    }

    ret = LzSpiDeinit(id);
    if (ret != LZ_HARDWARE_SUCCESS) 
    {
        return IOT_FAILURE;
    }

    return IOT_SUCCESS;
}

unsigned int IoTSpiWrite(unsigned int id, unsigned char *buf, unsigned int len)
{
    unsigned int ret = 0;

    if (id >= ESPIDEV_MAX) {
        PRINT_ERR("id(%d) >= ESPIDEV_MAX(%d)\n", id, ESPIDEV_MAX);
        return IOT_FAILURE;
    } 

    ret = LzSpiWrite(m_spi_bus_info[id].id, m_spi_bus_info[id].id, buf, len);
    if (ret != LZ_HARDWARE_SUCCESS) 
    {
        return IOT_FAILURE;
    }

    return IOT_SUCCESS;
}

unsigned int IoTSpiRead(unsigned int id, unsigned char *buf, unsigned int len)
{
    unsigned int ret = 0;

    if (id >= ESPIDEV_MAX) {
        PRINT_ERR("id(%d) >= ESPIDEV_MAX(%d)\n", id, ESPIDEV_MAX);
        return IOT_FAILURE;
    } 

    ret = LzSpiRead(m_spi_bus_info[id].id, m_spi_bus_info[id].id, buf, len);
    if (ret != LZ_HARDWARE_SUCCESS) 
    {
        return IOT_FAILURE;
    }

    return IOT_SUCCESS;    
}
