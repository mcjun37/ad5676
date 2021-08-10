/*
 * Copyright 2021 Minchul Jun (mcjun37@naver.com).  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY MINCHUL JUN ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL MINCHUL JUN OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of Minchul Jun.
 *
 */

#include "AD5676R.h"
#include "cmsis_os.h"
#include "stdlib.h"

_dacConf* dac1Conf = NULL;

HAL_StatusTypeDef AD5676R_init(_dacType dacType, SPI_HandleTypeDef* hspi)
{
    HAL_StatusTypeDef result = HAL_OK;
    _dacConf* dacConf;

    if(dacType == AD5676R_DAC1)
    {
        dac1Conf = (_dacConf*)malloc(sizeof(_dacConf));
        dac1Conf->type = dacType;
        dac1Conf->hspi = hspi;
        dac1Conf->nClr.port     = SPI1_NCLR_GPIO_Port;
        dac1Conf->nClr.pin      = SPI1_NCLR_Pin;
        dac1Conf->nLdac.port    = SPI1_NLDAC_GPIO_Port;
        dac1Conf->nLdac.pin     = SPI1_NLDAC_Pin;
        dac1Conf->sync.port     = SPI1_SYNC_GPIO_Port;
        dac1Conf->sync.pin      = SPI1_SYNC_Pin;

        dacConf = dac1Conf;
    }
    else
    {
        result = HAL_ERROR;
        return result;
    }

    // Reset Sequence
    HAL_GPIO_WritePin(dacConf->nClr.port, dacConf->nClr.pin, GPIO_PIN_RESET);
    // Keep always low, to update output simultaneously
    HAL_GPIO_WritePin(dacConf->nLdac.port, dacConf->nLdac.pin, GPIO_PIN_RESET);
    // Set low during send data
    HAL_GPIO_WritePin(dacConf->sync.port, dacConf->sync.pin, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(dacConf->nClr.port, dacConf->nClr.pin, GPIO_PIN_SET);

    return result;
}

static HAL_StatusTypeDef AD5676R_write(_dacType dacType, uint8_t cmd, uint8_t addr, uint16_t val)
{
    HAL_StatusTypeDef result = HAL_OK;
    uint8_t txLen = 3;
    uint8_t sendBuf[3];
    _dacConf* dacConf;

    sendBuf[0] = (AD5676R_CMD(cmd) | AD5676R_ADDR(addr));
    sendBuf[1] = (uint8_t)AD5676R_VAL_H(val);
    sendBuf[2] = (uint8_t)AD5676R_VAL_L(val);

    if(dacType == AD5676R_DAC1)
    {
        dacConf = dac1Conf;
    }
    else
    {
        result = HAL_ERROR;
        return result;
    }

    // Check availablity to send data
    while(GPIO_PIN_RESET == HAL_GPIO_ReadPin(dacConf->sync.port, dacConf->sync.pin))
    {}

    HAL_GPIO_WritePin(dacConf->sync.port, dacConf->sync.pin, GPIO_PIN_RESET);
    result = HAL_SPI_Transmit_DMA(dacConf->hspi, sendBuf, txLen);

    if(HAL_OK != result)
    {
        HAL_GPIO_WritePin(dacConf->sync.port, dacConf->sync.pin, GPIO_PIN_SET);
    }
    else
    {
        // DO NOTHING
    }
    return result;
}

HAL_StatusTypeDef AD5676R_output_ch_mVolt(_dacType dacType, uint8_t ch, float mVolt)
{
    HAL_StatusTypeDef result = HAL_OK;

    const float dacPer1mv = (float)65535.0 / (float)5000.0; // 0xFFFF (16bit)
    uint8_t cmd;
    uint8_t addr;
    uint16_t val;

    cmd = AD5676R_CMD_WRITE_INPUT_N;
    addr = ch;
    val = (uint16_t)(mVolt * dacPer1mv);

    result = AD5676R_write(dacType, cmd, addr, val);

    return result;
}