#include <stellarsolver.h>
#include <napi.h>
#include <fitsio.h>

#include "structures.h"
#include "ss.h"
#include <iostream>

Napi::String Method(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    FitsImageStatistic* statobj = Napi::ObjectWrap<FitsImageStatistic>::Unwrap(info[0].As<Napi::Object>());
    Napi::Buffer<uint8_t> buffer = info[1].As<Napi::Buffer<uint8_t>>();

    FITSImage::Statistic stat = statobj->getStat();

    StellarSolver solver(stat, buffer.Data());

    //solver.start();

    /*fitsfile *fptr{nullptr};
    int status = 0;

    fits_open_diskfile(&fptr, "test.fit", READONLY, &status);*/

    return Napi::String::New(env, "world");
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "hello"),
            Napi::Function::New(env, Method));

    SS::Init(env, exports);
    FitsImageStatistic::Init(env, exports);

    return exports;
}

NODE_API_MODULE(addon, Init)
