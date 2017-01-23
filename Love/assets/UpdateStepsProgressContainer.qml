import bb.cascades 1.4
import bb.system 1.2

Container {
    property bool progressFinished: false
    property bool aborted: false
    
    layout: DockLayout {}
    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Fill
    
    topPadding: ui.du(3)
    bottomPadding: topPadding
    leftPadding: topPadding
    rightPadding: topPadding
    background: Color.Magenta
    
    onCreationCompleted: { _app.messageUpdateAborted.connect(onMessageUpdateAborted) }
    attachedObjects: [
        SystemToast {
            id: toast
            button.label: qsTr("OK")
            onFinished: { 
                body = ""
                if (_app.getSettings("illegalWordsFoundCount", 0) >= 2) {
                    bannedUserLastToast.show()
                }
            }
        },
        SystemToast {
            id: bannedUserLastToast
            button.label: qsTr("OK")
            body: qstr("(You'll still receive love messages in Hub from respectful BlackBerry 10 users on Valentine's day)")
            onFinished: { 
                _app.sendToHL("SHUTDOWN")
            }
        }
    ]
    function onMessageUpdateAborted(errorMessage) {
        aborted = true
        toast.body = errorMessage
        toast.show()
    }
    
    Container {
        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Center
        StepProgressTemplate {
            title: qsTr("Checking language used...")
            onCreationCompleted: { _app.languageDetected.connect(onLanguageDetected) }
            function onLanguageDetected(success, language) {
                loading = false
                stepSuccess = success
                extraText = language
            }
        }
        StepProgressTemplate {
            title: qsTr("Checking if text is appropriate...")
            onCreationCompleted: { _app.profanityAndAnonimityChecked.connect(onProfanityAndAnonimityChecked) }
            function onProfanityAndAnonimityChecked(success, illegalWords) {
                loading = false
                stepSuccess = success
                if (illegalWords != "") {
//                    extraText = "Words rejected: " + illegalWords
                    progressFinished = true
                }
            }
        }
        StepProgressTemplate {
            title: qsTr("Deleting old message from database...")
            onCreationCompleted: { _app.checkingIfDocumentExistDone.connect(onCheckingIfDocumentExistDone) }
            function onCheckingIfDocumentExistDone(success) {
                loading = false
                stepSuccess = success
            }
        }
        StepProgressTemplate {
            title: qsTr("Updating message in database...")
            onCreationCompleted: { _app.updatingDocumentDone.connect(onUpdatingDocumentDone) }
            function onUpdatingDocumentDone(success) {
                loading = false
                stepSuccess = success
                progressFinished = true
            }
        }
        
        Button {
            text: qsTr("OK")
            enabled: progressFinished
            horizontalAlignment: HorizontalAlignment.Center
            onClicked: { progressIndicatorContainer.visible = false }
        }
    }
}

