#include <emscripten/bind.h>
#include "imgproc.h"

using namespace emscripten;
using namespace WebTracking;

EMSCRIPTEN_BINDINGS(Web_Tracking) {
    emscripten::class_<ImgProc>("ImgProc")
        .constructor<int, int, int>()
        .function("detectFastCorner", &ImgProc::detectFastCorner, allow_raw_pointers());
}