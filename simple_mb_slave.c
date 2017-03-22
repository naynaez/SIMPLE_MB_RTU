#include "simple_mb_slave.h"

#define __ENABLE_FULL_DEBUG__   // Use to enable dubug in hardware (Recommend)
#ifdef  __ENABLE_FULL_DEBUG__
#include <stdio.h>
#define debug_print(...) printf(__VA_ARGS__)
#else
#define debug_print(...)
#endif


#define STATE_FIND_ADDR     0
#define STATE_FIND_DATA     1
#define STATE_CAL_CRC1      2
#define STATE_CAL_CRC2      3

static uint8_t* p_rx_buf;
static uint8_t mb_state = STATE_FIND_ADDR;
static uint8_t mb_addr  = 0x01;
static uint8_t mb_count = 0;
static mb_data_cb g_mb_data_cb;
mb_req_packet *p_req;


static uint16_t crc16_update(uint16_t crc, uint8_t a)
{
    int i;
    crc ^= a;
    for (i = 0; i < 8; ++i)
    {
        if (crc & 1)
            crc = (crc >> 1) ^ 0xA001;
        else
            crc = (crc >> 1);
    }
    return crc;
}

void mb_init(uint8_t _mb_addr,
             uint8_t *rx_buffer,
             mb_data_cb mb_cb)
{

    p_rx_buf     = rx_buffer;
    mb_addr      = _mb_addr;
    g_mb_data_cb = mb_cb;
}

void mb_rx(uint8_t data) {
    static uint16_t crc = 0xFFFF;
    static uint16_t req_crc = 0;

    switch(mb_state) {
        case STATE_FIND_ADDR :
        if(data == mb_addr) {
            crc = 0xFFFF;
            req_crc = 0;         ////// reset value
            mb_count = 0;
            mb_state = STATE_FIND_DATA;
            p_rx_buf[0] = data;
            debug_print("CRC CAL %d %X LAST DATA : %X\n", mb_count, crc, data);
            crc = crc16_update(crc, data);       ////// return new crc (Overwirte)
        }
        break;

        case STATE_FIND_DATA :
        mb_count = mb_count + 1;
        p_rx_buf[mb_count] = data;
        crc = crc16_update(crc, data);
        debug_print("CRC CAL %d %X LAST DATA : %X\n", mb_count, crc, data);
        if(mb_count >= 5) {
            mb_state = STATE_CAL_CRC1;
        }
        break;

        case STATE_CAL_CRC1 :
        req_crc = data;
        mb_state = STATE_CAL_CRC2;
        break;

        case STATE_CAL_CRC2 :
        req_crc |= (data << 8);
        debug_print("CRC RCV %X\n", req_crc);
            if(crc == req_crc) {
                p_req = (mb_req_packet *)p_rx_buf;
                debug_print("CRC CORRECT\n");
                debug_print("DATA ADDR %d\n", p_req->addr);
                debug_print("DATA POINT HI %d\n", p_req->no_point_hi);
                debug_print("DATA POINT LOW %d\n", p_req->no_point_low);
                g_mb_data_cb(p_req);
            } else {
                debug_print("CRC INCORRECT\n");
            }
        mb_state = STATE_FIND_ADDR;

        default :
        break;
    }
}
