#include "fussballbackend.h"

#include "constants.h"

#include <QByteArray>
#include <QCryptographicHash>
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

QString imageSuffix(const QByteArray &contentType)
{
    const QByteArray type = contentType.split(';').first().trimmed().toLower();
    if (type == "image/jpeg" || type == "image/jpg") {
        return QStringLiteral(".jpg");
    }
    if (type == "image/svg+xml") {
        return QStringLiteral(".svg");
    }
    if (type == "image/webp") {
        return QStringLiteral(".webp");
    }
    if (type == "image/gif") {
        return QStringLiteral(".gif");
    }
    return QStringLiteral(".png");
}

} // namespace

FussballBackend::FussballBackend(const QString &fontDir, const QString &logoDir, QObject *parent)
    : QObject(parent)
    , m_fontDir(fontDir)
    , m_logoDir(logoDir)
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

QString FussballBackend::logoFileName(const QString &logoUrl) const
{
    // The logo url contains a stable club id,
    // e.g. https://www.fussball.de/export.media/-/action/getLogo/id/<clubId>/verband/<associationId>
    const QString path = QUrl(logoUrl).path();
    const int idIndex = path.indexOf(QStringLiteral("/id/"));
    if (idIndex >= 0) {
        const QString clubId = path.mid(idIndex + 4).section(QLatin1Char('/'), 0, 0);
        if (!clubId.isEmpty()) {
            return clubId;
        }
    }

    return QString::fromLatin1(
        QCryptographicHash::hash(logoUrl.toUtf8(), QCryptographicHash::Sha1).toHex());
}

QString FussballBackend::cachedLogoUrl(const QString &logoUrl) const
{
    if (logoUrl.isEmpty()) {
        return QString();
    }

    const QString logoName = logoFileName(logoUrl);
    const QDir logoDir(m_logoDir);
    const QStringList fileNames = logoDir.entryList(QDir::Files, QDir::Name);
    for (const QString &fileName : fileNames) {
        if (fileName.startsWith(logoName + QLatin1Char('.'))) {
            qDebug() << "logo taken from file:" << logoDir.filePath(fileName);
            return QUrl::fromLocalFile(logoDir.filePath(fileName)).toString();
        }
    }

    return QString();
}

QString FussballBackend::storeLogo(const QString &logoUrl, const QByteArray &logoData,
                                   const QByteArray &contentType)
{
    const QString logoPath = QDir(m_logoDir).filePath(logoFileName(logoUrl) + imageSuffix(contentType));

    QFile file(logoPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "logo could not be stored:" << logoPath;
        return QString();
    }
    file.write(logoData);
    file.close();
    qDebug() << "logo stored in file:" << logoPath;

    return logoPath;
}

void FussballBackend::cacheLogo(const QString &logoUrl)
{
    if (logoUrl.isEmpty() || !cachedLogoUrl(logoUrl).isEmpty()) {
        return;
    }

    if (m_pendingLogos.contains(logoUrl)) {
        return;
    }
    m_pendingLogos.insert(logoUrl);

    qDebug() << "logo url: " << logoUrl;

    const QUrl url(logoUrl);
    QNetworkRequest request(url);
    // request.setTransferTimeout(30000);

    QNetworkReply *reply = m_network.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, logoUrl] {
        reply->deleteLater();
        m_pendingLogos.remove(logoUrl);
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "logo could not be loaded:" << logoUrl << reply->errorString();
            return;
        }

        const QString logoPath = storeLogo(logoUrl, reply->readAll(),
                                           reply->header(QNetworkRequest::ContentTypeHeader).toByteArray());
        if (logoPath.isEmpty()) {
            return;
        }

        emit logoReady(logoUrl, QUrl::fromLocalFile(logoPath).toString());
    });
}
