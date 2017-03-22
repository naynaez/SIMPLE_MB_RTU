#include <stdio.h>
#include "simple_mb_slave.h"

uint8_t rx_data[20] = "";
uint8_t tx_data[20] = "";

void modbus_cb(mb_req_packet* p_req) {
    printf("DATA ADDR %d\n", p_req->addr);
    printf("DATA POINT HI %d\n", p_req->no_point_hi);
    printf("DATA POINT LOW %d\n", p_req->no_point_low);

    // TODO : implement tx function by your self
    tx_data[0] = 0x11;
    tx_data[1] = 0x04;
    tx_data[2] = 0x02;
    tx_data[3] = 0x00;
    tx_data[4] = 0x20;
    tx_data[5] = 0x79;
    tx_data[6] = 0x2B;
}

int main(void) {

    printf("helloworld\n");
    mb_init(0x11, rx_data, modbus_cb); //// (Device Address , pointer to DATA , Fx callback pointer extract to address stucture)

    // 11 03 00 6B 00 03 7687
    printf("---------Pack 1---------\n");
    mb_rx(0x11);
    mb_rx(0x04);
    mb_rx(0x00);
    mb_rx(0x6B);
    mb_rx(0x00);
    mb_rx(0x03);
    mb_rx(0x76);
    mb_rx(0x87);

    printf("---------Pack 2---------\n");

    mb_rx(0x11);
    mb_rx(0x03);
    mb_rx(0x00);
    mb_rx(0x6B);
    mb_rx(0x00);
    mb_rx(0x03);
    mb_rx(0x76);
    mb_rx(0x87);
    return 0;
}
