#ifndef MAIN_H
#define MAIN_H

#include "c_defs.h"
#include "compiler_defs.h"

#define BOOTLOADER_BACKUP_REG    ( BKP_DR1 )
#define BOOTLOADER_NOT_REQUESTED ( 0u )
#define BOOTLOADER_REQUESTED     ( 1u )

void JumpToBootloader(void);
void Bootloader_check( void );

#endif
