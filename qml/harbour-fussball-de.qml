import QtQuick 2.2
import Sailfish.Silica 1.0
import Nemo.Configuration 1.0
import "pages"

ApplicationWindow {
    initialPage: Component { OverviewPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations

//     property alias competitionId: competitionIdSetting.value

    ConfigurationGroup {
        id: settings
        path: "/apps/harbour-fussball-de/settings"

        property string competitionId: ""
        property int currentMatchDay: 0
    }
}
