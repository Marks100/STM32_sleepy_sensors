/*! \file
*               Author: mstewart
*   \brief      RNG_MGR: Random Number Generator module
*/
/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "STDC.h"
#include "C_defs.h"
#include "RTC.h"
#include "RNG_MGR.h"



/***************************************************************************************************
**                              Data declarations and definitions                                 **
***************************************************************************************************/
/* None */
STATIC u32_t RNG_MGR_seed_s;
 

/***************************************************************************************************
**                              Public Functions                                                  **
***************************************************************************************************/
/*!
****************************************************************************************************
*
*   \brief         Module (re-)initialisation function
*
*   \author        MS
*
*   \return        none
*
*   \note
*
***************************************************************************************************/
void RNG_MGR_init( void )
{
	RNG_MGR_seed_s = 0u;
}



/*!
****************************************************************************************************
*
*   \brief         Generate a pseudo random number
*
*   \author        MS
*
*   \return        u8_t pseudo random number
*
*   \note
*
***************************************************************************************************/
u8_t RNG_MGR_gen_random_number( void )
{
	u8_t  random_number;

	/* Update the seed */
	RNG_MGR_update_seed();

    /* Now Set the seed for srand */
	srand( RNG_MGR_seed_s );

    /* Grab the now "random :)" number and limit the values to between 0 and 255 ( 1 byte ) */
    random_number = ( rand()%U8_T_MAX );

	return ( random_number );
}



/*!
****************************************************************************************************
*
*   \brief         Generate a seed
*
*   \author        MS
*
*   \return        u8_t seed
*
*   \note
*
***************************************************************************************************/
void RNG_MGR_update_seed( void )
{
	/* We need to create a "random" value here, and in order to do that we "seed" the rand function with
	   an instantaneous time value comprised of secs, mins, hours and even days */
	RNG_MGR_seed_s = RTC_get_current_running_time_secs();

	/* This means that "seed" is now a 32bit number but the max value that srand can accept is 32768,
	   so lets mod the "seed" with 32768 to give us a remainder that can be fed in to create a seed  */
	RNG_MGR_seed_s = ( RNG_MGR_seed_s % 32768u );
}







/***************************************************************************************************
**                              Private Functions                                                 **
***************************************************************************************************/


/****************************** END OF FILE *******************************************************/
