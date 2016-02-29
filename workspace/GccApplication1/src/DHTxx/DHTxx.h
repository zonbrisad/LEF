/**
 * DHTxx.h
 *
 *  Created on: 1 mar 2012
 *      Author: pmg
 */

#ifndef DHTXX_H_
#define DHTXX_H_

#include <stdint.h>

/*--------------------------------------------------------------*/
#define DHT_PORT  PORTH
#define DHT_IN    PINH
#define DHT_PIN   PH4
#define DHT_DDR   DDRH
/*--------------------------------------------------------------*/

#define DHT_OK            0
#define DHT_NO_RESPONS    1
#define DHT_CSUM_ERR      2   // checksum error

/*--------------------------------------------------------------*/


/**
 * Initialize DHTxx sensor.
 */
void dht_init();

/**
 * Read temperature and moist level from sensor.
 */
uint8_t dht_read(int16_t *temperature, int16_t *moisture);





#endif /* DHTXX_H_ */
