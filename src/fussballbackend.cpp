#include "fussballbackend.h"

#include "constants.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

namespace {

QString fontUrl(const QString &fontId)
{
    return QStringLiteral("https://www.fussball.de/export.fontface/-/format/woff/id/%1/type/font")
        .arg(fontId);
}

} // namespace

FussballBackend::FussballBackend(QObject *parent)
    : QObject(parent)
{
}

void FussballBackend::getMatchDay(int matchDay)
{
    m_matchDay = matchDay;
    emit matchDayChanged(matchDay);
    fetchJson(matchDay);
}

void FussballBackend::fetchJson(int matchDay)
{
    QNetworkRequest request(QUrl(gameResultsEndpoint(matchDay)));
    // request.setTransferTimeout(30000);

    QNetworkReply *reply = m_network.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit loadFailed(reply->errorString());
            return;
        }

        QJsonParseError error;
        const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            emit loadFailed(error.errorString());
            return;
        }

        m_jsonDoc = doc.object();
        const QString fontId = m_jsonDoc.value(QStringLiteral("pageProps"))
                                   .toObject()
                                   .value(QStringLiteral("obfuscatedFont"))
                                   .toString();
        if (fontId.isEmpty()) {
            emit loadFailed(QStringLiteral("No obfuscatedFont field found in payload"));
            return;
        }
        fetchFont(fontId);
    });
}

void FussballBackend::fetchFont(const QString &fontId)
{
    m_fontId = fontId;
    QNetworkRequest request(QUrl(fontUrl(fontId)));
    // request.setTransferTimeout(30000);

    QNetworkReply *reply = m_network.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit loadFailed(reply->errorString());
            return;
        }

        if (!m_decoder.loadFont(reply->readAll())) {
            emit loadFailed(m_decoder.error());
            return;
        }
        decodeAndStore();
    });
}

void FussballBackend::decodeAndStore()
{
    const QJsonValue decoded = FontDecoder::decodeJson(QJsonValue(m_jsonDoc), m_decoder);
    m_matches = decoded.toObject()
                    .value(QStringLiteral("pageProps"))
                    .toObject()
                    .value(QStringLiteral("matches"))
                    .toArray();
    emit matchesChanged();
}
