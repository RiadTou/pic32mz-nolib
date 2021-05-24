#ifndef PLC_H
#define PLC_H

/**
 * @brief initialize PLC config, 
 *
 * @return the tick in millisecond 
 */
long int plc_init(void);

/**
 * @brief must be called every tick, set by the st file config
 * performs the ladder diagram
 *
 */

void plc_run(void);

/**
 * @brief visulations routine, to print debug messages
 *
 */

void get_current_time(void);
#endif /* PLC_H */