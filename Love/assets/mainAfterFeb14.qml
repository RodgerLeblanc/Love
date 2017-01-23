import bb.cascades 1.4

Page {
    Container {
        background: Color.Magenta
        layout: DockLayout {}
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        ScrollView {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            Container {
                id: mainPageContainer
                layout: DockLayout {}
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                leftPadding: ui.du(5)
                rightPadding: leftPadding
                bottomPadding: leftPadding
                topPadding: leftPadding
                ImageView {
                    imageSource: "asset:///Images/Icons/icon_500.png"
                    opacity: 0.3
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                    scalingMethod: ScalingMethod.AspectFit
                }
                Button {
                    visible: _app.getSettings("isRoger", false)
                    text: "Shutdown"
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Top
                    onClicked: {  _app.sendToHL("SHUTDOWN") }
                }
                Container {
                    layout: StackLayout {}
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                    Label {
                        text: qsTr("Valentine's day is over, but keep on loving BlackBerry 10 forever!\n\nYou can uninstall this app, it won't do anything else from now on.")
                        horizontalAlignment: HorizontalAlignment.Center
                        textStyle.textAlign: TextAlign.Center
                        textStyle.color: Color.White
                        multiline: true
                    }
                }
            }
        }
    }
}
