#include "structures.h"

FitsImageStatistic::FitsImageStatistic(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<FitsImageStatistic>(info) {
    Napi::Env env = info.Env();

    _stat.dataType = 20;
    _stat.bytesPerPixel = sizeof(int16_t);
}

Napi::Object FitsImageStatistic::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "FitsImageStatistic",
        {
            FitsImageStatistic::InstanceAccessor<&FitsImageStatistic::GetMin,
                &FitsImageStatistic::SetMin>("min"),
            FitsImageStatistic::InstanceAccessor<&FitsImageStatistic::GetMax>("max"),
            FitsImageStatistic::InstanceAccessor<&FitsImageStatistic::GetMean>("mean"),
            FitsImageStatistic::InstanceAccessor<&FitsImageStatistic::GetMedian>("median"),
            FitsImageStatistic::InstanceAccessor<&FitsImageStatistic::GetDataType>("dataType"),
            FitsImageStatistic::InstanceAccessor<&FitsImageStatistic::GetSamplesPerChannel,
                &FitsImageStatistic::SetSamplesPerChannel>("samplesPerChannel"),
            FitsImageStatistic::InstanceAccessor<&FitsImageStatistic::GetWidth,
                &FitsImageStatistic::SetWidth>("width"),
            FitsImageStatistic::InstanceAccessor<&FitsImageStatistic::GetHeight,
                &FitsImageStatistic::SetHeight>("height"),
            FitsImageStatistic::InstanceAccessor<&FitsImageStatistic::GetChannels,
                &FitsImageStatistic::SetChannels>("channels"),
        });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();

    *constructor = Napi::Persistent(func);

    exports.Set("FitsImageStatistic", func);

    env.SetInstanceData<Napi::FunctionReference>(constructor);

    return exports;
}

Napi::Value FitsImageStatistic::GetMin(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Array res = Napi::Array::New(env, 3);

    for (unsigned int i = 0; i <= 2; i++) {
        res.Set(i, Napi::Number::New(env, _stat.min[i]));
    }

    return res;
}

void FitsImageStatistic::SetMin(const Napi::CallbackInfo& info, const Napi::Value& value) {
    Napi::Env env = info.Env();

    SetDoubleArray(_stat.min, 3, value);
}

Napi::Value FitsImageStatistic::GetMax(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Array res = Napi::Array::New(env, 3);

    for (unsigned int i = 0; i <= 2; i++) {
        res.Set(i, Napi::Number::New(env, _stat.max[i]));
    }

    return res;
}

Napi::Value FitsImageStatistic::GetMean(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Array res = Napi::Array::New(env, 3);

    for (unsigned int i = 0; i <= 2; i++) {
        res.Set(i, Napi::Number::New(env, _stat.mean[i]));
    }

    return res;
}

Napi::Value FitsImageStatistic::GetMedian(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Array res = Napi::Array::New(env, 3);

    for (unsigned int i = 0; i <= 2; i++) {
        res.Set(i, Napi::Number::New(env, _stat.median[i]));
    }

    return res;
}

Napi::Value FitsImageStatistic::GetDataType(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::Number::New(env, _stat.dataType);
}

Napi::Value FitsImageStatistic::GetSamplesPerChannel(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), _stat.samples_per_channel);
}

void FitsImageStatistic::SetSamplesPerChannel(const Napi::CallbackInfo& info, const Napi::Value& value) {
    if (value.IsNumber()) {
        _stat.samples_per_channel = value.As<Napi::Number>().Uint32Value();
    }
}

Napi::Value FitsImageStatistic::GetWidth(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), _stat.width);
}

void FitsImageStatistic::SetWidth(const Napi::CallbackInfo& info, const Napi::Value& value) {
    if (value.IsNumber()) {
        _stat.width = value.As<Napi::Number>().Uint32Value();
    }
}

Napi::Value FitsImageStatistic::GetHeight(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), _stat.height);
}

void FitsImageStatistic::SetHeight(const Napi::CallbackInfo& info, const Napi::Value& value) {
    if (value.IsNumber()) {
        _stat.height = value.As<Napi::Number>().Uint32Value();
    }
}

Napi::Value FitsImageStatistic::GetChannels(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), _stat.channels);
}

void FitsImageStatistic::SetChannels(const Napi::CallbackInfo& info, const Napi::Value& value) {
    if (value.IsNumber()) {
        _stat.channels = value.As<Napi::Number>().Uint32Value();
    }
}

void FitsImageStatistic::SetDoubleArray(
    double* array, unsigned int length, const Napi::Value& value) {
    if (value.IsNumber()) {
        for (unsigned int i = 0; i < length; i++)
            array[i] = value.As<Napi::Number>().DoubleValue();

        return;
    }

    if (value.IsArray()) {
        Napi::Array values = value.As<Napi::Array>();

        for (unsigned int i = 0; i < length && i <= values.Length(); i++) {
            Napi::Value v = values[i];
            if (v.IsNumber()) {
                array[i] = v.As<Napi::Number>().DoubleValue();
            }
        }

        return;
    }
}
