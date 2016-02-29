/**
 * DHTxx.c
 *
 *  Created on: 1 mar 2012
 *      Author: pmg
 */

#include <stdio.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/pgmspace.h>
#include "DHTxx.h"

#define DHT_SET_HIGH()        DHT_PORT |= (1<<DHT_PIN)
#define DHT_SET_LOW()         DHT_PORT  &= ~(1<<DHT_PIN)
#define DHT_SET_AS_INPUT()    DHT_DDR &= ~(1<<DHT_PIN)
#define DHT_SET_AS_OUTPUT()   DHT_DDR |= (1<<DHT_PIN)

#define DHT_READ()            DHT_IN & (1<<DHT_PIN)

void dht_init() {
//  DHT_DDR  |= (1<<DHT_PIN);     // set as output port
//  DHT_PORT |= (1<<DHT_PIN);     // set pin high

  DHT_SET_AS_OUTPUT();      // set as output port
  DHT_SET_HIGH();           // set pin high
}

uint8_t dht_readByte() {
  uint8_t i       = 0;
  uint8_t result  = 0;

  for (i=0;i<8;i++) {
    // wait forever until port is one
    // @todo do not wait for ever
    while(!(DHT_IN & (1<<DHT_PIN))) {
    }
    _delay_us(30);
    if (DHT_IN & (1<<DHT_PIN))
      result |= (1<<(7-i));

    while((DHT_IN & (1<<DHT_PIN))) {
    }
  }
  return result;
}

uint8_t dht_read(int16_t *temperature, int16_t *moisture) {
  uint8_t in, i;
  uint8_t buf[5];
  uint8_t checkSum;

  DHT_SET_LOW();      // pull pin low and wait 18 ms
  _delay_ms(18);
  DHT_SET_HIGH();     // pull high and wait 40 us
  _delay_us(1);
  DHT_SET_AS_INPUT();
  _delay_us(40);

  in = DHT_READ();    // read input

  if (in) {           // check for first response signal LOW
    //printf_P(PSTR("DHT: No response signal 1\n"));
    return DHT_NO_RESPONS;
  }

  _delay_us(80);

  in = DHT_READ();      // read input
  if (!in) {            // check for second response signal HIGH
    //printf_P(PSTR("DHT: No response signal 2\n"));
    return DHT_NO_RESPONS;
  }

  _delay_us(80);

  // now ready for datareceive

  for(i=0;i<5;i++) {
    buf[i] = dht_readByte();
  }

  checkSum = buf[0] + buf[1] + buf[2] + buf[3];

  //printf_P(PSTR("DHT data %2x %2x %2x %2x %2x  checksum=%x\n"),buf[0],buf[1],buf[2],buf[3],buf[4], checkSum);

  DHT_SET_AS_INPUT();
  DHT_SET_HIGH();

  if (checkSum != buf[4])   // check checksum
    return DHT_CSUM_ERR;

  *moisture     = 100*buf[0] + buf[1];
  *temperature  = 100*buf[2] + buf[3];
  return DHT_OK;
}





