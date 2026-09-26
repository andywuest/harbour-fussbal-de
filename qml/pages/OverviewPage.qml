import QtQuick 2.2
import Sailfish.Silica 1.0
import "../components"
import "../components/thirdparty"

Page {
    id: page

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    property string competitionName
    property int currentMatchDay: settings.currentMatchDay
    property int maxMatchDay: 0
    property bool loaded : false

    function loadMatchDay(matchDayIndex) {
        loaded = false;
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

    function localLogo(logoUrl) {
        if (!logoUrl)
            return ""
        var localUrl = fussballBackend.cachedLogoUrl(logoUrl)
        if (localUrl.length === 0)
            fussballBackend.cacheLogo(logoUrl)
        return localUrl
    }

    function updateLogo(logoUrl, localUrl) {
        for (var i = 0; i < gameDayModel.count; ++i) {
            var match = gameDayModel.get(i)
            if (match.homeTeamLogoSource === logoUrl)
                gameDayModel.setProperty(i, "homeTeamLogoUrl", localUrl)
            if (match.awayTeamLogoSource === logoUrl)
                gameDayModel.setProperty(i, "awayTeamLogoUrl", localUrl)
        }
    }

    function applyResult(decodedJson) {
        // console.log("Game day result: " + JSON.stringify(decodedJson))

        var pageProps = decodedJson.pageProps ? decodedJson.pageProps : {}
        competitionName = pageProps.competitionName ? pageProps.competitionName : ""
        var matchDays = pageProps.matchDays ? pageProps.matchDays : []
        maxMatchDay = matchDays.length
        currentMatchDay = pageProps.currentMatchDay > 0 ? (pageProps.currentMatchDay - 1) : 0

        var matches = pageProps.matches ? pageProps.matches : []
        gameDayModel.clear()
        for (var i = 0; i < matches.length; ++i) {
            var match = matches[i]
            var home = match.homeTeam ? match.homeTeam : {}
            var guest = match.guestTeam ? match.guestTeam : {}
            var kickoff = match.kickoff ? match.kickoff : {}
            var homeLogoUrl = home.clubLogoURL ? home.clubLogoURL : ""
            var awayLogoUrl = guest.clubLogoURL ? guest.clubLogoURL : ""
            gameDayModel.append({
                matchDate: (kickoff.dateWithWeekday ? kickoff.dateWithWeekday + " " : "") + (kickoff.time ? kickoff.time : ""),
                homeTeamName: home.name ? home.name : "",
                homeTeamLogo: home.name ? home.name[0] : "",
                homeTeamLogoSource: homeLogoUrl,
                homeTeamLogoUrl: localLogo(homeLogoUrl),
                homeTeamLogoColor: logoColor(home.name ? home.name : ""),
                awayTeamName: guest.name ? guest.name : "",
                awayTeamLogo: guest.name ? guest.name[0] : "",
                awayTeamLogoSource: awayLogoUrl,
                awayTeamLogoUrl: localLogo(awayLogoUrl),
                awayTeamLogoColor: logoColor(guest.name ? guest.name : ""),
                homeGoals: match.result && match.result.homeResult ? parseInt(match.result.homeResult) : 0,
                awayGoals: match.result && match.result.guestResult ? parseInt(match.result.guestResult) : 0
            })
        }
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
                title: qsTr("Spieltag %1").arg(page.currentMatchDay + 1)
                description: page.competitionName
            }

        }

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
        PullDownMenu {
            MenuItem {
                //: OverviewPage about menu item
                text: qsTr("About")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }
            MenuItem {
                text: qsTr("Settings")
                onClicked: pageStack.animatorPush(Qt.resolvedUrl("SettingsPage.qml"))
            }
            MenuItem {
                text: qsTr("Table")
                onClicked: pageStack.animatorPush(Qt.resolvedUrl("TablePage.qml"))
            }
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

    LoadingIndicator {
        id: dataLoadingIndicator
        visible: !loaded
        Behavior on opacity {
            NumberAnimation {
            }
        }
        opacity: loaded ? 0 : 1
        height: parent.height
        width: parent.width
    }

    Connections {
        target: fussballBackend
        onResultReady: {
            page.applyResult(decodedJson)
            loaded = true;
        }
        onLoadFailed: {
            console.log("FussballBackend error: " + error)
            loaded = true;
        }
        onLogoReady: page.updateLogo(logoUrl, localFileUrl)
    }

    Component.onCompleted: {
        page.loadMatchDay(settings.currentMatchDay)
    }

}
