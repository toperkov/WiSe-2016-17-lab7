#include "Radio_nRF.h"
#include "Network_Ping.h"

RADIO::RADIO(){}
RADIO::~RADIO(){}


RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
RF24Network network(radio);

const short max_active_nodes = 10;            // Array of nodes we are aware of
uint16_t active_nodes[max_active_nodes];
short num_active_nodes = 0;



void RADIO::nRF_init(){
  SPI.begin();                                           // Bring up the RF network

  radio.begin();
  //radio.setDataRate( RF24_250KBPS );

  //radio.setChannel(111);
  //radio.setPALevel(RF24_PA_MAX);
  radio.setPALevel(RF24_PA_HIGH);
}

void RADIO::nRF_network_init(uint16_t this_node_){
  network.begin(/*channel*/ 100, /*node address*/ this_node_ );
}


bool RADIO::RF_send(uint16_t to, struct SensorData sensorData){
  RF24NetworkHeader header(/*to node*/ to, /*type*/ 'T' /*Time*/);
  Serial.println(sensorData.temp);
  Serial.println(sensorData.hum);
  Serial.println(sensorData.lightLevel);
  return network.write(header,&sensorData,sizeof(sensorData));
}

void RADIO::RF_receive(SensorData sensorDataRec, uint16_t this_nodes) {

  network.update();                                    // Pump the network regularly
  while ( network.available() )  {                     // Is there anything ready for us?
  RF24NetworkHeader header;                            // If so, take a look at it
  network.peek(header);
    switch (header.type){                              // Dispatch the message to the correct handler.
      case 'T': SensorData sensorDataRec;                                                                      // The 'T' message is just a ulong, containing the time
                network.read(header,&sensorDataRec,sizeof(sensorDataRec));
                printf_P(PSTR("---------------------------------\n\r"));
                Serial.println(sensorDataRec.temp);
                Serial.println(sensorDataRec.hum);
                Serial.println(sensorDataRec.lightLevel);
                /**
                 * Add a particular node to the current list of active nodes
                 */
                if ( header.from_node != this_nodes || header.from_node > 00 )   // If this message is from ourselves or the base, don't bother adding it to the active nodes.
                {

                  /**
                   * Add a particular node to the current list of active nodes
                   */
                  short i = num_active_nodes;                                    // Do we already know about this node?
                  while (i--)  {
                    if ( active_nodes[i] == header.from_node )
                        break;
                  }

                  if ( i == -1 && num_active_nodes < max_active_nodes ){         // If not, add it to the table
                      active_nodes[num_active_nodes++] = header.from_node;
                      printf_P(PSTR("%lu: APP Added 0%o to list of active nodes.\n\r"),millis(),header.from_node);
                  }
                }
                break;
      default:  printf_P(PSTR("*** WARNING *** Unknown message type %c\n\r"),header.type);
                network.read(header,0,0);
                break;
    };
  }
}

void RADIO::RF_powerUp() {
  radio.powerUp();
  delay(50);
}

void RADIO::RF_powerDown() {
  delay(50);
  radio.powerDown();
  delay(50);
}
