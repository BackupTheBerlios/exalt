/** @file default.c */
#include "default.h"

/**
 * @addgroup Exalt_save_load
 *@{
 */


/**
 * @brief save the configuration of one interface in the file distribution (interfaces, net ...)
 * @param eth the interface
 * @return Return 1 if success, else -1
 */
int exalt_eth_save_byeth(exalt_ethernet* eth)
{
 	printf("Configuration save is not yet implemented for your distribution\n");
	return -1;
}

/**
 * @brief load the configuration for one interface
 * @param eth the interface
 * @return Return 1 if success, else -1
 */
int exalt_eth_save_load_byeth(exalt_ethernet* eth)
{
 	printf("Configuration load is not yet implemented for your distribution\n");
	return -1;
}

/**
 * @brief save if the interface is load (or not) when the system start
 * @param eth the interface
 * @return Return 1 if sucess, else -1
 */
int exalt_eth_save_autoload(exalt_ethernet* eth)
{
 	printf("Configuration save is not yet implemented for your distribution\n");
	return -1;
}

/** @} */

