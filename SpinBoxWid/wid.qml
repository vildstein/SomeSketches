import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
	id : parentRect
	width: 200
	height: 200
	color: "silver"
	border.color: "black"
	border.width: 2
	radius: 15

	onWidthChanged : console.log(width)
	onHeightChanged : console.log(height)

	property alias parentRectWidth:  parentRect.width
	property alias parentRectHeight: parentRect.height

	signal shitChanged(width: int, height: int);
	signal startScanning();
	signal widgetsVisibilityChanged(vis: bool);

	property int betwWidMarg : 13

	Text {
		id : header

		x: 5
		y: 10

		text: qsTr("Scanning")
		font.family : "Segoe UI"
		font.bold: true
		font.pointSize: 10
	}

	Canvas {

	id : headerLine

	anchors.verticalCenter: header.verticalCenter
	anchors.left: header.right
	anchors.leftMargin: 4

	width: parent.width - 88
	height: 3

	onPaint: {

		var drawContext = getContext("2d");
		drawContext.fillStyle = "black";
		drawContext.fillRect(0, 0, width, height);
	}
	}

	Item {
		id : startScanRect

		width: parentRect.width - 6
		height: 30

		x: 3
		anchors.top: header.bottom
		anchors.topMargin: 10

		Text {
			id : startScanText
			width: 80

			text: qsTr("Run mode:")
			font.family : "Segoe UI"
			font.bold: false
			font.pointSize: 9

			anchors.verticalCenter: startScanRect.verticalCenter
			anchors.left: startScanRect.left
			anchors.leftMargin: 5
		}

		Button {
			id : startScanBtn
			width : 280
			height: 25
			text: qsTr("Start Scanning")

			anchors.verticalCenter: startScanRect.verticalCenter
			anchors.left: startScanText.right
			anchors.leftMargin: betwWidMarg

			onClicked: parentRect.startScanning()

		}

		Button {
			id : scanningSettingsBtn
			width : 26
			height: 26

			anchors.verticalCenter: startScanRect.verticalCenter
			anchors.left: startScanBtn.right
			anchors.leftMargin: betwWidMarg

			function proceedButtonClick() {

				var isVisible = true;

				if (scanAnglesRangeRect.visible == true) {
					isVisible = false;
					scanAnglesRangeRect.visible = isVisible;
					angleStepRect.visible = isVisible;
				} else {
					isVisible = true;
					scanAnglesRangeRect.visible = isVisible;
					angleStepRect.visible = isVisible;
				}

				parentRect.widgetsVisibilityChanged(isVisible);
				parentRect.shitChanged(parentRect.parentRectWidth, parentRect.parentRectHeight);
			}

			onClicked: proceedButtonClick()
		}
	}

	Item {
		id : scanAnglesRangeRect

		width: parentRect.width - 6
		height: startScanRect.height

		anchors.left : startScanRect.left
		anchors.top: startScanRect.bottom
		anchors.topMargin: 10

		Text {
			id : scanAngleText
			width: startScanText.width

			text: qsTr("Scan Angle:")
			font.family : "Segoe UI"
			font.bold: false
			font.pointSize: 9

			anchors.verticalCenter: scanAnglesRangeRect.verticalCenter
			anchors.left: scanAnglesRangeRect.left
			anchors.leftMargin: 5
		}

		SpinBox { // Поставить валидатор
			id: angleRangeSpinBox

			width: startScanBtn.width
			height: startScanBtn.height

			anchors.verticalCenter: scanAnglesRangeRect.verticalCenter
			anchors.left: scanAngleText.right
			anchors.leftMargin: betwWidMarg

			from: 1
			to: 360

			value: 360
			editable: true

			property string suffix: "o"

			valueFromText: function(text, locale) {
					return Number.fromLocaleString(locale, numberExtractionRegExp.exec(text)[1])
				}
		}

		Button {
			id : saveSomeShitButton
			width : 26
			height: 26

			anchors.verticalCenter: angleRangeSpinBox.verticalCenter
			anchors.left: angleRangeSpinBox.right
			anchors.leftMargin: betwWidMarg
		}

	}

	Item {
		id : angleStepRect

		width: parentRect.width - 6
		height: startScanRect.height

		anchors.left : scanAnglesRangeRect.left
		anchors.top: scanAnglesRangeRect.bottom
		anchors.topMargin: 10

		Text {
			id: scanStepText
			width: startScanText.width
			text: qsTr("Scan Step:")

			font.family : "Segoe UI"
			font.bold: false
			font.pointSize: 9

			anchors.verticalCenter: angleStepRect.verticalCenter
			anchors.left: angleStepRect.left
			anchors.leftMargin: 5
		}

		SpinBox { // Поставить валидатор
			id: angleStepSpinBox

			width: angleRangeSpinBox.width
			height: angleRangeSpinBox.height

			anchors.verticalCenter: angleStepRect.verticalCenter
			anchors.left: scanStepText.right
			anchors.leftMargin: betwWidMarg

			from: 1
			to: 180

			value: 3
			editable: true

			property string suffix: "o"

			valueFromText: function(text, locale) {
					return Number.fromLocaleString(locale, numberExtractionRegExp.exec(text)[1])
				}
		}

		Rectangle {
			id : saveSomethingBnt

			width: saveSomeShitButton.width
			height: saveSomeShitButton.height

			anchors.verticalCenter: angleStepSpinBox.verticalCenter
			anchors.left: angleStepSpinBox.right
			anchors.leftMargin: betwWidMarg

			color: "silver"
			border.color: "black"

			border.width: 1
			radius : 3

			MouseArea {
				id : mouseEventHandler
				anchors.fill: parent

				onPressed: { parent.color = "lightsteelblue"; } // Посмотреть градиенты
				onReleased:{ parent.color = "silver"; }

			}

		}
	}

}

