// decode_font_cli.cpp -- C++/Qt port of decode_font.py
// Builds the PUA-code-point -> plain-text mapping for a fussball.de
// obfuscated font and stores it in a JSON file (same schema as decode_font.py).
//
//   decode_font <fontId> [-o output.json] [--font-file path.woff]
//
// With --font-file the font is read locally (fontId is stored as null);
// otherwise it is fetched from:
//
//   https://www.fussball.de/export.fontface/-/format/woff/id/<fontId>/type/font

#include "fontdecoder.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QEventLoop>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

#include <cstdio>

namespace {

constexpr quint32 kPuaMin = 0xE000;
constexpr quint32 kPuaMax = 0xF8FF;

QString fontUrl(const QString &fontId)
{
    return QStringLiteral("https://www.fussball.de/export.fontface/-/format/woff/id/%1/type/font")
        .arg(fontId);
}

QByteArray fetchFont(const QString &fontId, QString *error)
{
    QNetworkAccessManager nam;
    QNetworkRequest req(QUrl(fontUrl(fontId)));
    req.setTransferTimeout(30000);
    std::unique_ptr<QNetworkReply> reply(nam.get(req));

    QEventLoop loop;
    QObject::connect(reply.get(), &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        *error = QStringLiteral("font fetch failed: %1").arg(reply->errorString());
        return QByteArray();
    }
    return reply->readAll();
}

QJsonObject buildMappingJson(const QString &fontId, bool fromFile, const FontDecoder &dec)
{
    QJsonObject mapping;
    const auto &plain = dec.plainMap();
    for (auto it = plain.cbegin(); it != plain.cend(); ++it) {
        const quint32 cp = it.key();
        if (cp < kPuaMin || cp > kPuaMax)
            continue;
        mapping.insert(QStringLiteral("%1").arg(cp, 4, 16, QLatin1Char('0')), it.value());
    }

    QJsonObject root;
    root.insert(QStringLiteral("fontId"), fromFile ? QJsonValue() : QJsonValue(fontId));
    root.insert(QStringLiteral("mapping"), mapping);
    return root;
}

} // namespace

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("decode_font"));
    QCoreApplication::setApplicationVersion(QStringLiteral("1.0"));

    QCommandLineParser parser;
    parser.setApplicationDescription(
        QStringLiteral("Decode a fussball.de obfuscated font into a PUA -> plain-text mapping."));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(QStringLiteral("font_id"),
                                 QStringLiteral("obfuscatedFont id from pageProps.obfuscatedFont"));
    QCommandLineOption outputOpt(
        QStringLiteral("o"), QStringLiteral("output mapping file (default: font_mapping.json)"),
        QStringLiteral("output"), QStringLiteral("font_mapping.json"));
    QCommandLineOption fontFileOpt(
        QStringLiteral("font-file"), QStringLiteral("read a local woff file instead of downloading"),
        QStringLiteral("path"));
    parser.addOption(outputOpt);
    parser.addOption(fontFileOpt);
    parser.process(app);

    const QStringList positional = parser.positionalArguments();
    const bool fromFile = parser.isSet(fontFileOpt);
    if (positional.isEmpty() && !fromFile) {
        parser.showHelp(1);
    }
    const QString fontId = fromFile ? QString() : positional.first();
    const QString outputPath = parser.value(outputOpt);

    QByteArray fontData;
    QString error;
    if (fromFile) {
        QFile f(parser.value(fontFileOpt));
        if (!f.open(QIODevice::ReadOnly)) {
            std::fprintf(stderr, "cannot open %s\n", qPrintable(parser.value(fontFileOpt)));
            return 1;
        }
        fontData = f.readAll();
    } else {
        std::fprintf(stderr, "downloading font %s ...\n", qPrintable(fontId));
        fontData = fetchFont(fontId, &error);
        if (fontData.isEmpty()) {
            std::fprintf(stderr, "error: %s\n", qPrintable(error));
            return 1;
        }
    }

    FontDecoder dec;
    if (!dec.loadFont(fontData)) {
        std::fprintf(stderr, "font parse failed: %s\n", qPrintable(dec.error()));
        return 1;
    }

    const QJsonObject root = buildMappingJson(fontId, fromFile, dec);
    const int count = root.value(QStringLiteral("mapping")).toObject().size();
    QFile out(outputPath);
    if (!out.open(QIODevice::WriteOnly)) {
        std::fprintf(stderr, "cannot write %s\n", qPrintable(outputPath));
        return 1;
    }
    out.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    std::fprintf(stderr, "wrote %d PUA mappings to %s\n", count, qPrintable(outputPath));
    return 0;
}