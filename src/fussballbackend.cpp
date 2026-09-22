#include "fussballbackend.h"

#include "constants.h"

#include <QDebug>
#include <QDir>
#include <QFile>
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

FussballBackend::FussballBackend(const QString &fontDir, QObject *parent)
    : QObject(parent)
    , m_fontDir(fontDir)
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

    // looks like there is an indefinite number of font files, since we are always getting a different fontId
    const QString fontPath = QDir(m_fontDir).filePath(fontId + QStringLiteral(".woff"));
    if (QFile::exists(fontPath)) {
        qDebug() << "font taken from file:" << fontPath;
        QFile file(fontPath);
        if (file.open(QIODevice::ReadOnly) && m_decoder.loadFont(file.readAll())) {
            decodeAndStore();
            return;
        }
        qDebug() << "font file could not be used:" << fontPath;
    }

    qDebug() << "font url: " << QUrl(fontUrl(fontId));

    QNetworkRequest request(QUrl(fontUrl(fontId)));
    // request.setTransferTimeout(30000);

    QNetworkReply *reply = m_network.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, fontPath] {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit loadFailed(reply->errorString());
            return;
        }

        const QByteArray fontData = reply->readAll();

        QFile file(fontPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(fontData);
            file.close();
            qDebug() << "font stored in file:" << fontPath;
        } else {
            qDebug() << "font could not be stored:" << fontPath;
        }

        if (!m_decoder.loadFont(fontData)) {
            emit loadFailed(m_decoder.error());
            return;
        }
        decodeAndStore();
    });
}

void FussballBackend::decodeAndStore()
{
    const QJsonValue decoded = FontDecoder::decodeJson(QJsonValue(m_jsonDoc), m_decoder);
    const QJsonObject decodedJson = decoded.toObject();
    m_matches = decodedJson.value(QStringLiteral("pageProps"))
                    .toObject()
                    .value(QStringLiteral("matches"))
                    .toArray();
    emit matchesChanged();
    emit resultReady(decodedJson);
}
