#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

class MyPacket{
    public:
    uint8_t *data;
    int size;
};

class MyEncoder
{
private:
    const AVCodec* codec;
    AVCodecContext* c = NULL; // encoder context
    AVFrame* frame; //Frame to be encode
    AVPacket* pkt;  //Packet encoded

    int width;
    int height;
    int frame_rate;
    int bit_rate;
    int gop_size;

public:
    int encoder_init(int w, int h, int fr, int br, int gop); 

    int encoder_encode_frame(uint8_t* fdata, MyPacket* packet,  int pts);

    int encoder_close();
};
