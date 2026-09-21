#include "fontdecoder.h"

#include "agl_data.h"
#include "post_names.h"

#include <zlib.h>

#include <QDebug>

namespace {

quint16 be16At(const QByteArray &d, int off)
{
    const char *p = d.constData() + off;
    return static_cast<quint16>((unsigned char)p[0] << 8 | (unsigned char)p[1]);
}

quint32 be32At(const QByteArray &d, int off)
{
    const char *p = d.constData() + off;
    return (quint32)(unsigned char)p[0] << 24 | (quint32)(unsigned char)p[1] << 16
        | (quint32)(unsigned char)p[2] << 8 | (quint32)(unsigned char)p[3];
}

QByteArray zlibDecompress(const QByteArray &in, quint32 expected)
{
    QByteArray out(qMax(1, (int)expected), Qt::Uninitialized);
    uLongf destLen = expected;
    int rc = ::uncompress((unsigned char *)out.data(), &destLen,
                          (const unsigned char *)in.constData(), in.size());
    if (rc != Z_OK)
        return QByteArray();
    out.resize((int)destLen);
    return out;
}

} // namespace

quint16 FontDecoder::be16(const char *p)
{
    return static_cast<quint16>((unsigned char)p[0] << 8 | (unsigned char)p[1]);
}

quint32 FontDecoder::be32(const char *p)
{
    return (quint32)(unsigned char)p[0] << 24 | (quint32)(unsigned char)p[1] << 16
        | (quint32)(unsigned char)p[2] << 8 | (quint32)(unsigned char)p[3];
}

bool FontDecoder::readWoffOrSfnt(const QByteArray &blob, QList<Table> *tables)
{
    if (blob.size() < 8)
        return false;
    const quint32 signature = be32At(blob, 0);

    if (signature == 0x774F4646u) { // 'wOFF'
        if (blob.size() < 44)
            return false;
        const int numTables = be16At(blob, 12);
        for (int i = 0; i < numTables; ++i) {
            const int e = 44 + i * 20;
            if (e + 20 > blob.size())
                return false;
            QByteArray tag = blob.mid(e, 4);
            const quint32 doff = be32At(blob, e + 4);
            const quint32 compLen = be32At(blob, e + 8);
            const quint32 origLen = be32At(blob, e + 12);
            if (doff + compLen > (quint32)blob.size())
                return false;
            QByteArray data = blob.mid(doff, compLen);
            if (compLen != origLen) {
                data = zlibDecompress(data, origLen);
                if (data.isEmpty())
                    return false;
            }
            tables->append({tag, data});
        }
        return true;
    }

    if (signature == 0x00010000u /* '\\0\\1\\0\\0' */ || signature == 0x4F54544Fu /* 'OTTO' */
        || signature == 0x74727565u /* 'true' */) {
        const int numTables = be16At(blob, 4);
        for (int i = 0; i < numTables; ++i) {
            const int e = 12 + i * 16;
            if (e + 16 > blob.size())
                return false;
            QByteArray tag = blob.mid(e, 4);
            const quint32 toff = be32At(blob, e + 8);
            const quint32 len = be32At(blob, e + 12);
            if (toff + len > (quint32)blob.size())
                return false;
            tables->append({tag, blob.mid(toff, len)});
        }
        return true;
    }

    return false;
}

bool FontDecoder::parseCmap(const QByteArray &cmap, QMap<quint32, quint16> *out)
{
    if (cmap.size() < 4)
        return false;
    const int numTables = be16At(cmap, 2);

    int bestOffset = -1;
    int bestFormat = -1;
    for (int i = 0; i < numTables; ++i) {
        const int e = 4 + i * 8;
        if (e + 8 > cmap.size())
            return false;
        const quint16 platform = be16At(cmap, e);
        const quint16 encoding = be16At(cmap, e + 2);
        const quint32 offset = be32At(cmap, e + 4);
        const bool unicode = platform == 0 || (platform == 3 && (encoding == 10 || encoding == 1));
        if (!unicode || offset + 2 > (quint32)cmap.size())
            continue;
        const int format = be16At(cmap, offset);
        if (format == 12 && bestFormat < 12) {
            bestFormat = 12;
            bestOffset = offset;
        } else if (format == 4 && bestFormat < 4) {
            bestFormat = 4;
            bestOffset = offset;
        }
    }
    if (bestOffset < 0)
        return false;

    const char *p = cmap.constData() + bestOffset;
    if (bestFormat == 12) {
        const int nGroups = be32(p + 12);
        for (int i = 0; i < nGroups; ++i) {
            const int g = 16 + i * 12;
            const quint32 start = be32(p + g);
            const quint32 end = be32(p + g + 4);
            const quint32 startGlyph = be32(p + g + 8);
            for (quint32 c = start; c <= end; ++c)
                out->insert(c, startGlyph + (c - start));
        }
        return true;
    }

    // format 4
    const int segCountX2 = be16(p + 6);
    const int segCount = segCountX2 / 2;
    const int endCodeOff = 14;
    const int startCodeOff = endCodeOff + segCountX2 + 2;
    const int idDeltaOff = startCodeOff + segCountX2;
    const int idRangeOffOff = idDeltaOff + segCountX2;
    const int glyphArrayOff = idRangeOffOff + segCountX2;
    Q_UNUSED(glyphArrayOff)
    for (int s = 0; s < segCount; ++s) {
        const quint16 end = be16(p + endCodeOff + s * 2);
        const quint16 start = be16(p + startCodeOff + s * 2);
        if (end < start)
            continue;
        const qint32 idDelta = be16(p + idDeltaOff + s * 2);
        const quint16 idRangeOffset = be16(p + idRangeOffOff + s * 2);
        for (quint32 c = start; c <= end; ++c) {
            quint16 glyph;
            if (idRangeOffset == 0) {
                glyph = (quint16)(c + static_cast<quint32>(idDelta));
            } else {
                const int gaddr = idRangeOffOff + s * 2 + idRangeOffset + (c - start) * 2;
                if (bestOffset + gaddr + 2 > cmap.size())
                    continue;
                glyph = be16At(cmap, bestOffset + gaddr);
                glyph = (quint16)(static_cast<int>(glyph) + idDelta);
            }
            out->insert(c, glyph);
        }
    }
    return true;
}

bool FontDecoder::parsePost(const Table &sfnt, QHash<quint16, QString> *byGlyphIndex)
{
    const QByteArray &d = sfnt.data;
    if (d.size() < 34)
        return false;
    const quint32 format = be32At(d, 0);
    if (format != 0x00020000u)
        return false;

    const quint16 numGlyphs = be16At(d, 32);
    QStringList names;
    for (int i = 0; i < numGlyphs; ++i) {
        const quint16 idx = be16At(d, 34 + i * 2);
        if (idx < 258) {
            names.append(QString::fromLatin1(kMacStandardGlyphNames[idx]));
        } else {
            int p = 34 + numGlyphs * 2;
            int cur = 0;
            QString name;
            while (p < d.size() && cur <= idx - 258) {
                const int len = (unsigned char)d.at(p);
                QByteArray raw = d.mid(p + 1, len);
                name = QString::fromLatin1(raw.constData(), raw.size());
                p += 1 + len;
                ++cur;
            }
            names.append(name);
        }
    }
    if (names.size() < numGlyphs)
        return false;
    for (int g = 0; g < numGlyphs; ++g)
        byGlyphIndex->insert(g, names.at(g));
    return true;
}

bool FontDecoder::loadFont(const QByteArray &fontData)
{
    m_ready = false;
    m_plain.clear();
    m_error.clear();

    QList<Table> tables;
    if (!readWoffOrSfnt(fontData, &tables)) {
        m_error = QStringLiteral("Unsupported or invalid font (only WOFF/WOFF2-free SFNT accepted)");
        return false;
    }

    QByteArray cmapData;
    const Table *post = nullptr;
    for (const Table &t : tables) {
        if (t.tag == "cmap")
            cmapData = t.data;
        else if (t.tag == "post")
            post = &t;
    }
    if (cmapData.isEmpty() || !post) {
        m_error = QStringLiteral("Font is missing cmap or post table");
        return false;
    }

    QMap<quint32, quint16> cmap;
    if (!parseCmap(cmapData, &cmap)) {
        m_error = QStringLiteral("Unable to parse cmap table");
        return false;
    }

    QHash<quint16, QString> glyphNames;
    if (!parsePost(*post, &glyphNames)) {
        m_error = QStringLiteral("Unable to parse post table (glyph names unavailable)");
        return false;
    }
    m_cmapDbg = cmap;
    m_nameDbg = glyphNames;

    const QHash<QString, QString> agl = aglTable();
    for (auto it = cmap.cbegin(); it != cmap.cend(); ++it) {
        const QString glyph = glyphNames.value(it.value());
        if (glyph.isEmpty())
            continue;
        QString plain = agl.value(glyph);
        if (plain.isEmpty() && glyph.startsWith(QLatin1String("uni")) && glyph.size() == 7) {
            bool ok = false;
            const quint32 cp = glyph.mid(3).toUInt(&ok, 16);
            if (ok)
                plain = QString(QChar(cp));
        }
        if (!plain.isEmpty())
            m_plain.insert(it.key(), plain);
    }

    m_ready = true;
    return true;
}

QString FontDecoder::decodeText(const QString &pua) const
{
    if (!m_ready || pua.isEmpty())
        return pua;
    QString out;
    out.reserve(pua.size());
    for (int i = 0; i < pua.size(); ++i) {
        const QChar c = pua.at(i);
        quint32 cp = c.unicode();
        if (c.isHighSurrogate() && i + 1 < pua.size() && pua.at(i + 1).isLowSurrogate()) {
            cp = QChar::surrogateToUcs4(c, pua.at(i + 1));
            const QString repl = m_plain.value(cp);
            if (!repl.isEmpty()) {
                out.append(repl);
                ++i;
                continue;
            }
        }
        if (cp >= 0xE000 && cp <= 0xF8FF) {
            const QString repl = m_plain.value(cp);
            out.append(repl.isEmpty() ? c : repl);
        } else {
            out.append(c);
        }
    }
    return out;
}

QJsonValue FontDecoder::decodeJson(const QJsonValue &value, const FontDecoder &dec)
{
    switch (value.type()) {
    case QJsonValue::Object: {
        QJsonObject obj;
        const QJsonObject src = value.toObject();
        for (auto it = src.begin(); it != src.end(); ++it)
            obj.insert(it.key(), decodeJson(it.value(), dec));
        return obj;
    }
    case QJsonValue::Array: {
        QJsonArray arr;
        const QJsonArray src = value.toArray();
        for (const QJsonValue &v : src)
            arr.append(decodeJson(v, dec));
        return arr;
    }
    case QJsonValue::String:
        return dec.decodeText(value.toString());
    default:
        return value;
    }
}