#pragma once

#include <napi.h>
#include <stellarsolver.h>

class FitsImageStatistic : public Napi::ObjectWrap<FitsImageStatistic> {
    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports);
        FitsImageStatistic(const Napi::CallbackInfo& info);
        Napi::Value GetMin(const Napi::CallbackInfo& info);
        void SetMin(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMax(const Napi::CallbackInfo& info);
        Napi::Value GetMean(const Napi::CallbackInfo& info);
        Napi::Value GetMedian(const Napi::CallbackInfo& info);
        Napi::Value GetSNR(const Napi::CallbackInfo& info);
        Napi::Value GetDataType(const Napi::CallbackInfo& info);
        //static Napi::Value SetDataType(const Napi::CallbackInfo& info);
        Napi::Value GetSamplesPerChannel(const Napi::CallbackInfo& info);
        void SetSamplesPerChannel(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetWidth(const Napi::CallbackInfo& info);
        void SetWidth(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetHeight(const Napi::CallbackInfo& info);
        void SetHeight(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetChannels(const Napi::CallbackInfo& info);
        void SetChannels(const Napi::CallbackInfo& info, const Napi::Value& value);

        FITSImage::Statistic& getStat() {
            return _stat;
        };

    private:
        void SetDoubleArray(double* array, unsigned int length, const Napi::Value& value);
        FITSImage::Statistic _stat;
};

/*
typedef struct Statistic
{
    double min[3] = {0}, max[3] = {0};  // Minimum and Maximum R, G, B pixel values in the image
    double mean[3] = {0};               // Average R, G, B value of the pixels in the image
    double stddev[3] = {0};             // Standard Deviation of the R, G, B pixel values in the image
    double median[3] = {0};             // Median R, G, B pixel value in the image
    double SNR { 0 };                   // Signal to noise ratio
    uint32_t dataType { 0 };            // FITS image data type (TBYTE, TUSHORT, TULONG, TFLOAT, TLONGLONG, TDOUBLE)
    int bytesPerPixel { 1 };            // Number of bytes used for each pixel, size of datatype above
    int ndim { 2 };                     // Number of dimensions in a fits image
    int64_t size { 0 };                 // Filesize in bytes
    uint32_t samples_per_channel { 0 }; // area of the image in pixels
    uint16_t width { 0 };               // width of the image in pixels
    uint16_t height { 0 };              // height of the image in pixels
    uint8_t channels { 1 };             // Mono Images have 1 channel, RGB has 3 channels
} Statistic;
*/
