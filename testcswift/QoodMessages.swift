//
//  QoodMessages.swift
//  qood_message_decoder
//
//  Created by peng on 15/8/6.
//  Copyright (c) 2015年 peng. All rights reserved.
//

import Foundation

struct ObdConnect:QoodCmd {
    private var _timestamp:UInt32
    var model1Support:UInt32
    var model2Support:UInt32
    var protocolType:UInt8
    var mafMapType:UInt8
    var timestamp:NSDate{
        return NSDate(timeIntervalSince1970: NSTimeInterval(_timestamp))
    }
    var description:String{
        return "ObdConnect:\(timestamp,model1Support,model2Support,protocolType,mafMapType)"
    }
}
struct RtcFeedback:QoodCmd{
    private var _rtcOld:UInt32
    private var _rtcNew:UInt32
    var rtcOld:NSDate{
        return NSDate(timeIntervalSince1970: NSTimeInterval(_rtcOld))
    }
    var rtcNew:NSDate{
        return NSDate(timeIntervalSince1970: NSTimeInterval(_rtcNew))
    }
    var description:String{
        return "RtcFeedback:\(rtcOld,rtcNew)"
    }
}

struct ObdSum:QoodCmd{
    private var _timestamp:UInt32
    private var _mileage:UInt24
    private var _fuel:UInt24
    private var _duration:UInt24
    var mileage:UInt32{
        return _mileage.uInt32Value
    }
    var duration:UInt32{
        return _duration.uInt32Value
    }
    var fuel:UInt32{
        return _fuel.uInt32Value
    }
    var timestamp:NSDate{
        return NSDate(timeIntervalSince1970: NSTimeInterval(_timestamp))
    }
    var description:String{
        return "ObdSum:\(timestamp,mileage,duration,fuel)"
    }
}

struct Realtime:QoodCmd{
    private var _timestamp:UInt32
    var rpm:UInt16
    var speed:UInt8
    var fuel:UInt16
    var engin:UInt8
    var timestamp:NSDate{
        return NSDate(timeIntervalSince1970: NSTimeInterval(_timestamp))
    }
    var description:String{
        return "Realtime:\(timestamp,rpm,speed,fuel,engin)"
    }
}
struct VehiclePara:QoodCmd{
    var _timestamp:UInt32
    var fuleLevel:UInt8
    var waterTemp:Int8
    var timestamp:NSDate{
        return NSDate(timeIntervalSince1970: NSTimeInterval(_timestamp))
    }
    var description:String{
        return "VehiclePara:\(timestamp,fuleLevel,waterTemp)"
    }
}
