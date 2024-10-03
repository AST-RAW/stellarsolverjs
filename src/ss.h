#pragma once

#include <napi.h>
#include <stellarsolver.h>
#include <QCoreApplication>
#include <QPushButton>
#include <uv.h>

class SS : public Napi::ObjectWrap<SS> {
    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports);
        SS(const Napi::CallbackInfo& info);
        ~SS();
        Napi::Value Process(const Napi::CallbackInfo& info);
        Napi::Value Start(const Napi::CallbackInfo& info);
        Napi::Value Stop(const Napi::CallbackInfo& info);
        Napi::Value SetProperty(const Napi::CallbackInfo& info);
        Napi::Value SetParameterProfile(const Napi::CallbackInfo& info);
        Napi::Value GetStarList(const Napi::CallbackInfo& info);
        Napi::Value GetStarListFromSolve(const Napi::CallbackInfo& info);
        Napi::Value GetSolution(const Napi::CallbackInfo& info);
        Napi::Value SetSSLogLevel(const Napi::CallbackInfo& info);
        Napi::Value SetIndexFolderPaths(const Napi::CallbackInfo& info);
        Napi::Value SetPosition(const Napi::CallbackInfo& info);
        Napi::Value SetScale(const Napi::CallbackInfo& info);
        Napi::Value ExtractionDone(const Napi::CallbackInfo& info);
        Napi::Value SolvingDone(const Napi::CallbackInfo& info);
        Napi::Value Failed(const Napi::CallbackInfo& info);

    private:
        Napi::Object createObjectFromStar(Napi::Env env, FITSImage::Star star);
        QCoreApplication* app;
        StellarSolver* solver;
        static int _argc;
        uv_idle_t _timerh;
        Napi::FunctionReference _cb;
        Napi::ThreadSafeFunction _tsfn;
        Napi::Reference<Napi::Buffer<uint8_t>> _bufRef;
};
