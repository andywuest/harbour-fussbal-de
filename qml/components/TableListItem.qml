import QtQuick 2.0
import Sailfish.Silica 1.0

ListItem {
    id: root

    property string position
    property string teamName
    property string teamLogo
    property string teamLogoUrl
    property color teamLogoColor: Theme.highlightColor
    property string matches: "0"
    property string won: "0"
    property string drawn: "0"
    property string lost: "0"
    property string goalRatio: "0:0"
    property string goalDifference: "0"
    property string points: "0"

    contentHeight: row.height + (2 * Theme.paddingSmall)

    Row {
        id: row
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: Theme.horizontalPageMargin
        anchors.rightMargin: Theme.horizontalPageMargin
        anchors.verticalCenter: parent.verticalCenter
        spacing: Theme.paddingSmall

        Label {
            id: positionLabel
            anchors.verticalCenter: parent.verticalCenter
            width: Theme.fontSizeSmall * 1.8
            text: root.position
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: Theme.fontSizeSmall
            font.bold: true
            color: root.highlighted ? Theme.highlightColor : Theme.primaryColor
        }

        Item {
            id: teamColumn
            anchors.verticalCenter: parent.verticalCenter
            width: row.width - positionLabel.width - matchesLabel.width - wdlLabel.width
                   - goalsLabel.width - pointsLabel.width - row.spacing * 5
            height: Math.max(teamLogoItem.width, teamNameLabel.height)

            Item {
                id: teamLogoItem
                anchors.verticalCenter: parent.verticalCenter
                width: Theme.iconSizeSmall
                height: width

                Rectangle {
                    id: teamLogoFallback
                    anchors.fill: parent
                    radius: width / 2
                    color: root.teamLogoColor
                    visible: root.teamLogoUrl.length === 0

                    Label {
                        anchors.centerIn: parent
                        text: root.teamLogo
                        color: Theme.primaryColor
                        font.pixelSize: Theme.fontSizeSmall
                        font.bold: true
                    }
                }

                Image {
                    anchors.fill: parent
                    source: root.teamLogoUrl
                    visible: root.teamLogoUrl.length > 0
                    fillMode: Image.PreserveAspectFit
                    sourceSize.width: width
                    sourceSize.height: height
                }
            }

            Label {
                id: teamNameLabel
                anchors.left: teamLogoItem.right
                anchors.right: parent.right
                anchors.leftMargin: Theme.paddingSmall
                anchors.rightMargin: Theme.paddingSmall
                anchors.verticalCenter: parent.verticalCenter
                text: root.teamName
                verticalAlignment: Text.AlignVCenter
                maximumLineCount: 1
                truncationMode: TruncationMode.Elide
                font.pixelSize: Theme.fontSizeSmall
                color: root.highlighted ? Theme.highlightColor : Theme.primaryColor
            }
        }

        Label {
            id: matchesLabel
            anchors.verticalCenter: parent.verticalCenter
            width: Theme.fontSizeSmall * 1.8
            text: root.matches
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: Theme.fontSizeSmall
            color: root.highlighted ? Theme.highlightColor : Theme.secondaryColor
        }

        Label {
            id: wdlLabel
            anchors.verticalCenter: parent.verticalCenter
            width: Theme.fontSizeSmall * 3.4
            text: root.won + "-" + root.drawn + "-" + root.lost
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: Theme.fontSizeSmall
            color: root.highlighted ? Theme.highlightColor : Theme.secondaryColor
        }

        Label {
            id: goalsLabel
            anchors.verticalCenter: parent.verticalCenter
            width: Theme.fontSizeSmall * 4.0
            text: root.goalRatio
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: Theme.fontSizeSmall
            color: root.highlighted ? Theme.highlightColor : Theme.secondaryColor
        }

        Label {
            id: pointsLabel
            anchors.verticalCenter: parent.verticalCenter
            width: Theme.fontSizeSmall * 1.8
            text: root.points
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: Theme.fontSizeSmall
            font.bold: true
            color: root.highlighted ? Theme.highlightColor : Theme.primaryColor
        }
    }

    onClicked: console.log("Table position " + root.position + ": " + root.teamName)
}