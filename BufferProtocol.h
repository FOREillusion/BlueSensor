//
// Created by Alex Chi on 2019/11/17.
//

#ifndef SERIALPB_BUFFERPROTOCOL_H
#define SERIALPB_BUFFERPROTOCOL_H

class SerialProtocol {
public:
    char read();
    bool available();
    void write(char ch);
};

const int BUFFER_T_BEGIN = -1;
const int BUFFER_T_END = -2;
const int BUFFER_END = -3;

class BufferProtocol {
    SerialProtocol *sp;
    int recv_cnt;
    int send_cnt;
    unsigned char recv_ch;
    unsigned char send_ch;
    bool transaction_begin;
public:
    BufferProtocol(SerialProtocol *sp);
    int read();
    void write(unsigned char ch);
    void begin();
    void end();
};


#endif //SERIALPB_BUFFERPROTOCOL_H
