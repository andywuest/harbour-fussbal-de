import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: page

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    Connections {
        target: fussballBackend
        onResultReady: console.log("Game day result: " + JSON.stringify(decodedJson))
        onLoadFailed: console.log("FussballBackend error: " + error)
    }

    Component.onCompleted: {
        fussballBackend.getMatchDay(2)
    }

    SilicaListView {
        id: listView
        anchors.fill: parent

        // JavaScript model with mock data for 10 made-up matches.
        model: [
            {
                matchDate: "Sa, 25.01.2026, 15:30",
                homeTeamName: "FC Küstenwanderer",
                homeTeamLogo: "KW",
                homeTeamLogoColor: "#4a90d9",
                awayTeamName: "SV Wellenbändiger",
                awayTeamLogo: "WB",
                awayTeamLogoColor: "#d97742",
                homeGoals: 2,
                awayGoals: 1
            },
            {
                matchDate: "Sa, 25.01.2026, 15:30",
                homeTeamName: "TSV Hafenkicker",
                homeTeamLogo: "HK",
                homeTeamLogoColor: "#3cb371",
                awayTeamName: "FC Nordwind",
                awayTeamLogo: "NW",
                awayTeamLogoColor: "#4682b4",
                homeGoals: 0,
                awayGoals: 0
            },
            {
                matchDate: "Sa, 25.01.2026, 15:30",
                homeTeamName: "Sportfreunde Düne",
                homeTeamLogo: "SD",
                homeTeamLogoColor: "#d9a441",
                awayTeamName: "Rot-Weiß Fähre",
                awayTeamLogo: "RF",
                awayTeamLogoColor: "#b03a3a",
                homeGoals: 3,
                awayGoals: 2
            },
            {
                matchDate: "Sa, 25.01.2026, 18:00",
                homeTeamName: "SV Uferrand",
                homeTeamLogo: "UR",
                homeTeamLogoColor: "#8e44ad",
                awayTeamName: "BSG Leuchtturm",
                awayTeamLogo: "LT",
                awayTeamLogoColor: "#2c3e50",
                homeGoals: 1,
                awayGoals: 1
            },
            {
                matchDate: "Sa, 25.01.2026, 18:00",
                homeTeamName: "FC Deichgrafen",
                homeTeamLogo: "DG",
                homeTeamLogoColor: "#27ae60",
                awayTeamName: "TuS Molenbrüder",
                awayTeamLogo: "MB",
                awayTeamLogoColor: "#d35400",
                homeGoals: 0,
                awayGoals: 2
            },
            {
                matchDate: "So, 26.01.2026, 13:00",
                homeTeamName: "Borussia Watt",
                homeTeamLogo: "BW",
                homeTeamLogoColor: "#e67e22",
                awayTeamName: "VfB Südwind",
                awayTeamLogo: "SW",
                awayTeamLogoColor: "#2980b9",
                homeGoals: 4,
                awayGoals: 1
            },
            {
                matchDate: "So, 26.01.2026, 13:00",
                homeTeamName: "SC Hansestadt",
                homeTeamLogo: "HS",
                homeTeamLogoColor: "#e74c3c",
                awayTeamName: "SV Muschelberg",
                awayTeamLogo: "MG",
                awayTeamLogoColor: "#16a085",
                homeGoals: 1,
                awayGoals: 3
            },
            {
                matchDate: "So, 26.01.2026, 15:30",
                homeTeamName: "FC Seepferdchen",
                homeTeamLogo: "SP",
                homeTeamLogoColor: "#7f8c8d",
                awayTeamName: "Union Leuchtboje",
                awayTeamLogo: "LB",
                awayTeamLogoColor: "#c0392b",
                homeGoals: 2,
                awayGoals: 2
            },
            {
                matchDate: "So, 26.01.2026, 15:30",
                homeTeamName: "TSV Landspitze",
                homeTeamLogo: "LS",
                homeTeamLogoColor: "#3498db",
                awayTeamName: "VfL Novembrum",
                awayTeamLogo: "NV",
                awayTeamLogoColor: "#95a5a6",
                homeGoals: 0,
                awayGoals: 1
            },
            {
                matchDate: "So, 26.01.2026, 18:00",
                homeTeamName: "Fortuna Ankerplatz",
                homeTeamLogo: "FA",
                homeTeamLogoColor: "#8e44ad",
                awayTeamName: "SpVg Nebelschein",
                awayTeamLogo: "NS",
                awayTeamLogoColor: "#1abc9c",
                homeGoals: 3,
                awayGoals: 3
            }
        ]

        header: Column {
            width: listView.width

            PageHeader {
                title: qsTr("Spieltag 18")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - (2 * Theme.horizontalPageMargin)
                text: qsTr("Ergebnisse vom 25./26. Januar 2026")
                color: Theme.secondaryHighlightColor
                font.pixelSize: Theme.fontSizeSmall
            }
        }

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
        PullDownMenu {
            MenuItem {
                text: qsTr("Settings")
                onClicked: pageStack.animatorPush(Qt.resolvedUrl("SettingsPage.qml"))
            }
        }

        delegate: GameResultListItem {
            matchDate: modelData.matchDate
            homeTeamName: modelData.homeTeamName
            homeTeamLogo: modelData.homeTeamLogo
            homeTeamLogoColor: modelData.homeTeamLogoColor
            awayTeamName: modelData.awayTeamName
            awayTeamLogo: modelData.awayTeamLogo
            awayTeamLogoColor: modelData.awayTeamLogoColor
            homeGoals: modelData.homeGoals
            awayGoals: modelData.awayGoals
        }

        VerticalScrollDecorator {}
    }
}