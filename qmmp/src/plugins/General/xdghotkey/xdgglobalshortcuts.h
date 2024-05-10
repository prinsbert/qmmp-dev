#ifndef XDGGLOBALSHORTCUTS_H
#define XDGGLOBALSHORTCUTS_H

#include <QObject>
#include <QDBusObjectPath>

class XdgGlobalShortcuts : public QObject
{
    Q_OBJECT
public:
    explicit XdgGlobalShortcuts(QObject *parent = nullptr);

private slots:
    void onSessionCreateResponse(uint res, const QVariantMap &results);

private:
    QString getSessionToken();
    QString getRequestToken();

    uint m_sessionTokenCounter = 0;
    uint m_requestTokenCounter = 0;
    QDBusObjectPath m_sessionHandle;
};

#endif // XDGGLOBALSHORTCUTS_H
