let Module;
let imgproc_HEAPU8;
let img_processor;
let ch,cw;
let framedata;
function testmain() {
    Module.testmain()
}
function malloc(len) {
    return Module._malloc(len);
}
function free(mem) {
    return Module._free(mem);
}
function HEAPU8() {
    return Module.HEAPU8;
}
function detectFastCorner(data, cw, ch) {
    const len = data.length;
    const mem = malloc(len);
    imgproc_HEAPU8.set(data, mem);
    var num = img_processor.detectFastCorner(mem, cw, ch);
    const filtered = imgproc_HEAPU8.subarray(mem, mem + len);
    free(mem);
    return filtered;
}
var factory = require('./out')
factory().then((m) => {
    Module = m
    testmain()
    console.log(Module)
    worker.postMessage({
        msg: 'ready'
    })
    worker.onMessage(function (res) {
        if(res[0] === "init"){
            ch = res[1]
            cw = res[2]
            img_processor= new Module["ImgProc"](cw, ch, cw);
            imgproc_HEAPU8 = HEAPU8();
            console.log(ch)
            console.log(cw)
        }
        else{
            framedata = res
            var data = new Uint8Array(framedata);
            var clamped = new Uint8ClampedArray(data);
            var newclamped = detectFastCorner(clamped,cw,ch);
            var clp = new Uint8ClampedArray(newclamped).buffer
            worker.postMessage(clp)
            console.log(newclamped.length)
        }
    })
})