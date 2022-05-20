// index.js

const app = getApp()

Page({
    data: {
        Module: {},
        ctx: null,
        canvas: null,
        img_processor:null,
        imgproc_HEAPU8:null,
        fs:'',
        time:0,
    },
    // 事件处理函数
    onLoad() {
        var time = new Date().valueOf()
        var cnt = 0;
        var fs = 0
        wx.startGyroscope({
            interval: 'game',
            success: function() {
              wx.onGyroscopeChange((result) => {
                cnt+=1
                fs = cnt/(new Date().valueOf()-time)
              })
            }
        })
        var that = this
        var factory = require('./web_tracking_wasm.js')
        factory().then((Module) => {
            that.data.Module = Module;
            console.log(Module)
            this.testmain()
            this.eigenTest()
            const ctx = wx.createCameraContext()
            const listener = ctx.onCameraFrame((frame) => {
                let ch = frame.height
                let cw = frame.width
                if(that.data.time === 0){
                    that.data.img_processor= new Module["ImgProc"](cw, ch, cw);
                    that.data.imgproc_HEAPU8 = that.HEAPU8();
                }
                var data = new Uint8Array(frame.data);
                var clamped = new Uint8ClampedArray(data);
                var newclamped = that.detectFastCorner(clamped, cw, ch);
                var a = that.data.ctx.createImageData(cw, ch)
                a.data.set(newclamped)
                that.data.ctx.putImageData(a, 0, 0)
                that.data.time += 1;
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
        const mem = this.malloc(len);
        this.data.imgproc_HEAPU8.set(data, mem);
        this.data.img_processor.detectFastCorner(mem, cw, ch);
        const filtered = this.data.imgproc_HEAPU8.subarray(mem, mem + len);
        this.free(mem);
        return filtered;
    },
    AngleVectorFromRotation(quaternion) {
        var Module = this.data.Module
        var data = new Module['DoubleVector']();
        for (var i=0; i<quaternion.length; ++i)
        {
          data.push_back(quaternion[i]);
        }
        var av = Module.MatrixProc.AngleVectorFromRotation(data);
        data.delete();
        return av;
      },
      Chi2(size, res, S) {
        var Module = this.data.Module
        var res_data = new Module.DoubleVector();
        for (var i=0; i<res.length; ++i)
        {
          res_data.push_back(res[i]);
        }
        var S_data = new Module.DoubleVector();
        for (var i=0; i<S.length; ++i)
        {
          S_data.push_back(S[i]);
        }
        var chi2 = Module.MatrixProc.Chi2(size, res_data, S_data);
        res_data.delete();
        S_data.delete();
        return chi2;
      },
      eigenTest() {
        let q_array = [0.99894403, -0.00045966289, -0.045835741, 0.0031135603];
        let angle_vector = this.AngleVectorFromRotation(q_array);
        for (var i = 0; i < angle_vector.size(); i++) {
          console.log(angle_vector.get(i));
        }
  
        let size = 9;
        let res_array = [0.14740699, -2.3008106, -0.14179067, -1.642709, -0.087483585, -1.1038882, 0.088762618, -0.20561008, 0.2652723];
        let S_array = [47.43882, -19.394628, 35.115914, -24.109937, 20.93693, -24.344906, 12.571666, -18.995493, -15.804585,
          -19.394628, 195.90506, -26.512773, 159.49212, -33.557157, 122.85588, -37.92472, 83.965011, 53.268423,
          35.115914, -26.512773, 29.936722, -29.362635, 18.750332, -28.247698, 12.663787, -21.77703, -17.438169,
          -24.109937, 159.49212, -29.362635, 135.61078, -32.344451, 106.85348, -34.346363, 74.671888, 49.191292,
          20.93693, -33.557157, 18.750332, -32.344451, 14.927402, -28.557874, 11.081678, -21.200425, -15.727509,
          -24.344906, 122.85588, -28.247698, 106.85348, -28.557874, 88.212634, -28.969528, 62.159677, 42.260373,
          12.571666, -37.92472, 12.663787, -34.346363, 11.081678, -28.969528, 11.231062, -21.057158, -14.836223,
          -18.995493, 83.965011, -21.77703, 74.671888, -21.200425, 62.159677, -21.057158, 45.925328, 31.154471,
          -15.804585, 53.268423, -17.438169, 49.191292, -15.727509, 42.260373, -14.836223, 31.154471, 23.305918];
        let chi2 = this.Chi2(size, res_array, S_array);
        console.log(chi2);
      },
      testmain(){
        var Module = this.data.Module;
        Module.testmain()
      }
})