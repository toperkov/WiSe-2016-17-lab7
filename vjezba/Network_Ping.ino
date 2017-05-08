/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example: Network topology, and pinging across a tree/mesh network
 *
 * Using this sketch, each node will send sensor data to every other node in the network every few seconds.
 * The RF24Network library will route the message across the mesh to the correct node.
 *
 * To see the underlying frames being relayed, compile RF24Network with
 * #define SERIAL_DEBUG.
 *
 * Update: The logical node address of each node is set below, and are grouped in twos for demonstration.
 * Number 0 is the master node. Numbers 1-2 represent the 2nd layer in the tree (02,05).
 * Number 3 (012) is the first child of number 1 (02). Number 4 (015) is the first child of number 2.
 * Below that are children 5 (022) and 6 (025), and so on as shown below
 * The tree below represents the possible network topology with the addresses defined lower down
 *
 *     Addresses/Topology                            Node Numbers  (To simplify address assignment in this demonstration)
 *             00                  - Master Node         ( 0 )
 *           02  05                - 1st Level children ( 1,2 )
 *    32 22 12    15 25 35 45    - 2nd Level children (7,5,3-4,6,8)
 *
 * eg:
 * For node 4 (Address 015) to contact node 1 (address 02), it will send through node 2 (address 05) which relays the payload
 * through the master (00), which sends it through to node 1 (02). This seems complicated, however, node 4 (015) can be a very
 * long way away from node 1 (02), with node 2 (05) bridging the gap between it and the master node.
 *
 * To use the sketch, upload it to two or more units and set the NODE_ADDRESS below. If configuring only a few
 * units, set the addresses to 0,1,3,5... to configure all nodes as children to each other. If using many nodes,
 * it is easiest just to increment the NODE_ADDRESS by 1 as the sketch is uploaded to each device.
 */

#include "printf.h"
#include "Sensors.h"
#include "Network_Ping.h"
#include "Radio_nRF.h"

// These are the Octal addresses that will be assigned
const uint16_t node_address_set[10] = { 00, 02, 05, 012, 015, 022, 025, 032, 035, 045 };

// 0 = Master
// 1-2 (02,05)   = Children of Master(00)
// 3,5 (012,022) = Children of (02)
// 4,6 (015,025) = Children of (05)
// 7   (032)     = Child of (02)
// 8,9 (035,045) = Children of (05)

uint16_t this_node;                           // Our node address
uint8_t NODE_ADDRESS = 3;  // Use numbers 0 through to select an address from the array


/***********************************************************************/
/***********************************************************************/

unsigned long last_time_sent;
const unsigned long interval = 1000; // ms       // Delay manager to send pings regularly.

/***********************************************************************/
/***********************************************************************/

bool send_T(uint16_t to);                      // Prototypes for functions to send & handle messages

SENSORS sensor;

RADIO radioNRF;

/*----------- Radio variables -----------*/
// Payload that the child sends to a parent
SensorData sensorData;

void setup(){

  Serial.begin(115200);

  this_node = node_address_set[NODE_ADDRESS];            // Which node are we?

  sensor.DHT_init();
  sensor.BH1750_init();
  radioNRF.nRF_init();
  radioNRF.nRF_network_init(this_node);

  printf_begin();
  printf_P(PSTR("\n\rRF24Network/examples/meshping/\n\r"));

}

void loop(){

  State state = GOTO_SLEEP;
  for(;;) {
    switch(state) {
      case READ_SERIAL:
        state = GOTO_SLEEP;
        break;
      case GOTO_SLEEP:
        //state = RECEIVING_RADIO;
        state = READ_SENSORS;
        break;
      case READ_SENSORS:
        sensorData.temp = sensor.readTemp();
        sensorData.hum = sensor.readHum();
        sensorData.lightLevel = sensor.readLight();
        delay(100); // give some time to send data over Serial
        state = SENDING_RADIO;
        break;
      case SENDING_RADIO:
        if ( millis() - last_time_sent >= interval ){                     // Send sensor data to the next node every 'interval' ms
          last_time_sent = millis();
          if ( this_node > 00){
            if (send_T(00)){                                              // Notify us of the result
                printf_P(PSTR("%lu: APP Send ok\n\r"),millis());
            }else{
                printf_P(PSTR("%lu: APP Send failed\n\r"),millis());
                last_time_sent -= 100;                                    // Try sending at a different time next time due to possible collisions
            }
          }
        }
        state = RECEIVING_RADIO;
        break;
      case RECEIVING_RADIO:
        SensorData sensorDataRec;
        radioNRF.RF_receive(sensorDataRec, this_node);
        state = READ_SERIAL;
        break;
    }
  }

}

/**
 * Send a message containing sensor data
 */
bool send_T(uint16_t to)
{
  return radioNRF.RF_send(to, sensorData);
}
