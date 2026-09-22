#ifndef FUSSBALLBACKEND_H
#define FUSSBALLBACKEND_H

#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>

#include "fontobfuscation/fontdecoder.h"

class FussballBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int matchDay READ matchDay NOTIFY matchDayChanged)
    Q_PROPERTY(QJsonArray matches READ matches NOTIFY matchesChanged)

public:
    explicit FussballBackend(const QString &fontDir, QObject *parent = nullptr);

    int matchDay() const { return m_matchDay; }
    const QJsonArray &matches() const { return m_matches; }

    Q_INVOKABLE void getMatchDay(int matchDay);

signals:
    void matchDayChanged(int matchDay);
    void matchesChanged();
    void resultReady(const QJsonObject &decodedJson);
    void loadFailed(const QString &error);

private:
    void fetchJson(int matchDay);
    void fetchFont(const QString &fontId);
    void decodeAndStore();

    QNetworkAccessManager m_network;
    FontDecoder m_decoder;
    QString m_fontDir;
    int m_matchDay = 0;
    QJsonArray m_matches;
    QJsonObject m_jsonDoc;
    QString m_fontId;
};

#endif // FUSSBALLBACKEND_H