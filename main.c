//
// Created by huku on 12.11.2020.
//


#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <malloc.h>

#define SOF 73

enum packet_receive_error {
    OK = 0, NO_SOF = 1, CRC_INCORRECT = 2
};

uint8_t gencrc(const uint8_t *data, size_t len) {
    uint8_t crc = 0xff;
    size_t i, j;
    for (i = 0; i < len; i++) {
        crc ^= data[i];
        for (j = 0; j < 8; j++) {
            if ((crc & (uint8_t)0x80) != 0) {
                crc = (uint8_t) ((uint8_t)(uint8_t)(uint8_t)(uint8_t)(uint8_t)(uint8_t)(uint8_t) crc << (uint8_t) 1 ^ (uint8_t) 0x31);
            } else {
                crc <<= (uint8_t)1;
            }
        }
    }
    return crc;
}

struct __attribute__((packed)) packet {
    uint8_t type;
    uint16_t length;
    uint8_t *payload;
    uint8_t crc8;
};

int check_crc(const struct packet *Packet) {
    uint8_t crc = gencrc((const uint8_t *) &Packet->type, Packet->type + 3);
    return Packet->crc8 == crc;
}

uint8_t receive_byte() {
    //stub
    return 0;
}

void send_byte() {
    //stub
}

void send_packet(uint8_t* data, uint16_t length){

}

enum packet_receive_error try_receive_packet(struct packet *pptr) {
    if (receive_byte() != SOF)
        return NO_SOF;
    pptr->type = receive_byte();
    pptr->length = ((uint16_t) receive_byte() << 8) + receive_byte();
    pptr->payload = malloc(pptr->length);
    for (int i = 0; i < pptr->length; ++i) {
        pptr->payload[i] = receive_byte();
    }
    pptr->crc8 = receive_byte();
    if (check_crc(pptr)) {
        return OK;
    } else {
        return CRC_INCORRECT;
    }
}

int main(int argc, char **argv) {
    struct packet Packet = {0, 8, "ti pidor", 235};
    //Packet.crc8 = gencrc((const uint8_t *) &Packet.length, Packet.length + 2);
    putc(check_crc(&Packet) + '0', stdout);
    return 0;
}
