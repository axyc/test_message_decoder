//
//  qood_messages.c
//  QoodMessageProcessor
//
//  Created by xiaop on 15/6/23.
//  Copyright (c) 2015年 xiaop. All rights reserved.
//

#include "qood_messages.h"
#include <string.h>
#define qood_message_debug_log 0

int qood_msg_feed(qood_msg_ctx_t *ctx,qood_protocol_packte_t* p,void* ud);

int qood_msg_append_buffer(qood_msg_ctx_t *ctx,uint8_t *data,uint16_t len,void *ud){
    if (ctx->recv_buffer.tail+len>mmp_recv_buff_len) {
        qood_message_log(ctx, "qood buffer full",ud);
        return -1;
    }
    memcpy(ctx->recv_buffer.data+ctx->recv_buffer.tail, data, len);
    ctx->recv_buffer.tail+=len;
    return 0;
}
#if qood_message_debug_log
void mmp_log_buffer(qood_msg_ctx_t *ctx){
    for (int i=0; i<mmp_num_of_buffer_packte; i++) {
        for (int j=0; j<mmp_packte_len; j++) {
            fprintf(stderr, "%02x",ctx->recv_buffer.data[i*mmp_packte_len+j]);
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "head:%d,tail:%d\n\n",ctx->recv_buffer.head,ctx->recv_buffer.tail);
}
#endif

int mmp_get_recive_buff_len(qood_msg_ctx_t *ctx){
    return ctx->recv_buffer.tail-ctx->recv_buffer.head;
}

int mmp_check_head(qood_msg_ctx_t *ctx){
    if (ctx->recv_buffer.data[ctx->recv_buffer.head]==mmp_header) {
        return 1;
    }else{
        return 0;
    }
}
int mmp_check_packte(qood_msg_ctx_t *ctx,qood_protocol_packte_t *p,void* ud){
    uint8_t checksum=0;
    uint8_t data[mmp_packte_len];
    memcpy(data, p, mmp_packte_len);
    for (int i=0; i<mmp_packte_len-1; i++) {
        checksum+=data[i];
    }
    if (p->crc!=checksum) {
        qood_message_log(ctx, "packte checksum error",ud);
        return -1;//checksun error
    }
    if (p->config.data_len>mmp_data_len) {
        qood_message_log(ctx, "packte length error",ud);
        return -2;//data len error
    }
    if (p->config.version != mmp_version){
        qood_message_log(ctx, "mmp version error",ud);
        return -3;
    }
    return 0;
}

int qood_packte_extract(qood_msg_ctx_t *ctx,void* ud){
    qood_protocol_packte_t p;
    while (mmp_get_recive_buff_len(ctx)>0) {
        if (mmp_check_head(ctx)) {
            if(mmp_get_recive_buff_len(ctx)>=mmp_packte_len){
                memcpy(&p, ctx->recv_buffer.data+ctx->recv_buffer.head, mmp_packte_len);
                if(mmp_check_packte(ctx,&p,ud)==0){
                    qood_msg_feed(ctx, &p,ud);
                    ctx->recv_buffer.head+=mmp_packte_len;
                }else{
                    ctx->recv_buffer.head++;
                }
            }else{
                return 0;
            }
        }else{
            ctx->recv_buffer.head++;
        }
    }
    return 0;
}

int qood_relayout_buffer(qood_msg_ctx_t *ctx,void* ud){
    if ((mmp_recv_buff_len-ctx->recv_buffer.tail)>(5*mmp_packte_len)) {
        return 1;
    }
    qood_message_log(ctx, "relayout buffer",ud);
    for (int i=0; i<ctx->recv_buffer.tail-ctx->recv_buffer.head; i++) {
        ctx->recv_buffer.data[i]=ctx->recv_buffer.data[i+ctx->recv_buffer.head];
    }
    uint8_t *p=ctx->recv_buffer.data+ctx->recv_buffer.tail-ctx->recv_buffer.head;
    memset(p, 0x0, ctx->recv_buffer.head);
    ctx->recv_buffer.tail=ctx->recv_buffer.tail-ctx->recv_buffer.head;
    ctx->recv_buffer.head=0;
    return 0;
}
qood_msg_ctx_t* qood_msg_ctx_new(){
    qood_msg_ctx_t* ctx = malloc(sizeof(qood_msg_ctx_t));
    memset(ctx, 0x0, sizeof(qood_msg_ctx_t));
    return ctx;
}
void qood_msg_ctx_destroy(qood_msg_ctx_t* ctx){
    free(ctx);
}
int qood_message_feed(qood_msg_ctx_t *ctx,const void *data,uint16_t len,void* ud){
#if qood_message_debug_log
    fprintf(stderr, "before append buffer\n");
    mmp_log_buffer(ctx);
#endif
    if (qood_msg_append_buffer(ctx, (uint8_t *)data, len,ud)!=0) {
        return -1;//append error
    }
#if qood_message_debug_log
    fprintf(stderr, "after append buffer\n");
    mmp_log_buffer(ctx);
#endif
    if (qood_packte_extract(ctx,ud)!=0) {
        return -2;
    }
#if qood_message_debug_log
    fprintf(stderr, "after extract buffer\n");
    mmp_log_buffer(ctx);
#endif
    if (qood_relayout_buffer(ctx,ud)<0) {
        return -3;
    }
#if qood_message_debug_log
    fprintf(stderr, "after relayout buffer\n");
    mmp_log_buffer(ctx);
#endif
    return 0;
}

int qood_msg_feed(qood_msg_ctx_t *ctx,qood_protocol_packte_t* p,void* ud){
    if (p->config.frame_type==mmp_single_frame) {
        qood_message_callback(ctx, p->fid, p->pid, p->data, p->config.data_len,ud);
    }else if (p->config.frame_type==mmp_multy_first_frame) {
        ctx->multy_packet_buffer.err=0;
        ctx->multy_packet_buffer.fid=p->fid;
        ctx->multy_packet_buffer.pid=p->pid;
        memcpy(ctx->multy_packet_buffer.data, p->data, p->config.data_len);
        ctx->multy_packet_buffer.tail=p->config.data_len;
        ctx->multy_packet_buffer.index=p->config.index;
    }else if(p->config.frame_type==mmp_multy_middle_frame){
        if (ctx->multy_packet_buffer.err==0&&ctx->multy_packet_buffer.fid==p->fid&&ctx->multy_packet_buffer.pid==p->pid&&ctx->multy_packet_buffer.index!=p->config.index) {
            memcpy(ctx->multy_packet_buffer.data+ctx->multy_packet_buffer.tail, p->data, p->config.data_len);
            ctx->multy_packet_buffer.tail+=p->config.data_len;
            ctx->multy_packet_buffer.index=p->config.index;
        }else{
            ctx->multy_packet_buffer.err=1;
        }
    }else if(p->config.frame_type==mmp_multy_last_frame){
        memcpy(ctx->multy_packet_buffer.data+ctx->multy_packet_buffer.tail, p->data, p->config.data_len);
        ctx->multy_packet_buffer.tail+=p->config.data_len;
        qood_message_callback(ctx, ctx->multy_packet_buffer.fid, ctx->multy_packet_buffer.pid, ctx->multy_packet_buffer.data, ctx->multy_packet_buffer.tail,ud);
    }
    return 0;
}

