import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    // To enable PullDownMenu, place our content in a SilicaFlickable
    SilicaFlickable {
        id: flickable
        anchors.fill: parent

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
        PullDownMenu {
            MenuItem {
                text: qsTr("Show Page 2")
                onClicked: pageStack.animatorPush(Qt.resolvedUrl("SecondPage.qml"))
            }
        }

        // Tell SilicaFlickable the height of its content.
        contentHeight: column.height

        ListModel {
            id: gameDayModel

            ListElement {
                matchDate: "Sa, 25.01.2026, 15:30"
                homeTeamName: "FC Küstenwanderer"
                homeTeamLogo: "KW"
                homeTeamLogoColor: "#4a90d9"
                awayTeamName: "SV Wellenbändiger"
                awayTeamLogo: "WB"
                awayTeamLogoColor: "#d97742"
                homeGoals: 2
                awayGoals: 1
            }
            ListElement {
                matchDate: "Sa, 25.01.2026, 15:30"
                homeTeamName: "TSV Hafenkicker"
                homeTeamLogo: "HK"
                homeTeamLogoColor: "#3cb371"
                awayTeamName: "FC Nordwind"
                awayTeamLogo: "NW"
                awayTeamLogoColor: "#4682b4"
                homeGoals: 0
                awayGoals: 0
            }
            ListElement {
                matchDate: "Sa, 25.01.2026, 15:30"
                homeTeamName: "Sportfreunde Düne"
                homeTeamLogo: "SD"
                homeTeamLogoColor: "#d9a441"
                awayTeamName: "Rot-Weiß Fähre"
                awayTeamLogo: "RF"
                awayTeamLogoColor: "#b03a3a"
                homeGoals: 3
                awayGoals: 2
            }
            ListElement {
                matchDate: "Sa, 25.01.2026, 18:00"
                homeTeamName: "SV Uferrand"
                homeTeamLogo: "UR"
                homeTeamLogoColor: "#8e44ad"
                awayTeamName: "BSG Leuchtturm"
                awayTeamLogo: "LT"
                awayTeamLogoColor: "#2c3e50"
                homeGoals: 1
                awayGoals: 1
            }
            ListElement {
                matchDate: "Sa, 25.01.2026, 18:00"
                homeTeamName: "FC Deichgrafen"
                homeTeamLogo: "DG"
                homeTeamLogoColor: "#27ae60"
                awayTeamName: "TuS Molenbrüder"
                awayTeamLogo: "MB"
                awayTeamLogoColor: "#d35400"
                homeGoals: 0
                awayGoals: 2
            }
            ListElement {
                matchDate: "So, 26.01.2026, 13:00"
                homeTeamName: "Borussia Watt"
                homeTeamLogo: "BW"
                homeTeamLogoColor: "#e67e22"
                awayTeamName: "VfB Südwind"
                awayTeamLogo: "SW"
                awayTeamLogoColor: "#2980b9"
                homeGoals: 4
                awayGoals: 1
            }
            ListElement {
                matchDate: "So, 26.01.2026, 13:00"
                homeTeamName: "SC Hansestadt"
                homeTeamLogo: "HS"
                homeTeamLogoColor: "#e74c3c"
                awayTeamName: "SV Muschelberg"
                awayTeamLogo: "MG"
                awayTeamLogoColor: "#16a085"
                homeGoals: 1
                awayGoals: 3
            }
            ListElement {
                matchDate: "So, 26.01.2026, 15:30"
                homeTeamName: "FC Seepferdchen"
                homeTeamLogo: "SP"
                homeTeamLogoColor: "#7f8c8d"
                awayTeamName: "Union Leuchtboje"
                awayTeamLogo: "LB"
                awayTeamLogoColor: "#c0392b"
                homeGoals: 2
                awayGoals: 2
            }
            ListElement {
                matchDate: "So, 26.01.2026, 15:30"
                homeTeamName: "TSV Landspitze"
                homeTeamLogo: "LS"
                homeTeamLogoColor: "#3498db"
                awayTeamName: "VfL Novembrum"
                awayTeamLogo: "NV"
                awayTeamLogoColor: "#95a5a6"
                homeGoals: 0
                awayGoals: 1
            }
            ListElement {
                matchDate: "So, 26.01.2026, 18:00"
                homeTeamName: "Fortuna Ankerplatz"
                homeTeamLogo: "FA"
                homeTeamLogoColor: "#8e44ad"
                awayTeamName: "SpVg Nebelschein"
                awayTeamLogo: "NS"
                awayTeamLogoColor: "#1abc9c"
                homeGoals: 3
                awayGoals: 3
            }
        }

        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Spieltag 18")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - (2 * Theme.horizontalPageMargin)
                text: qsTr("Ergebnisse vom 25./26. Januar 2026")
                color: Theme.secondaryHighlightColor
                font.pixelSize: Theme.fontSizeSmall
            }

            Repeater {
                model: gameDayModel
                delegate: GameResultListItem {
                    matchDate: model.matchDate
                    homeTeamName: model.homeTeamName
                    homeTeamLogo: model.homeTeamLogo
                    homeTeamLogoColor: model.homeTeamLogoColor
                    awayTeamName: model.awayTeamName
                    awayTeamLogo: model.awayTeamLogo
                    awayTeamLogoColor: model.awayTeamLogoColor
                    homeGoals: model.homeGoals
                    awayGoals: model.awayGoals
                }
            }
        }
    }
}