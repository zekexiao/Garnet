#ifndef TEST_H
#define TEST_H

#include <Garnet/VariadicArgument>
#include <QtTest>

#define GARNET_TEST_COMPARE_QVARIANT(a, b) \
    do { \
        if (a.metaType() == QMetaType::fromType<double>() \
            && b.metaType() == QMetaType::fromType<double>()) { \
            QCOMPARE(a.toDouble(), b.toDouble()); \
        } else { \
            QCOMPARE(a, b); \
        } \
    } while (false)

#define GARNET_TEST_COMPARE_QLIST(a, b) \
    do { \
        QCOMPARE(a.size(), b.size()); \
        for (int i = 0; i < a.size(); ++i) { \
            GARNET_TEST_COMPARE_QVARIANT(a[i], b[i]); \
        } \
    } while (false)

#define GARNET_TEST_COMPARE_QHASH(a, b) \
    do { \
        QCOMPARE(a.size(), b.size()); \
        for (auto it = a.begin(); it != a.end(); ++it) { \
            auto bIt = b.find(it.key()); \
            if (bIt == b.end()) { \
                QVERIFY(false); \
            } else { \
                GARNET_TEST_COMPARE_QVARIANT(it.value(), bIt.value()); \
            } \
        } \
    } while (false)

class TestObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text MEMBER text_)

public:
    Q_INVOKABLE TestObject() = default;
    Q_INVOKABLE TestObject(const QString &text)
        : text_(text)
    {}

    Q_INVOKABLE double testMethod(int v1, double v2, bool v3, const QString &v4)
    {
        return v1 * v2 + (v3 ? -5 : 5) + v4.toInt();
    }

    Q_INVOKABLE QVariant
    testVariantMethod(const QVariant &v1, const QVariant &v2, const QVariant &v3)
    {
        return QString("%1-%2").arg(v1.toInt() + v2.toDouble()).arg(v3.toString());
    }

    Q_INVOKABLE int testVariadicMethod(const QString &v1, const Garnet::VariadicArgument &vr)
    {
        auto list = vr.toList();
        return v1.toInt() + list[0].toDouble() * list[1].toDouble();
    }

    Q_INVOKABLE int testOverloadedMethod() { return 1; }

    Q_INVOKABLE int testOverloadedMethod(int x) { return x; }

private:
    QString text_;
};

void addTestObject(QObject *testObject);

#define ADD_TEST_CLASS(klass) \
    static int _dummyValue = []() { \
        addTestObject(new klass()); \
        return 0; \
    }();

#endif // TEST_H
