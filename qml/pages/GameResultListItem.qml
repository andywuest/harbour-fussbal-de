import QtQuick 2.0
import Sailfish.Silica 1.0

ListItem {
    id: root

    property string matchDate
    property string homeTeamName
    property string homeTeamLogo
    property color homeTeamLogoColor: Theme.highlightColor
    property string awayTeamName
    property string awayTeamLogo
    property color awayTeamLogoColor: Theme.secondaryHighlightColor
    property int homeGoals: 0
    property int awayGoals: 0

    contentHeight: contentColumn.height

    Column {
        id: contentColumn
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: Theme.horizontalPageMargin
        anchors.rightMargin: Theme.horizontalPageMargin
        spacing: Theme.paddingSmall

        Label {
            id: dateLabel
            text: root.matchDate
            color: Theme.secondaryColor
            font.pixelSize: Theme.fontSizeSmall
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Item {
            id: resultRow
            width: parent.width
            height: Math.max(homeColumn.height, awayColumn.height)

            Column {
                id: homeColumn
                width: resultRow.width / 2
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                spacing: Theme.paddingSmall

                Rectangle {
                    id: homeLogo
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: Theme.iconSizeMedium
                    height: width
                    radius: width / 2
                    color: root.homeTeamLogoColor

                    Label {
                        anchors.centerIn: parent
                        text: root.homeTeamLogo
                        color: Theme.primaryColor
                        font.pixelSize: Theme.fontSizeSmall
                        font.bold: true
                    }
                }

                Label {
                    text: root.homeTeamName
                    anchors.horizontalCenter: parent.horizontalCenter
                    maximumLineCount: 1
                    truncationMode: TruncationMode.Elide
                    font.pixelSize: Theme.fontSizeSmall
                    color: root.highlighted ? Theme.highlightColor : Theme.primaryColor
                }
            }

            Column {
                id: awayColumn
                width: resultRow.width / 2
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                spacing: Theme.paddingSmall

                Rectangle {
                    id: awayLogo
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: Theme.iconSizeMedium
                    height: width
                    radius: width / 2
                    color: root.awayTeamLogoColor

                    Label {
                        anchors.centerIn: parent
                        text: root.awayTeamLogo
                        color: Theme.primaryColor
                        font.pixelSize: Theme.fontSizeSmall
                        font.bold: true
                    }
                }

                Label {
                    text: root.awayTeamName
                    anchors.horizontalCenter: parent.horizontalCenter
                    maximumLineCount: 1
                    truncationMode: TruncationMode.Elide
                    font.pixelSize: Theme.fontSizeSmall
                    color: root.highlighted ? Theme.highlightColor : Theme.primaryColor
                }
            }

            Label {
                id: scoreLabel
                text: root.homeGoals + " : " + root.awayGoals
                anchors.centerIn: parent
                font.pixelSize: Theme.fontSizeLarge
                font.bold: true
                color: root.highlighted ? Theme.highlightColor : Theme.highlightFromColor(Theme.primaryColor, 0.15)
            }
        }
    }

    onClicked: console.log("Game: " + root.homeTeamName + " " + root.homeGoals
                           + " : " + root.awayGoals + " " + root.awayTeamName)
}