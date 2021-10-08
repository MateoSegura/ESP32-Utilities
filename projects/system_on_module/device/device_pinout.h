#pragma once
/*
* Company: ANZE Suspension
* File Name: main.cpp
* Project: ESP32 System on Module
* Version: 1.0
* Compartible Hardware: REV1.0
* Date Created: September 23, 2021
* Last Modified: September 23, 2021
*/

//*********************************************************     READ ME    **********************************************************/

//*     All settings related to the esp hardware go here (buses speeds, cpu speed, etc)

//***********************************************************************************************************************************/

//********************************************************     APP GPIO     *********************************************************/

//* VSPI
#define VSPI_MOSI_PIN 23
#define VSPI_MISO_PIN 19
#define VSPI_CLCK_PIN 18

//* HSPI
#define HSPI_MOSI_PIN 25
#define HSPI_MISO_PIN 17
#define HSPI_CLCK_PIN 16

//* eMMC Memory
#define eMMC0_EN_PIN -1
#define eMMC0_CD_PIN 13

//* CAN bus
#define CAN0_CONTROLLER_CS_PIN 27
#define CAN0_CONTROLLER_INT_PIN 35
//* End.