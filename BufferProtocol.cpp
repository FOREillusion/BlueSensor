//
// Created by Alex Chi on 2019/11/17.
//

#include "BufferProtocol.h"
#include "HardwareSerial.h"

BufferProtocol::BufferProtocol(SerialProtocol *sp) : sp(sp), send_cnt(0), recv_cnt(0), transaction_begin(false) {

}

int BufferProtocol::read() {
    while (true) {
        if (!sp->available()) return BUFFER_END;
        unsigned char ch = sp->read();
        if (ch & 0x80) {
            // Control Mode
            if (ch == 0xff) {
                recv_cnt = 0;
                transaction_begin = true;
                return BUFFER_T_BEGIN;
            } else {
                transaction_begin = false;
                return BUFFER_T_END;
            }
        } else {
            if (!transaction_begin) continue;
            // Transmission Mode
            unsigned char x = recv_ch | (ch >> (7 - recv_cnt));
            ++recv_cnt;
            if (recv_cnt == 8) {
                recv_cnt = 0;
                recv_ch = 0;
            } else {
                recv_ch = ch << recv_cnt;
            }
            if (recv_cnt != 1) return x;
        }
    }
}

void BufferProtocol::write(unsigned char ch) {
    ++send_cnt;
    unsigned char x = send_ch | (ch >> send_cnt);
    send_ch = (ch << (7 - send_cnt)) & ~0x80;
    sp->write(x);
    if (send_cnt == 7) {
        sp->write(ch & ~0x80);
        send_cnt = 0;
        send_ch = 0;
    }
}

void BufferProtocol::begin() {
    sp->write(0xff);
    send_ch = 0;
    send_cnt = 0;
}

void BufferProtocol::end() {
    if (send_cnt != 0) sp->write(send_ch);
    sp->write(0xfe);
}

char SerialProtocol::read() {
    return Serial.read();
}

bool SerialProtocol::available() {
    return Serial.available();
}

void SerialProtocol::write(char ch) {
    Serial.write(ch);
}
