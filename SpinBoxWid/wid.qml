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


	ColumnLayout {
		id : mainLay
		anchors.fill: parent
		spacing: 2
		Layout.alignment: Qt.AlignTop

		Rectangle {
			id : rowLayRect
			Layout.fillWidth: true
			Layout.fillHeight: true
			border.color: "black"

			Layout.minimumWidth: 50
			Layout.preferredWidth: 280
			Layout.maximumWidth: 350

			Layout.minimumHeight: header.font.pointSize + 4
			Layout.preferredHeight: header.font.pointSize + 8
			Layout.maximumHeight : header.font.pointSize + 10
			//Layout.alignment : Qt.AlignHCenter

			Layout.leftMargin : 10

			Text {
				id : header
				text: "Scanning"
				font.family : qsTr("Segoe UI")
				font.bold: true
				font.pointSize: 12
			}
		}

		// RowLayout {
		// 	id : headerLay
		// 	//anchors.left : parent.left
		// 	//anchors.leftMargin: 10
		// 	Layout.fillWidth: true
		// 	//Layout.alignment : Qt.AlignLeft
		// 	Layout.alignment : Qt.AlignTop

		// 	Layout.minimumWidth: 50
		// 	Layout.preferredWidth: 280
		// 	Layout.maximumWidth: 350

		// 	Layout.minimumHeight: header.font.pointSize
		// 	Layout.preferredHeight: header.font.pointSize + 5
		// 	Layout.maximumHeight : header.font.pointSize + 8

		// 	Layout.leftMargin : 10
		// 	Layout.topMargin : 5


		// }

		RowLayout {
			id: runModeLayout
			Layout.alignment: Qt.AlignTop

			Layout.minimumWidth: 50
			Layout.preferredWidth: 280
			Layout.maximumWidth: 350

			Layout.minimumHeight: startScanBtn.height
			Layout.preferredHeight: startScanBtn.height + 5
			Layout.maximumHeight : startScanBtn.height + 8

			Layout.leftMargin : 5

			Rectangle {
				id : rowLayRect2
				border.color: "black"
				Layout.fillWidth: true
				Layout.fillHeight: true
				Layout.alignment : Qt.AlignLeft

				RowLayout {
					id : startScanWidLay
					Layout.fillWidth: true
					Layout.fillHeight: true



					Text {
						id : startScanText
						text: "Run mode"
						font.family : qsTr("Segoe UI")
						font.bold: false
						font.pointSize: 12
						Layout.alignment : Qt.AlignHCenter
						Layout.leftMargin : 15
					}

					Button {
						id : startScanBtn
						width : 285
						height: 25
						text: qsTr("Start Scanning")
					}

					Button {
						id : scanningSettingsBtn
						width : 26
						height: 26
						text: qsTr("Start Scanning")
					}
				}


			}
		}
	}
}

