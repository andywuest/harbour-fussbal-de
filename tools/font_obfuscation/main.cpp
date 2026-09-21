#include "fontdecoder.h"

#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QFile>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QStatusBar>
#include <QTableWidget>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWidget>

#include <cstdio>

namespace {

const QString kDefaultUrl =
    QStringLiteral("https://next.fussball.de/_next/data/3GE9wzufdQY_tRu-Qkpfs/de/widget/competition"
                   "/ac5e71aa-1ee8-4579-9508-1a2f27fe240d/spieltag/1.json");

QByteArray readLocalOrFetch(const QString &path, bool *ok)
{
    if (QFile::exists(path)) {
        QFile f(path);
        if (f.open(QIODevice::ReadOnly)) {
            *ok = true;
            return f.readAll();
        }
    }
    *ok = false;
    return QByteArray();
}

} // namespace

class DecoderWindow : public QMainWindow
{
public:
    DecoderWindow()
    {
        setWindowTitle(QStringLiteral("fussball.de font-obfuscation decoder"));

        m_url = new QLineEdit(kDefaultUrl);
        m_fontId = new QLineEdit(QStringLiteral("auto"));
        m_fontId->setToolTip(QStringLiteral("auto = use the obfuscatedFont field from the payload"));
        m_decode = new QPushButton(QStringLiteral("Decode"));
        m_goToSource = new QPushButton(QStringLiteral("Open endpoint"));

        auto *top = new QHBoxLayout;
        top->addWidget(new QLabel(QStringLiteral("JSON URL / file path:")));
        top->addWidget(m_url, 1);
        top->addWidget(new QLabel(QStringLiteral("Font id:")));
        top->addWidget(m_fontId);
        top->addWidget(m_decode);

        m_matches = new QTableWidget(0, 4);
        m_matches->setHorizontalHeaderLabels(
            {QStringLiteral("Datum / Zeit"), QStringLiteral("Heim"),
             QStringLiteral("Ergebnis"), QStringLiteral("Gast")});
        m_matches->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        m_json = new QPlainTextEdit;
        m_json->setReadOnly(true);
        m_json->setMaximumBlockCount(200000);

        auto *split = new QSplitter(Qt::Vertical);
        split->addWidget(m_matches);
        split->addWidget(m_json);

        auto *layout = new QVBoxLayout;
        layout->addLayout(top);
        layout->addWidget(split, 1);
        auto *central = new QWidget;
        central->setLayout(layout);
        setCentralWidget(central);
        statusBar()->showMessage(QStringLiteral("Ready"));

        m_nam = new QNetworkAccessManager(this);
        connect(m_decode, &QPushButton::clicked, this, &DecoderWindow::start);
        connect(m_goToSource, &QPushButton::clicked, this,
                [this] { QDesktopServices::openUrl(QUrl(m_url->text())); });

        resize(900, 700);
    }

private:
    void start()
    {
        const QString src = m_url->text().trimmed();
        if (src.isEmpty())
            return;
        m_pendingJson.clear();
        m_fontOverride = m_fontId->text().trimmed();
        statusBar()->showMessage(QStringLiteral("Downloading JSON …"));
        bool local = false;
        m_pendingJson = readLocalOrFetch(src, &local);
        if (local) {
            onJsonReady(true);
        } else {
            auto *reply = m_nam->get(QNetworkRequest(QUrl(src)));
            connect(reply, &QNetworkReply::finished, this, [this, reply] {
                reply->deleteLater();
                if (reply->error() != QNetworkReply::NoError) {
                    statusBar()->showMessage(QStringLiteral("JSON fetch failed: %1")
                                                 .arg(reply->errorString()));
                    return;
                }
                m_pendingJson = reply->readAll();
                onJsonReady(true);
            });
        }
    }

    void onJsonReady(bool)
    {
        QJsonParseError err;
        m_doc = QJsonDocument::fromJson(m_pendingJson, &err);
        if (err.error != QJsonParseError::NoError) {
            statusBar()->showMessage(QStringLiteral("Invalid JSON: %1").arg(err.errorString()));
            return;
        }

        QString fontId = m_fontOverride;
        if (fontId.isEmpty() || fontId.compare(QLatin1String("auto"), Qt::CaseInsensitive) == 0) {
            fontId = m_doc.object().value(QLatin1String("pageProps"))
                         .toObject()
                         .value(QLatin1String("obfuscatedFont"))
                         .toString();
            if (fontId.isEmpty()) {
                statusBar()->showMessage(
                    QStringLiteral("No obfuscatedFont field found and no font id given"));
                return;
            }
        }

        statusBar()->showMessage(QStringLiteral("Downloading obfuscated font %1 …").arg(fontId));
        const QString fontUrl = QStringLiteral("https://www.fussball.de/export.fontface/-/format/"
                                               "woff/id/%1/type/font")
                                    .arg(fontId);

        m_fontPath = QStringLiteral("/tmp/fussball-decode-%1.woff").arg(fontId);
        bool local = false;
        const QByteArray cached = readLocalOrFetch(m_fontPath, &local);
        if (local) {
            loadFontData(cached);
            return;
        }
        auto *reply = m_nam->get(QNetworkRequest(QUrl(fontUrl)));
        connect(reply, &QNetworkReply::finished, this, [this, reply] {
            reply->deleteLater();
            if (reply->error() != QNetworkReply::NoError) {
                statusBar()->showMessage(
                    QStringLiteral("Font fetch failed: %1").arg(reply->errorString()));
                return;
            }
            const QByteArray fontData = reply->readAll();
            QFile f(m_fontPath);
            f.open(QIODevice::WriteOnly);
            f.write(fontData);
            loadFontData(fontData);
        });
    }

    void loadFontData(const QByteArray &fontData)
    {
        if (!m_decoder.loadFont(fontData)) {
            statusBar()->showMessage(QStringLiteral("Font parse failed: %1").arg(m_decoder.error()));
            return;
        }
        statusBar()->showMessage(QStringLiteral("Decoded OK"));

        const QJsonValue decoded =
            FontDecoder::decodeJson(QJsonValue(m_doc.object()), m_decoder);
        m_json->setPlainText(QString::fromUtf8(
            QJsonDocument(decoded.toObject()).toJson(QJsonDocument::Indented)));

        const QJsonArray matches =
            decoded.toObject().value(QLatin1String("pageProps")).toObject()
                .value(QLatin1String("matches"))
                .toArray();
        const QString competition = decoded.toObject()
            .value(QLatin1String("pageProps")).toObject()
            .value(QLatin1String("competitionName")).toString();
        setWindowTitle(QStringLiteral("fussball.de decoder — %1").arg(competition));

        m_matches->setRowCount(matches.size());
        for (int i = 0; i < matches.size(); ++i) {
            const QJsonObject m = matches.at(i).toObject();
            const QJsonObject kickoff = m.value(QLatin1String("kickoff")).toObject();
            const QJsonObject home = m.value(QLatin1String("homeTeam")).toObject();
            const QJsonObject guest = m.value(QLatin1String("guestTeam")).toObject();
            const QString when = kickoff.value(QLatin1String("dateWithWeekday")).toString() + QLatin1String("  ")
                + kickoff.value(QLatin1String("time")).toString();
            const QString score = m.value(QLatin1String("result")).toObject()
                                      .value(QLatin1String("text"))
                                      .toString();
            QTableWidgetItem *items[4] = {
                new QTableWidgetItem(when),
                new QTableWidgetItem(home.value(QLatin1String("name")).toString()),
                new QTableWidgetItem(score),
                new QTableWidgetItem(guest.value(QLatin1String("name")).toString()),
            };
            for (int c = 0; c < 4; ++c)
                m_matches->setItem(i, c, items[c]);
        }
    }

    QLineEdit *m_url = nullptr;
    QLineEdit *m_fontId = nullptr;
    QPushButton *m_decode = nullptr;
    QPushButton *m_goToSource = nullptr;
    QTableWidget *m_matches = nullptr;
    QPlainTextEdit *m_json = nullptr;
    QNetworkAccessManager *m_nam = nullptr;
    QByteArray m_pendingJson;
    QJsonDocument m_doc;
    FontDecoder m_decoder;
    QString m_fontOverride;
    QString m_fontPath;
};

int runHeadlessSelfTest(const QString &jsonPath, const QString &fontPath)
{
    QFile jf(jsonPath);
    if (!jf.open(QIODevice::ReadOnly)) {
        std::fprintf(stderr, "cannot open %s\n", qPrintable(jsonPath));
        return 1;
    }
    const QByteArray json = jf.readAll();

    FontDecoder dec;
    if (!dec.loadFont([&] {
            QFile ff(fontPath);
            ff.open(QIODevice::ReadOnly);
            return ff.readAll();
        }())) {
        std::fprintf(stderr, "font error: %s\n", qPrintable(dec.error()));
        return 1;
    }

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError)
        return 1;
    const QJsonValue out = FontDecoder::decodeJson(QJsonValue(doc.object()), dec);
    QTextStream ts(stdout);
    ts << QJsonDocument(out.toObject()).toJson(QJsonDocument::Indented);
    return 0;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    if (argc == 4 && QLatin1String(argv[1]) == QLatin1String("--selftest"))
        return runHeadlessSelfTest(QString::fromLocal8Bit(argv[2]), QString::fromLocal8Bit(argv[3]));

    DecoderWindow win;
    win.show();
    return app.exec();
}