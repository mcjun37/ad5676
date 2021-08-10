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

#ifndef __AD5676R_HEADER
#define __AD5676R_HEADER

#include "main.h"

#define AD5676R_ADDR(x)                             ((x) & 0x0F)
#define AD5676R_CMD(x)                              (((x) << 4) & 0xF0)
#define AD5676R_VAL_H(x)                            (((x) >> 8) & 0x00FF)
#define AD5676R_VAL_L(x)                            ((x) & 0x00FF)

#define AD5676R_DAC_ADDR(ch)                        (0x1 << (ch))
#define AD5676R_DAC_ADDR_ALL                        0xF

#define AD5676R_CMD_NOOP                            0x0
#define AD5676R_CMD_WRITE_INPUT_N                   0x1
#define AD5676R_CMD_UPDATE_DAC_N                    0x2
#define AD5676R_CMD_WRITE_INPUT_N_UPDATE_N          0x3
#define AD5676R_CMD_POWERDOWN_DAC                   0x4
#define AD5676R_CMD_LDAC_MASK                       0x5
#define AD5676R_CMD_SOFT_RESET                      0x6
#define AD5676R_CMD_INTERNAL_REFER_SETUP            0x7
#define AD5676R_CMD_DAISYCHAIN_ENABLE               0x8
#define AD5676R_CMD_READBACK_ENABLE                 0x9
#define AD5676R_CMD_UPDATE_ALLCH_IREG_WITH_INPUT    0xA
#define AD5676R_CMD_UPDATE_ALLCH_DACREG_WITH_INPUT  0xB

typedef enum
{
    AD5676R_DAC1
}_dacType;

typedef struct
{
    GPIO_TypeDef* port;
    uint16_t pin;
}_AD5676R_io;

typedef struct
{
    _dacType type;
    SPI_HandleTypeDef *hspi;
    _AD5676R_io nClr;
    _AD5676R_io nLdac;
    _AD5676R_io sync;
}_dacConf;

extern HAL_StatusTypeDef AD5676R_init(_dacType dacType, SPI_HandleTypeDef* hspi);
extern HAL_StatusTypeDef AD5676R_output_ch_mVolt(_dacType dacType, uint8_t ch, float mVolt);

#endif  // __AD5676R_HEADER