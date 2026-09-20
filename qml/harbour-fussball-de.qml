import QtQuick 2.0
import Sailfish.Silica 1.0
import Nemo.Configuration 1.0
import "pages"

ApplicationWindow {
    initialPage: Component { OverviewPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations

    property alias competitionId: competitionIdSetting.value

    ConfigurationGroup {
        id: settings
        path: "/harbour-fussball-de"

        ConfigurationValue {
            id: competitionIdSetting
            key: "competitionId"
            value: ""
        }
    }
}