#ifndef Radio_nRF_h
#define Radio_nRF_h

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24Network.h>
#include <RF24.h>


#define CE_PIN   9
#define CSN_PIN 10


class RADIO {
public:
        RADIO();
        ~RADIO();
        void nRF_init();
        void nRF_network_init(uint16_t);
        bool RF_send(uint16_t, struct SensorData sensorData);
        void RF_receive(struct SensorData sensorDataRec, uint16_t);
        void RF_powerUp();
        void RF_powerDown();
};

#endif
