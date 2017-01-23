import bb.cascades 1.4

Container {
    property string title
    property bool loading: true
    property bool stepSuccess: false
    property string extraText
    
    horizontalAlignment: HorizontalAlignment.Center
    
    Divider {}
    Container {
        horizontalAlignment: HorizontalAlignment.Center
        layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
        Label {
            text: title
            multiline: true
            verticalAlignment: VerticalAlignment.Center
        }
        ActivityIndicator {
            minHeight: ui.du(10)
            minWidth: minHeight
            maxHeight: minHeight
            maxWidth: minHeight
            running: visible
            visible: loading && !aborted
            verticalAlignment: VerticalAlignment.Center
        }
        ImageView {
            minHeight: ui.du(10)
            minWidth: minHeight
            maxHeight: minHeight
            maxWidth: minHeight
            visible: !loading || aborted
            imageSource: stepSuccess ? "asset:///Images/success.png" : "asset:///Images/error.png"
            verticalAlignment: VerticalAlignment.Center
        }
        Label {
            text: extraText
            visible: extraText != ""
            multiline: true
            verticalAlignment: VerticalAlignment.Center
        }
    }
    Divider {}
}
