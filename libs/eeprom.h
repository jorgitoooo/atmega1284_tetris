#ifndef __EEPROM_H__
#define __EEPROM_H__

typedef unsigned int ui;
typedef unsigned char uc;

void EEPROM_write(unsigned int uiAddress, uc ucData)
{
	while(EECR & (1 << EEPE));
	
	EEAR = uiAddress;     // Set up address register
	EEDR = ucData;        // Set up data register
	EECR |= (1 << EEMPE); // Write logical 1 to EEMPE
	EECR |= (1 << EEPE);  // Start EEPROM write by setting EEPE
}
uc EEPROM_read(unsigned int uiAddress)
{
	while(EECR & (1 << EEPE));
	
	EEAR = uiAddress;
	EECR |= (1 << EERE);
	return EEDR;
}

#endif
