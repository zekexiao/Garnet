#ifndef TESTGARNETVALUE_H
#define TESTGARNETVALUE_H

#include <QObject>

class TestGarnetValue : public QObject
{
    Q_OBJECT
public:
    explicit TestGarnetValue(QObject *parent = nullptr);
private slots:
    void testQObject();
    void testInt();
    void testDouble();
    void testString();
    void testList();
    void testHash();
    void testSymbolToString();
};

#endif // TESTGARNETVALUE_H
