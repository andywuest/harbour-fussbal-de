import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: page

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    property string competitionName
    property int currentMatchDay: 0

    function logoColor(name) {
        var palette = ["#4a90d9", "#3cb371", "#d9a441", "#b03a3a", "#8e44ad", "#d35400",
                       "#2980b9", "#16a085", "#c0392b", "#3498db", "#1abc9c", "#e67e22"]
        var hash = 0
        for (var i = 0; i < name.length; ++i)
            hash = (hash * 31 + name.charCodeAt(i)) % 0xFFFFFF
        return palette[hash % palette.length]
    }

    function applyResult(decodedJson) {
        console.log("Game day result: " + JSON.stringify(decodedJson))

        var pageProps = decodedJson.pageProps ? decodedJson.pageProps : {}
        competitionName = pageProps.competitionName ? pageProps.competitionName : ""
        currentMatchDay = pageProps.currentMatchDay ? pageProps.currentMatchDay : 0

        var matches = pageProps.matches ? pageProps.matches : []
        gameDayModel.clear()
        for (var i = 0; i < matches.length; ++i) {
            var match = matches[i]
            var home = match.homeTeam ? match.homeTeam : {}
            var guest = match.guestTeam ? match.guestTeam : {}
            var kickoff = match.kickoff ? match.kickoff : {}
            gameDayModel.append({
                matchDate: (kickoff.dateWithWeekday ? kickoff.dateWithWeekday + " " : "") + (kickoff.time ? kickoff.time : ""),
                homeTeamName: home.name ? home.name : "",
                homeTeamLogo: home.name ? home.name[0] : "",
                homeTeamLogoUrl: home.clubLogoURL ? home.clubLogoURL : "",
                homeTeamLogoColor: logoColor(home.name ? home.name : ""),
                awayTeamName: guest.name ? guest.name : "",
                awayTeamLogo: guest.name ? guest.name[0] : "",
                awayTeamLogoUrl: guest.clubLogoURL ? guest.clubLogoURL : "",
                awayTeamLogoColor: logoColor(guest.name ? guest.name : ""),
                homeGoals: match.result && match.result.homeResult ? parseInt(match.result.homeResult) : 0,
                awayGoals: match.result && match.result.guestResult ? parseInt(match.result.guestResult) : 0
            })
        }
    }

    Connections {
        target: fussballBackend
        onResultReady: page.applyResult(decodedJson)
        onLoadFailed: console.log("FussballBackend error: " + error)
    }

    Component.onCompleted: {
        fussballBackend.getMatchDay(2)
    }

    SilicaListView {
        id: listView
        anchors.fill: parent

        ListModel {
            id: gameDayModel
        }

        model: gameDayModel

        header: Column {
            width: listView.width

            PageHeader {
                title: qsTr("Spieltag %1").arg(page.currentMatchDay > 0 ? page.currentMatchDay : "")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - (2 * Theme.horizontalPageMargin)
                text: page.competitionName
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
            matchDate: model.matchDate
            homeTeamName: model.homeTeamName
            homeTeamLogo: model.homeTeamLogo
            homeTeamLogoUrl: model.homeTeamLogoUrl
            homeTeamLogoColor: model.homeTeamLogoColor
            awayTeamName: model.awayTeamName
            awayTeamLogo: model.awayTeamLogo
            awayTeamLogoUrl: model.awayTeamLogoUrl
            awayTeamLogoColor: model.awayTeamLogoColor
            homeGoals: model.homeGoals
            awayGoals: model.awayGoals
        }

        VerticalScrollDecorator {}
    }
}
