import QtQuick 2.9
import QtQuick.Window 2.2
import QtLocation 5.5
import QtPositioning 5.5

    PositionSource {
        id: src
        updateInterval: 1000
        active: true

        onPositionChanged: {
            var coord = src.position.coordinate;
            //console.debug("current position:", coord.latitude, coord.longitude);
        }
    }
