// index.js

// import Module from './a.out'
// 获取应用实例
const app = getApp()

Page({
    data: {
        Module: {},
        ctx: null,
        canvas: null,
        flag: false,
        frame: null,
    },
    // 事件处理函数
    onLoad() {
        let cw = 480,
            ch = 640;
            var that = this
        var factory = require('./web_tracking.js')
        factory().then((Module) => {
            that.data.Module = Module;
            console.log(Module)
            var instance = new Module["ImgProc"](cw, ch, cw);
            that.data.img_processor = instance;
            that.data.imgproc_HEAPU8 = that.HEAPU8();
            const ctx = wx.createCameraContext()
            const listener = ctx.onCameraFrame((frame) => {
                console.log(frame.width)
                console.log(frame.height)
                var data = new Uint8Array(frame.data);
                var clamped = new Uint8ClampedArray(data);
                var newclamped = that.detectFastCorner(clamped, cw, ch);
                var a = that.data.ctx.createImageData(480, 640)
                a.data.set(newclamped)
                console.log(clamped)
                that.data.ctx.putImageData(a, 0, 0)
            })
            listener.start();
            const query = wx.createSelectorQuery()
            query.select('#myCanvas')
                .fields({
                    node: true,
                    size: true
                })
                .exec((res) => {
                    const canvas = res[0].node
                    const ctx = canvas.getContext('2d')
                    that.data.ctx = ctx
                    that.data.flag = true
                    const dpr = wx.getSystemInfoSync().pixelRatio
                    canvas.width = res[0].width
                    canvas.height = res[0].height
                })
        })
    },
    malloc(len) {
        return this.data.Module._malloc(len);
    },
    free(mem) {
        return this.data.Module._free(mem);
    },
    HEAPU8() {
        return this.data.Module.HEAPU8;
    },
    detectFastCorner(data, cw, ch) {
        const len = data.length;
        console.log(len)
        const mem = this.malloc(len);
        this.data.imgproc_HEAPU8.set(data, mem);
        this.data.img_processor.detectFastCorner(mem, cw, ch);
        const filtered = this.data.imgproc_HEAPU8.subarray(mem, mem + len);
        this.free(mem);
        return filtered;
    },
})