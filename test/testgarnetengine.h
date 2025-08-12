#ifndef TESTGARNETENGINE_H
#define TESTGARNETENGINE_H

#include <QObject>

class TestGarnetEngine : public QObject
{
    Q_OBJECT
public:
    explicit TestGarnetEngine(QObject *parent = nullptr);

private slots:
    void testError();
};

#endif // TESTGARNETENGINE_H
