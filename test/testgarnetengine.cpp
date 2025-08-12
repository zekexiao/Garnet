#include "testgarnetengine.h"
#include "test.h"
#include <Garnet/Engine>

TestGarnetEngine::TestGarnetEngine(QObject *parent)
    : QObject(parent)
{}

void TestGarnetEngine::testError()
{
    Garnet::Engine engine;
    engine.evaluate(
        "5.times do\n"
        "raise 'error'\n"
        "end",
        "*script*");
    QCOMPARE(engine.error(), QString("error (RuntimeError)"));
    QCOMPARE(engine.backtrace().size(), 1);
    QCOMPARE(engine.backtrace()[0], QString("*script*:1"));
}

ADD_TEST_CLASS(TestGarnetEngine)
