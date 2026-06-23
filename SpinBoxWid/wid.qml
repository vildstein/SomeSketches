import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
	id : pareRect
	width: 200
	height: 200
	color: "silver"
	border.color: "black"
	border.width: 2
	radius: 5


	ColumnLayout {
		id : mainLay

		RowLayout {
			id : headerLay
			anchors.left : parent.left
			anchors.leftMargin: 10

			Text {
				id : header
				text: "Scanning"
				font.family : qsTr("Segoe UI")
				font.bold: true
			}
		}

		RowLayout {
			id: runModeLayout
			//anchors.fill: parent

			Text {
				id : startScanText
				text: "Run mode"
				font.family : qsTr("Segoe UI")
				font.bold: false
				anchors.left : parent.left
				anchors.leftMargin: 10
			}

			Button {
				id : startScanBtn
				width : 285
				height: 25
				text: "Start Scanning"
			}
			Button {
				id : scanningSettingsBtn
				width : 26
				height: 26
				text: "Start Scanning"
			}
		}
	}
}

