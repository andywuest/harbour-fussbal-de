import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: page

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    property string competitionName
    property int currentMatchDay: settings.currentMatchDay
    property int maxMatchDay: 0

    function loadMatchDay(matchDayIndex) {
        page.currentMatchDay = matchDayIndex
        settings.currentMatchDay = matchDayIndex
        settings.sync()
        fussballBackend.getMatchDay(matchDayIndex + 1)
    }

    function logoColor(name) {
        var palette = ["#4a90d9", "#3cb371", "#d9a441", "#b03a3a", "#8e44ad", "#d35400",
                       "#2980b9", "#16a085", "#c0392b", "#3498db", "#1abc9c", "#e67e22"]
        var hash = 0
        for (var i = 0; i < name.length; ++i)
            hash = (hash * 31 + name.charCodeAt(i)) % 0xFFFFFF
        return palette[hash % palette.length]
    }

    function applyTable(decodedJson) {
        console.log("Table result: " + JSON.stringify(decodedJson))

        var pageProps = decodedJson.pageProps ? decodedJson.pageProps : {}
        competitionName = pageProps.competitionName ? pageProps.competitionName : ""
        var matchDays = pageProps.matchDays ? pageProps.matchDays : []
        maxMatchDay = matchDays.length

        var entries = pageProps.table && pageProps.table.entries ? pageProps.table.entries : []
        tableModel.clear()
        for (var i = 0; i < entries.length; ++i) {
            var entry = entries[i]
            tableModel.append({
                position: entry.position ? entry.position : "",
                teamName: entry.teamName ? entry.teamName : "",
                teamLogo: entry.teamName ? entry.teamName[0] : "",
                teamLogoUrl: entry.clubLogoURL ? entry.clubLogoURL : "",
                teamLogoColor: logoColor(entry.teamName ? entry.teamName : ""),
                matches: entry.matches ? entry.matches : "0",
                won: entry.matchesWon ? entry.matchesWon : "0",
                drawn: entry.matchesDrawn ? entry.matchesDrawn : "0",
                lost: entry.matchesLost ? entry.matchesLost : "0",
                goalRatio: entry.goalRatio ? entry.goalRatio : "0:0",
                goalDifference: entry.goalDifference ? entry.goalDifference : "0",
                points: entry.points ? entry.points : "0"
            })
        }
    }

    Connections {
        target: fussballBackend
        onResultReady: page.applyTable(decodedJson)
        onLoadFailed: console.log("FussballBackend error: " + error)
    }

    Component.onCompleted: {
        page.loadMatchDay(settings.currentMatchDay)
    }

    SilicaListView {
        id: listView
        anchors.fill: parent

        ListModel {
            id: tableModel
        }

        model: tableModel

        header: Column {
            width: listView.width

            PageHeader {
                title: qsTr("Tabelle - Spieltag %1").arg(page.currentMatchDay + 1)
                description: page.competitionName
            }
        }

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
        PullDownMenu {
            MenuItem {
                visible: page.currentMatchDay > 0
                text: qsTr("Previous match day")
                onClicked: page.loadMatchDay(page.currentMatchDay - 1)
            }
            MenuItem {
                visible: page.currentMatchDay < page.maxMatchDay - 1
                text: qsTr("Next match day")
                onClicked: page.loadMatchDay(page.currentMatchDay + 1)
            }
        }

        delegate: TableListItem {
            position: model.position
            teamName: model.teamName
            teamLogo: model.teamLogo
            teamLogoUrl: model.teamLogoUrl
            teamLogoColor: model.teamLogoColor
            matches: model.matches
            won: model.won
            drawn: model.drawn
            lost: model.lost
            goalRatio: model.goalRatio
            goalDifference: model.goalDifference
            points: model.points
        }

        VerticalScrollDecorator {}
    }
}