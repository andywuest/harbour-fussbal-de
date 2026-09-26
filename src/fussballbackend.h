#ifndef FUSSBALLBACKEND_H
#define FUSSBALLBACKEND_H

#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QObject>
#include <QSet>
#include <QString>

#include "fontobfuscation/fontdecoder.h"

class FussballBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int matchDay READ matchDay NOTIFY matchDayChanged)
    Q_PROPERTY(QJsonArray matches READ matches NOTIFY matchesChanged)

public:
    explicit FussballBackend(const QString &fontDir, const QString &logoDir,
                             QObject *parent = nullptr);

    int matchDay() const { return m_matchDay; }
    const QJsonArray &matches() const { return m_matches; }

    Q_INVOKABLE void getMatchDay(int matchDay);

    // Returns the local file url of an already downloaded logo, empty if the logo is not available yet.
    Q_INVOKABLE QString cachedLogoUrl(const QString &logoUrl) const;
    // Downloads the logo, unless it has already been downloaded before.
    Q_INVOKABLE void cacheLogo(const QString &logoUrl);

signals:
    void matchDayChanged(int matchDay);
    void matchesChanged();
    void resultReady(const QJsonObject &decodedJson);
    void loadFailed(const QString &error);
    void logoReady(const QString &logoUrl, const QString &localFileUrl);

private:
    void fetchJson(int matchDay);
    void fetchFont(const QString &fontId);
    void decodeAndStore();
    QString logoFileName(const QString &logoUrl) const;
    QString storeLogo(const QString &logoUrl, const QByteArray &logoData, const QByteArray &contentType);

    QNetworkAccessManager m_network;
    FontDecoder m_decoder;
    QString m_fontDir;
    QString m_logoDir;
    QSet<QString> m_pendingLogos;
    int m_matchDay = 0;
    QJsonArray m_matches;
    QJsonObject m_jsonDoc;
    QString m_fontId;
};

#endif // FUSSBALLBACKEND_H