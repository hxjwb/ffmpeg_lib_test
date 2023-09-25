# include <iostream>
# include <string>
# include <fstream>

#include "avcodec_encode.hpp"

using namespace std;



int main(){
    string input_file = "C:/Users/Administrator/Documents/sequences/ParkScene_1920x1080_24.yuv";
    string output_file = "test.bin";

    ifstream input(input_file, ios::binary);
    ofstream output(output_file, ios::binary);

    MyEncoder encoder;

    int width = 1920;
    int height = 1080;
    int frame_rate = 24;

    int Mbps = 5;
    int bit_rate = Mbps * 1000 * 1000;
    int gop_size = 30;

    int frames = 240;
    encoder.encoder_init(width, height, frame_rate, bit_rate, gop_size);

    uint8_t* frame = new uint8_t[width * height * 3 / 2];

    MyPacket* packet = new MyPacket;

    for (int i = 0; i < frames; i++){
        input.read((char*)frame, width * height * 3 / 2);
        encoder.encoder_encode_frame(frame,packet,i);
        output.write((char*)packet->data, packet->size);
    }

    encoder.encoder_close();
    // fwrite(endcode, 1, sizeof(endcode), f);
    uint8_t endcode[]  = { 0, 0, 1, 0xb7 };
    output.write((char*)endcode, sizeof(endcode));


}