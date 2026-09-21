#pragma once

#include <QByteArray>
#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QStringList>

class FontDecoder
{
public:
    bool loadFont(const QByteArray &fontData);
    QString error() const { return m_error; }
    bool ready() const { return m_ready; }

    QString decodeText(const QString &pua) const;
    static QJsonValue decodeJson(const QJsonValue &value, const FontDecoder &dec);
    QString debugGlyph(quint32 cp) const { return m_plain.value(cp); }
    const QHash<quint32, QString> &plainMap() const { return m_plain; }

private:
    struct Table {
        QByteArray tag;
        QByteArray data;
    };

    static quint16 be16(const char *p);
    static quint32 be32(const char *p);
    static bool readWoffOrSfnt(const QByteArray &blob, QList<Table> *tables);
    static bool parseCmap(const QByteArray &cmap, QMap<quint32, quint16> *out);
    static bool parsePost(const Table &sfnt, QHash<quint16, QString> *byGlyphIndex);

    bool m_ready = false;
    QString m_error;
    QHash<quint32, QString> m_plain; // PUA codepoint -> decoded text
    QMap<quint32, quint16> m_cmapDbg;
    QHash<quint16, QString> m_nameDbg;
};