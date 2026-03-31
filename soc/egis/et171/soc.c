/*
 * Copyright (c) 2025 Egis Technology Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/types.h>
#include <zephyr/devicetree.h>

/* ================================================================== ET171 HAL
 * et171 core
 */
#include <et171_hal/et171.h>
#include <et171_hal/et171_hal_smu.h>
#include <et171_hal/et171_hal_otp.h>

static inline void disable_unused_device()
{
	const uint32_t unused_ip = 0
#if !DT_NODE_HAS_STATUS(DT_NODELABEL(usb), okay) || 1 /* Egis's usb driver will handle it by itself */
		| SMU_RST_USB2
#endif
#if !DT_NODE_HAS_STATUS(DT_NODELABEL(spis), okay)
		| SMU_RST_SPIS
#endif
#if !DT_NODE_HAS_STATUS(DT_NODELABEL(spi0), okay) && !CONFIG_XIP
		| SMU_RST_SPIM1
#endif
#if !DT_NODE_HAS_STATUS(DT_NODELABEL(spi1), okay)
		| SMU_RST_SPIM2
#endif
#if !DT_NODE_HAS_STATUS(DT_NODELABEL(spi2), okay)
		| SMU_RST_SPIM3
#endif
#if !DT_NODE_HAS_STATUS(DT_NODELABEL(rtc0), okay)
		| SMU_RST_RTC
#endif
#if !DT_NODE_HAS_STATUS(DT_NODELABEL(pit0), okay) && !DT_NODE_EXISTS(DT_NODELABEL(ext_wdt))
		| SMU_RST_PITPWM
#endif
#if !DT_NODE_HAS_STATUS(DT_NODELABEL(i2c0), okay) && 0 /* keep enable, i2c is using to workaround for WFI issue */
		| SMU_RST_I2C
#endif
#if !DT_NODE_HAS_STATUS(DT_NODELABEL(uart0), okay)
		| SMU_RST_UART
#endif
#if !DT_NODE_HAS_STATUS(DT_NODELABEL(gpio0), okay)
		| SMU_RST_GPIO
#endif
#if !DT_NODE_HAS_STATUS(DT_NODELABEL(wdt), okay)
		| SMU_RST_WDT
#endif
#if !CONFIG_HAS_EGIS_ET171_SXSMCRYPT
		| SMU_RST_CRYPTO
#endif
		;
	if (unused_ip)
		HAL_SMU_ResetLow(unused_ip);
}

static int soc_pre_init(void)
{
	/* Load Root clock triming value from OTP */
	HAL_OTP_GetRootClock();

	disable_unused_device();

	return 0;
}

SYS_INIT(soc_pre_init, PRE_KERNEL_1, 0);
