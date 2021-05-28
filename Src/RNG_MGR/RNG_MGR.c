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
#include "UID.h"
#include "RNG_MGR.h"


/***************************************************************************************************
**                              Data declarations and definitions                                 **
***************************************************************************************************/
STATIC u16_t RNG_MGR_seed_s;
 

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
u8_t RNG_MGR_gen_random_number_u8( void )
{
	u8_t random_number;

	/* Update the seed */
	RNG_MGR_update_seed();

    /* Now Set the seed for srand */
	srand( (u16_t)RNG_MGR_seed_s );

    /* Grab the now "random :)" number */
    random_number = ( rand()%U8_T_MAX );

	return ( random_number );
}

/*!
****************************************************************************************************
*
*   \brief         Generate a pseudo random number
*
*   \author        MS
*
*   \return        u16_t pseudo random number
*
*   \note
*
***************************************************************************************************/
u16_t RNG_MGR_gen_random_number_u16( void )
{
	u16_t random_number;

	/* Update the seed */
	RNG_MGR_update_seed();

    /* Now Set the seed for srand */
	srand( (u16_t)RNG_MGR_seed_s );

    /* Grab the now "random :)" number */
    random_number = ( rand()%U16_T_MAX );

	return ( random_number );
}


/*!
****************************************************************************************************
*
*   \brief         Generate a pseudo random number
*
*   \author        MS
*
*   \return        u32_t pseudo random number
*
*   \note
*
***************************************************************************************************/
u32_t RNG_MGR_gen_random_number_u32( void )
{
	u32_t random_number;

	/* Update the seed */
	RNG_MGR_update_seed();

    /* Now Set the seed for srand */
	srand( (u16_t)RNG_MGR_seed_s );

    /* Grab the now "random :)" number */
    random_number = ( rand()%U32_T_MAX );

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
	   part of the stm32 uid */
	RNG_MGR_seed_s = UID_get_unique_id_16();
}



/***************************************************************************************************
**                              Private Functions                                                 **
***************************************************************************************************/


/****************************** END OF FILE *******************************************************/
