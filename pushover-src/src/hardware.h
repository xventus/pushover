//
// vim: ts=4 et
// Copyright (c) 2024 Petr Vanek, petr@fotoventus.cz
//
/// @file   hardware.h
/// @author Petr Vanek

#pragma once

#include "driver/gpio.h"
#include "driver/uart.h"

/**
 * @brief Hardware definition, pin assignment, communication interfaces settings
 * 
 */


// LEDs
#define  HEART_BEAT_LED  GPIO_NUM_5

// Buttons
#define  FLASH_BUTTON    GPIO_NUM_0
#define  A_BUTTON        GPIO_NUM_2



