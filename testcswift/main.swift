//
//  main.swift
//  testcswift
//
//  Created by peng on 15/8/5.
//  Copyright (c) 2015年 peng. All rights reserved.
//

import Foundation

let data = NSData(contentsOfFile: "btlog.merged.raw")!
var decoder = QoodMessageDecoder()
for i in 0 ..< data.length / 20 {
    let sub = data.subdataWithRange(NSMakeRange(i * 20, 20))
    decoder.feed(sub)
}
