#include "ss.h"
#include "structures.h"
#include <iostream>
#include <uv.h>

int SS::_argc = 0;

SS::SS(const Napi::CallbackInfo& info) : Napi::ObjectWrap<SS>(info) {
    Napi::Env env = info.Env();

    FitsImageStatistic* statobj =
        Napi::ObjectWrap<FitsImageStatistic>::Unwrap(info[0].As<Napi::Object>());

    _bufRef = Napi::Reference<Napi::Buffer<uint8_t>>::New(info[1].As<Napi::Buffer<uint8_t>>(), 1);

    _tsfn = Napi::ThreadSafeFunction::New(env,
        info[2].As<Napi::Function>(), // JavaScript function called asynchronously
        "Resource Name", // Name
        0, // Unlimited queue
        1, // Only one thread will use this initially
        [](Napi::Env) { // Finalizer used to clean threads up
            // nativeThread.join();
        });

    app = new QCoreApplication(_argc, nullptr);
    solver = new StellarSolver(statobj->getStat(), _bufRef.Value().Data());

    solver->setLogLevel(LOG_ALL);

    QMetaObject::Connection con =
        QObject::connect(solver, &StellarSolver::logOutput, [=](QString text) {
            auto callback = [](Napi::Env env, Napi::Function jsCallback, std::string* log) {
                jsCallback.Call(
                    {Napi::String::New(env, "log"), Napi::String::New(env, log->c_str())});
                delete log;
            };

            std::string* log = new std::string(text.toStdString());
            this->_tsfn.BlockingCall(log, callback);
        });

    QObject::connect(solver, &StellarSolver::finished, [=]() {
        auto callback = [](Napi::Env env, Napi::Function jsCallback) {
            jsCallback.Call({{Napi::String::New(env, "finished")}});
        };

        this->_tsfn.BlockingCall(callback);
    });

    uv_loop_t* loop = uv_default_loop();

    uv_idle_init(loop, &_timerh);
    _timerh.data = this;

    uv_idle_start(&_timerh, [](uv_idle_t* handle) {
        SS* myclass = (SS*)(handle->data);
        myclass->app->processEvents();
    });
}

SS::~SS() {
    // std::cout << "destructor" << std::endl;
    /*uv_idle_stop(&_timerh);
    _tsfn.Release();*/
}

Napi::Object SS::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "SS",
        {
            SS::InstanceMethod<&SS::Process>("process"),
            SS::InstanceMethod<&SS::Start>("start"),
            SS::InstanceMethod<&SS::Stop>("stop"),
            SS::InstanceMethod<&SS::GetStarList>("getStarList"),
            SS::InstanceMethod<&SS::GetStarListFromSolve>("getStarListFromSolve"),
            SS::InstanceMethod<&SS::GetSolution>("getSolution"),
            SS::InstanceMethod<&SS::SetProperty>("setProperty"),
            SS::InstanceMethod<&SS::SetParameterProfile>("setParameterProfile"),
            SS::InstanceMethod<&SS::SetSSLogLevel>("setSSLogLevel"),
            SS::InstanceMethod<&SS::SetIndexFolderPaths>("setIndexFolderPaths"),
            SS::InstanceMethod<&SS::SetPosition>("setPosition"),
            SS::InstanceMethod<&SS::SetScale>("setScale"),
            SS::InstanceMethod<&SS::ExtractionDone>("extractionDone"),
            SS::InstanceMethod<&SS::SolvingDone>("solvingDone"),
            SS::InstanceMethod<&SS::Failed>("failed"),
        });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();

    *constructor = Napi::Persistent(func);

    exports.Set("SS", func);

    env.SetInstanceData<Napi::FunctionReference>(constructor);

    return exports;
}

Napi::Value SS::Process(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    bool success = solver->solve();

    std::cout << success << std::endl;

    QList<FITSImage::Star> stars = solver->getStarList();

    std::cout << stars.size() << std::endl;

    Napi::Array res = Napi::Array::New(env, stars.size());

    int i = 0;
    for (const auto& star : stars) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set(Napi::String::New(env, "x"), Napi::Number::New(env, star.x));
        obj.Set(Napi::String::New(env, "y"), Napi::Number::New(env, star.y));
        obj.Set(Napi::String::New(env, "hfr"), Napi::Number::New(env, star.HFR));
        obj.Set(Napi::String::New(env, "flux"), Napi::Number::New(env, star.flux));
        res.Set(i++, obj);
    }

    return res;
}

Napi::Value SS::Start(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    solver->start();

    return env.Undefined();
}

Napi::Value SS::Stop(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    uv_idle_stop(&_timerh);
    _tsfn.Release();

    return env.Undefined();
}

Napi::Value SS::SetProperty(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 2) {
    }

    if (!info[0].IsString()) {
    }

    if (info[1].IsString()) {
        solver->setProperty(info[0].As<Napi::String>().Utf8Value().c_str(),
            QString(info[1].As<Napi::String>().Utf8Value().c_str()));
    } else if (info[1].IsNumber()) {
        solver->setProperty(info[0].As<Napi::String>().Utf8Value().c_str(),
            QVariant(info[1].As<Napi::Number>().DoubleValue()));
    } else if (info[1].IsBoolean()) {
        solver->setProperty(info[0].As<Napi::String>().Utf8Value().c_str(),
            QVariant(info[1].As<Napi::Boolean>().Value()));
    }

    return env.Undefined();
}

Napi::Value SS::SetParameterProfile(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1 || !info[0].IsNumber()) {
    }

    solver->setParameterProfile(static_cast<SSolver::Parameters::ParametersProfile>(
        info[0].As<Napi::Number>().Int32Value()));

    return env.Undefined();
}

Napi::Object SS::createObjectFromStar(Napi::Env env, FITSImage::Star star) {
    Napi::Object obj = Napi::Object::New(env);

    obj.Set(Napi::String::New(env, "x"), Napi::Number::New(env, star.x));
    obj.Set(Napi::String::New(env, "y"), Napi::Number::New(env, star.y));
    obj.Set(Napi::String::New(env, "hfr"), Napi::Number::New(env, star.HFR));
    obj.Set(Napi::String::New(env, "flux"), Napi::Number::New(env, star.flux));
    obj.Set(Napi::String::New(env, "ra"), Napi::Number::New(env, star.ra));
    obj.Set(Napi::String::New(env, "dec"), Napi::Number::New(env, star.dec));

    return obj;
}

Napi::Value SS::GetStarList(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    auto stars = solver->getStarList();

    Napi::Array res = Napi::Array::New(env, stars.size());

    int i = 0;
    for (const auto& star : stars) {
        res.Set(i++, createObjectFromStar(env, star));
    }

    return res;
}

Napi::Value SS::GetStarListFromSolve(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    auto stars = solver->getStarListFromSolve();

    Napi::Array res = Napi::Array::New(env, stars.size());

    int i = 0;
    for (const auto& star : stars) {
        res.Set(i++, createObjectFromStar(env, star));
    }

    return res;
}

Napi::Value SS::GetSolution(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    auto solution = solver->getSolution();

    Napi::Object res = Napi::Object::New(env);
    res.Set(Napi::String::New(env, "ra"), Napi::Number::New(env, solution.ra));
    res.Set(Napi::String::New(env, "dec"), Napi::Number::New(env, solution.dec));
    res.Set(Napi::String::New(env, "raError"), Napi::Number::New(env, solution.raError));
    res.Set(Napi::String::New(env, "decError"), Napi::Number::New(env, solution.decError));
    res.Set(Napi::String::New(env, "fieldHeight"), Napi::Number::New(env, solution.fieldHeight));
    res.Set(Napi::String::New(env, "fieldWidth"), Napi::Number::New(env, solution.fieldWidth));
    res.Set(Napi::String::New(env, "orientation"), Napi::Number::New(env, solution.orientation));
    res.Set(Napi::String::New(env, "parity"), Napi::Number::New(env, solution.parity));
    res.Set(Napi::String::New(env, "pixscale"), Napi::Number::New(env, solution.pixscale));

    return res;
}

Napi::Value SS::SetSSLogLevel(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1 || !info[0].IsNumber()) {
    }

    solver->setSSLogLevel(
        static_cast<SSolver::SSolverLogLevel>(info[0].As<Napi::Number>().Int32Value()));

    return env.Undefined();
}

Napi::Value SS::SetIndexFolderPaths(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1 || !info[0].IsString()) {
    }

    solver->setIndexFolderPaths({info[0].As<Napi::String>().Utf8Value().c_str()});

    return env.Undefined();
}

Napi::Value SS::SetPosition(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    solver->setSearchPositionInDegrees(
        info[0].As<Napi::Number>().DoubleValue(), info[1].As<Napi::Number>().DoubleValue());

    return env.Undefined();
}

Napi::Value SS::SetScale(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    solver->setSearchScale(info[0].As<Napi::Number>().DoubleValue(),
        info[1].As<Napi::Number>().DoubleValue(),
        static_cast<SSolver::ScaleUnits>(info[2].As<Napi::Number>().Int32Value()));

    return env.Undefined();
}

Napi::Value SS::ExtractionDone(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), solver->extractionDone());
}

Napi::Value SS::SolvingDone(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), solver->solvingDone());
}

Napi::Value SS::Failed(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), solver->failed());
}