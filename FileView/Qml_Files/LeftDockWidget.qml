import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
	id : r

	ColumnLayout {

		id : leftDockMainLay
		spacing: 0
		Layout.alignment: Qt.AlignLeft
		Layout.leftMargin : 20
		//anchors.fill: parent

		RowLayout {
			id: topLayout

			Button {
				   text: "Prev"
				   //onClicked: model.submit()
			   }
			Button {
				   text: "Next"
				   //onClicked: model.submit()
			   }


		}

		StackView {
			id : stackView

		}
	}



	//color: "red"
	//border.color: "black"
	//border.width: 5
	//radius: 10


}


