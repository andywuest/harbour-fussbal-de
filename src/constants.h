#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

// sailjail data
const char APP_NAME[] = "harbour-watchlist";
const char ORGANISATION[] = "de.andreas-wuest-it-consulting";

// Endpoint that delivers the JSON payload of one game day (match day 2 shown).
const char GameResultsEndpoint[] = "https://next.fussball.de/_next/data/3GE9wzufdQY_tRu-Qkpfs/de/widget/competition/ac5e71aa-1ee8-4579-9508-1a2f27fe240d/spieltag/2.json";

// Build the endpoint URL for a given match day.
inline QString gameResultsEndpoint(int matchDay)
{
    return QStringLiteral("https://next.fussball.de/_next/data/3GE9wzufdQY_tRu-Qkpfs/de/widget/"
                          "competition/ac5e71aa-1ee8-4579-9508-1a2f27fe240d/spieltag/%1.json")
        .arg(matchDay);
}

#endif // CONSTANTS_H
