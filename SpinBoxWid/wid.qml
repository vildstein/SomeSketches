import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
	id : parentRect
	width: 200
	height: 200
	color: "silver"
	border.color: "black"
	border.width: 2
	radius: 5

	onWidthChanged : console.log(width)
	onHeightChanged : console.log(height)

	property alias parentRectWidth:  parentRect.width
	property alias parentRectHeight: parentRect.height

	signal shitChanged(width: int, height: int);
	signal startScanning()

	Text {
		id : header

		x: 3
		y: 10

		text: "Scanning"
		font.family : qsTr("Segoe UI")
		font.bold: true
		font.pointSize: 10
	}

	Rectangle {
		id : startScanRect // Заменить на Item
		border.color: "black"

		width: parentRect.width - 6
		height: 30

		x: 3
		y: 35

		Text {
			id : startScanText
			text: "Run mode"
			font.family : qsTr("Segoe UI")
			font.bold: false
			font.pointSize: 9

			anchors.verticalCenter: startScanRect.verticalCenter
			anchors.left: startScanRect.left
			anchors.leftMargin: 5
		}

		Button {
			id : startScanBtn
			width : 285
			height: 25
			text: qsTr("Start Scanning")

			anchors.verticalCenter: startScanRect.verticalCenter
			anchors.left: startScanText.right
			anchors.leftMargin: 10

			onClicked: parentRect.startScanning()

		}

		Button {
			id : scanningSettingsBtn
			width : 26
			height: 26

			anchors.verticalCenter: startScanRect.verticalCenter
			anchors.left: startScanBtn.right
			anchors.leftMargin: 8

			onClicked: parentRect.shitChanged(parentRect.parentRectWidth, parentRect.parentRectHeight)
		}
	}

	Canvas {

		id : headerLine

		width: 100
		height: 50

		onPaint: {
			var drawContext = getContext("2d");
			//drawContext.fillStyle = Qt.rgba(1, 0, 0, 1);
			//drawContext.fillRect(0, 0, width, height);

			drawContext.lineWidth = 2;
			//drawContext.moveTo(0, 0);
			drawContext.beginPath()
			drawContext.lineTo(width, height);
			drawContext.closePath()
		}
	}
}

