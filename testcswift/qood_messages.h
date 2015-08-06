//
//  qood_messages.h
//  QoodMessageProcessor
//
//  Created by xiaop on 15/6/23.
//  Copyright (c) 2015年 xiaop. All rights reserved.
//

#ifndef __QoodMessageProcessor__qood_messages__
#define __QoodMessageProcessor__qood_messages__

#pragma mark -
#pragma mark message context
#include <stdlib.h>

#define mmp_data_len 15  //数据长度
#define mmp_packte_len 20   //包长度
#define mmp_num_of_buffer_packte 1024   //缓存包数目
#define mmp_recv_buff_len (mmp_packte_len*mmp_num_of_buffer_packte) //缓存空间大小
#define mmp_multy_pack_buffer_len (mmp_packte_len*10)   //多包缓存空间大小
#define mmp_header 0x55    //包头
#define mmp_version 0x00    //协议版本号

#define mmp_single_frame 0x00   //单包flag
#define mmp_multy_first_frame 0x01  //多包第一包flag
#define mmp_multy_middle_frame 0x02 //多包中间包flag
#define mmp_multy_last_frame 0x03  //多包末尾包flag
/*
 * 底层书报格式
 */
typedef struct{
    uint8_t header;
    struct {
        uint8_t data_len:4;
        uint8_t frame_type:2;
        uint8_t index:1;
        uint8_t version:1;
    }config;
    uint8_t fid;
    uint8_t pid;
    uint8_t data[mmp_data_len];
    uint8_t crc;
}qood_protocol_packte_t;
typedef struct{
    struct{
        uint8_t data[mmp_recv_buff_len];
        uint16_t head;
        uint16_t tail;
    }recv_buffer;
    struct{
        uint8_t fid;
        uint8_t pid;
        uint8_t data[mmp_multy_pack_buffer_len];
        uint16_t tail;
        uint8_t index:1;
        uint8_t err;
    }multy_packet_buffer;
} qood_msg_ctx_t;
qood_msg_ctx_t* qood_msg_ctx_new();
void qood_msg_ctx_destroy(qood_msg_ctx_t* ctx);
int qood_message_feed(qood_msg_ctx_t *ctx,const void *data,uint16_t len,void* ud);
extern void qood_message_log(qood_msg_ctx_t *ctx,const char *msg,void* ud);
extern int qood_message_callback(qood_msg_ctx_t *ctx,uint8_t fid,uint8_t pid,const void *data,uint8_t len,void* ud);
#endif /* defined(__QoodMessageProcessor__qood_messages__) */
