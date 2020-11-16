//
// Created by huku on 12.11.2020.
//


#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>

#define SOF 73

uint8_t buffer[256];
int bufpos = 0;

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
    return buffer[bufpos++];
}

void send_byte(uint8_t byte) {
    //stub
    buffer[bufpos++] = byte;
}

void send_packet(const struct packet *pptr){
    send_byte(SOF);
    send_byte(pptr->type);
    send_byte( pptr->length >> (uint16_t)8);
    send_byte(pptr->length & 0xff);
    for (int i = 0; i < pptr->length; ++i) {
        send_byte(pptr->payload[i]);
    }
    send_byte(pptr->crc8);
}

struct packet make_packet(uint8_t type, uint8_t* data, uint16_t length){
    struct packet p;
    p.type = type;
    p.length = length;
    p.payload = data;
    p.crc8 = gencrc((const uint8_t *) &p.type, p.type + 3);
    return p;
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
    char data[] = "this is the test data\0";
    struct packet sent = make_packet(0, data, strlen(data));
    send_packet(&sent);
    struct packet received;
    bufpos = 0;
    enum packet_receive_error code = try_receive_packet(&received);
    if (code == CRC_INCORRECT){
        fputs("crc doesn't match!", stderr);
        return 1;
    } else if (code == NO_SOF){
        fputs("no SOF!", stderr);
        return 2;
    }
    puts(received.payload);
    puts("OK!");
    return 0;
}
