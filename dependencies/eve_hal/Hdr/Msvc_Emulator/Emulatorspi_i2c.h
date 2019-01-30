/**
 * SPII2CClass
 * $Id: 404f2321465c105752cfedee3564530c4d80fcdd $
 * \file ft800emu_spi_i2c.h
 * \brief SPII2CClass
 * \date 2013-06-21 21:56GMT
 * \author Jan Boon (Kaetemi)
 */

/*
 * Copyright (C) 2013  Future Technology Devices International Ltd
 */

#ifndef _EMULATOR_SPI_I2C_H
#define _EMULATOR_SPI_I2C_H
// #include <...>


// Project includes
#include "Platform.h"

namespace FT800EMU {

/**
 * SPII2CClass
 * \brief SPII2CClass
 * \date 2013-06-21 21:56GMT
 * \author Jan Boon (Kaetemi)
 */
class SPII2CClass
{
public:
	SPII2CClass() { }

	static void begin();
	static void end();

	static void csLow(bool low = true);
	static void csHigh(bool high = true);

	uint8_t transfer(uint8_t data);

	// Interrupt
	static bool intnLow();
	static bool intnHigh();

private:
	SPII2CClass(const SPII2CClass &);
	SPII2CClass &operator=(const SPII2CClass &);

}; /* class SPII2CClass */

extern SPII2CClass SPII2C;

} /* namespace FT800EMU */

#endif /* #ifndef FT800EMU_SPI_I2C_H */

/* end of file */
