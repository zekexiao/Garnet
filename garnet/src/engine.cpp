#include "engine.h"
#include "variadicargument.h"
#include "bridgeclass.h"
#include "conversion.h"
#include "utils.h"

#include <QVariant>
#include <QStringList>

#include <mruby.h>
#include <mruby/compile.h>

extern "C" mrb_value mrb_get_backtrace(mrb_state *mrb, mrb_value self);

namespace Garnet {

class Engine::Private
{
public:

    Private(Engine *engine, mrb_state *mrb);
    ~Private();

    Engine *q = nullptr;
    mrb_state *mrb_ = nullptr;
    bool ownsMrb_ = false;

    QScopedPointer<BridgeClass> bridgeClass;
    QScopedPointer<StaticBridgeClassManager> staticBridgeClassManager;
    QHash<QByteArray, QVariant> registeredVariants_;

    bool hasError_ = false;
    QString error_;
    QStringList backtrace_;

    static QHash<mrb_state *, Engine *> engines_;

    static void initializeGlobal();

    void setHasError(bool hasError) {
        if (hasError_ != hasError) {
            hasError_ = hasError;
            emit q->hasErrorChanged(hasError);
        }
    }

    void setError(const QString &error) {
        if (error_ != error) {
            error_ = error;
            emit q->errorChanged(error);
        }
    }

    void setBacktrace(const QStringList &backtrace) {
        if (backtrace_ != backtrace) {
            backtrace_ = backtrace;
            emit q->backtraceChanged(backtrace);
        }
    }

    void dumpError()
    {
        if (mrb_->exc) {
            ArenaSaver as(mrb_);

            auto exc = mrb_obj_value(mrb_->exc);

            auto backtraceVlist = Conversion::toQVariant(mrb_, mrb_get_backtrace(mrb_, exc)).toList();
            QStringList backtrace;
            backtrace.reserve(backtraceVlist.size());
            for (const auto &v : backtraceVlist) {
                backtrace << v.toString();
            }

            auto error = Conversion::toQVariant(mrb_, mrb_inspect(mrb_, exc)).toString();

            mrb_->exc = nullptr;

            setHasError(true);
            setError(error);
            setBacktrace(backtrace);
        }
        else {
            setHasError(false);
            setError(QString());
            setBacktrace(QStringList());
        }
    }
};

QHash<mrb_state *, Engine *> Engine::Private::engines_;

Engine::Private::Private(Engine *engine, mrb_state *mrb) :
    q(engine)
{
    initializeGlobal();

    if (!mrb) {
        ownsMrb_ = true;
        mrb = mrb_open();
    }
    mrb_ = mrb;
    engines_[mrb] = q;

    bridgeClass.reset(new BridgeClass(mrb));
    staticBridgeClassManager.reset(new StaticBridgeClassManager(mrb));
}

void Engine::Private::initializeGlobal()
{
    static bool initialized = false;
    if (initialized) return;
    initialized = true;

    qRegisterMetaType<VariadicArgument>("Garnet::VariadicArgument");
}

Engine::Private::~Private()
{
    if (ownsMrb_) {
        mrb_close(mrb_);
    }
    engines_.remove(mrb_);
}

Engine::Engine(QObject *parent) :
    Engine(nullptr, parent)
{
}

Engine::Engine(mrb_state *mrb, QObject *parent) :
    QObject(parent),
    d(new Private(this, mrb))
{
}

Engine::~Engine()
{
}

Engine *Engine::findByMrb(mrb_state *mrb)
{
    return Private::engines_.value(mrb, nullptr);
}

mrb_state *Engine::mrbState()
{
    return d->mrb_;
}

BridgeClass &Engine::bridgeClass()
{
    return *d->bridgeClass;
}

StaticBridgeClassManager &Engine::staticBridgeClassManager()
{
    return *d->staticBridgeClassManager;
}

void Engine::collectGarbage()
{
    mrb_full_gc(d->mrb_);
}

QVariant Engine::evaluate(const QString &script, const QString &fileName)
{
    auto mrb = d->mrb_;
    ArenaSaver as(mrb);

    auto context = mrbc_context_new(mrb);

    mrbc_filename(mrb, context, fileName.toUtf8().data());
    auto value = mrb_load_string_cxt(mrb, script.toUtf8().data(), context);
    mrbc_context_free(mrb, context);
    d->dumpError();

    return Conversion::toQVariant(mrb, value);
}

bool Engine::hasError() const
{
    return d->hasError_;
}

QString Engine::error() const
{
    return d->error_;
}

QStringList Engine::backtrace() const
{
    return d->backtrace_;
}

void Engine::registerClass(const QMetaObject *metaObject)
{
    d->staticBridgeClassManager->define(metaObject);
}

void Engine::registerObject(const QString &name, QObject *object)
{
    registerVariant(name, QVariant::fromValue(object));
}

void Engine::registerVariant(const QString &name, const QVariant &variant)
{
    auto byteArray = name.toUtf8();
    d->registeredVariants_[byteArray] = variant;
    auto methodImpl = [](mrb_state *mrb, mrb_value self) {
        Q_UNUSED(self);
        auto name = mrb_sym2name(mrb, mrb->c->ci->mid);
        return Conversion::toMrbValue(mrb, findByMrb(mrb)->d->registeredVariants_[name]);
    };
    mrb_define_method(d->mrb_, d->mrb_->kernel_module, byteArray, methodImpl, MRB_ARGS_ANY());
}

}
