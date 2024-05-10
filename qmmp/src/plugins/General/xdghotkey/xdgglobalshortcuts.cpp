#include <QDBusMessage>
#include <QDBusConnection>
#include <QMap>
#include <QtDebug>
#include <qmmp/qmmp.h>
#include "xdgglobalshortcuts.h"

XdgGlobalShortcuts::XdgGlobalShortcuts(QObject *parent)
    : QObject{parent}
{
    qDebug() << Q_FUNC_INFO;

    QDBusMessage createSessionMessage =  QDBusMessage::createMethodCall(u"org.freedesktop.portal.Desktop"_s,
                                                                       u"/org/freedesktop/portal/desktop"_s,
                                                                       u"org.freedesktop.portal.GlobalShortcuts"_s,
                                                                       u"CreateSession"_s);
    const QMap<QString, QVariant> createSessionOptions = {
        { u"handle_token"_s, u"qmmp"_s },
        { u"session_handle_token"_s, getRequestToken() }
    };

    createSessionMessage.setArguments({ createSessionOptions });

    QDBusMessage reply = QDBusConnection::sessionBus().call(createSessionMessage);
    m_sessionHandle = reply.arguments().constFirst().value<QDBusObjectPath>();

    QDBusConnection::sessionBus().connect(u"org.freedesktop.portal.Desktop"_s, m_sessionHandle.path(),
                                          u"org.freedesktop.portal.Request"_s, u"Response"_s,
                                          this, SLOT(onSessionCreateResponse(uint,QVariantMap)));
}

void XdgGlobalShortcuts::onSessionCreateResponse(uint res, const QVariantMap &results)
{
    qDebug() << Q_FUNC_INFO << res << results;
}

QString XdgGlobalShortcuts::getSessionToken()
{
    return QStringLiteral("u%1").arg(++m_sessionTokenCounter);
}

QString XdgGlobalShortcuts::getRequestToken()
{
    return QStringLiteral("u%1").arg(++m_requestTokenCounter);
}

