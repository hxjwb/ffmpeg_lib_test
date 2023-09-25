#include "avcodec_encode.hpp"

int MyEncoder::encoder_init(int w, int h, int fr, int br, int gop){
    
    width = w;
    height = h;
    frame_rate = fr;
    bit_rate = br;
    gop_size = gop;

#if H265
    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
#else
    codec = avcodec_find_encoder(AV_CODEC_ID_H265);
#endif
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    // context
    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    pkt = av_packet_alloc();
    if (!pkt)
        exit(1);

    /* put sample parameters */
    c->bit_rate = bit_rate;
    /* resolution must be a multiple of two */
    c->width = width;
    c->height = height;

    /* frames per second */
    c->time_base.den = frame_rate;
    c->time_base.num = 1;
    c->framerate.den = 1;
    c->framerate.num = frame_rate;

    c->pix_fmt = AV_PIX_FMT_YUV420P; // only support YUV420P

    
    c->max_b_frames = 0; // should be 0 when using zerolatency

    // Set preset and tune for libx264 and libx265
    if (codec->id == AV_CODEC_ID_H264 || codec->id == AV_CODEC_ID_H265) {
        av_opt_set(c->priv_data, "preset", "ultrafast", 0);
        av_opt_set(c->priv_data, "tune", "zerolatency", 0);
        
    }

    /* open Encoder */
    int ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width = c->width;
    frame->height = c->height;

    ret = av_frame_get_buffer(frame, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }

    return 0;

}

int MyEncoder::encoder_encode_frame(uint8_t* fdata, MyPacket* packet,  int pts){
     fflush(stdout);

    /* make sure the frame data is writable */
    int ret = av_frame_make_writable(frame);
    if (ret < 0){
        fprintf(stderr, "Error: av_frame_make_writable\n");
        exit(1);
    }
        
    // prepare frame (YUV)
    memcpy(frame->data[0], fdata, width * height);
    fdata += width * height;
    memcpy(frame->data[1], fdata, width * height / 4);
    fdata += width * height / 4;
    memcpy(frame->data[2], fdata, width * height / 4);


    frame->pts = pts;

    // encode frame!
    /* send the frame to the encoder */
    if (frame)
        printf("Send frame %3lld\n", frame->pts);

    ret = avcodec_send_frame(c, frame);
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        exit(1);
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(c, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return -1;
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }

        printf("Write packet %3lld (size=%5d)\n", pkt->pts, pkt->size);

        packet->size = pkt->size;
        packet->data = new uint8_t[packet->size];
        memcpy(packet->data, pkt->data, pkt->size);
        
        av_packet_unref(pkt);
    }

    return 0;
}


int MyEncoder::encoder_close(){
    /* flush the encoder */
    int ret;


    ret = avcodec_send_frame(c, NULL);
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        exit(1);
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(c, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return -1;
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }

        printf("Write packet %3lld (size=%5d)\n", pkt->pts, pkt->size);
        av_packet_unref(pkt);
    }

    /* free the encoder */
    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);

    return 0;
}