//
//  QoodMessageUtils.swift
//  qood_message_decoder
//
//  Created by peng on 15/8/6.
//  Copyright (c) 2015年 peng. All rights reserved.
//

import Foundation

class QoodMessageDecoder {
    var ctx:COpaquePointer = qood_msg_ctx_new()
    func log(msg:String){
        println(msg)
    }
    func feed(data:NSData){
        var me:QoodMessageDecoder = self
        qood_message_feed(ctx, data.bytes, Int16(data.length),unsafeAddressOf(self))
    }

    func input(cmd:String,data:NSData){
        switch cmd{
        case "8000":
            output(data, type: ObdConnect.self)
        case "8100":
            output(data, type: ObdSum.self)
        case "8101":
            output(data, type: Realtime.self)
        case "8102":
            output(data, type: VehiclePara.self)
        case "8400":
            output(data, type: RtcFeedback.self)
        default:
            self.log("unknow cmd:\(cmd,data)")
        }
    }
    private func output<T>(data:NSData,type:T.Type){
        var t:T = self.parse(data)
        //self.log("\(t)")
    }
    private func parse<T>(data:NSData) -> T{
        var tuple = UnsafeMutablePointer<T>.alloc(1)
        memcpy(tuple, data.bytes, data.length)
        tuple.dealloc(1)
        var t:T = tuple.memory
        return t
    }
    deinit{
        qood_msg_ctx_destroy(ctx)
    }
}

@asmname("qood_message_feed")
func qood_message_feed(ctx: COpaquePointer,data:UnsafePointer<Void>,len:Int16,ud:UnsafePointer<Void>) ->Int32

@asmname("qood_msg_ctx_new")
func qood_msg_ctx_new() ->COpaquePointer

@asmname("qood_msg_ctx_destroy")
func qood_msg_ctx_destroy(COpaquePointer) -> Void


@asmname("qood_message_callback")
func qood_message_callback(ctx:COpaquePointer,fid:UInt8,pid:UInt8,data:UnsafePointer<Void>,len:Int16,ud:UnsafePointer<Void>)->Void{
    var data:NSData = NSData(bytes: data, length: Int(len))
    let cmd = NSString(format: "%02X%02X", fid,pid) as String
    let p=unsafeBitCast(ud, QoodMessageDecoder.self)
    p.input(cmd, data: data)
}

@asmname("qood_message_log")
func qood_message_log(ctx: COpaquePointer, msg: UnsafePointer<Int8>,ud:UnsafePointer<Void>){
    let message = NSString(CString: msg, encoding: NSUTF8StringEncoding) ?? ""
    let p=unsafeBitCast(ud, QoodMessageDecoder.self)
    p.log(message as String)
}

struct Int24 {
    var value:(Int8,Int8,Int8)
    var int32Value:Int32{
        return (Int32(value.2) << 16) + (Int32(value.1) << 8) + (Int32(value.0) << 0)
    }
}
struct UInt24 {
    var value:(UInt8,UInt8,UInt8)
    var uInt32Value:UInt32{
        return (UInt32(value.2) << 16) + (UInt32(value.1) << 8) + (UInt32(value.0) << 0)
    }
}

protocol QoodCmd:Printable{}
