/*
 * Copyright (c) 2013-2015 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import bb.cascades 1.4
import bb.system 1.2

Page {
    property int messageUpdateCount: _app.getSettings("messageUpdateCount", 0)
    attachedObjects: [
        AppCover { id: activeFrame },
        SystemToast {
            id: messageUpdatedToast
            button.label: qsTr("OK")
            onFinished: { body = "" }
        },
        SystemToast {
            id: bannedUserToast
            body: qsTr("You have been banned from posting. This app will quit.") + "" + qstr("(You'll still receive love messages in Hub from respectful BlackBerry 10 users on Valentine's day)")
            button.label: qsTr("OK")
            onFinished: { _app.sendToHL("SHUTDOWN") }
        }
    ]
    
    onCreationCompleted: {
        Application.menuEnabled = false
        Application.setCover(activeFrame)
        _app.messageNotUpdated.connect(onMessageNotUpdated)
        _app.messageUpdatedSuccessfully.connect(onMessageUpdatedSuccessfully)
        
        if (_app.getSettings("illegalWordsFoundCount", 0) >= 2) {
            mainPageContainer.visible = false
            bannedUserToast.show()
        }
    }
    
    function onMessageNotUpdated(errorMessage) {
        messageUpdatedToast.body = errorMessage
        messageUpdatedToast.show()
        
        activityIndicatorContainer.visible = false
    }
    
    function onMessageUpdatedSuccessfully() {
        messageUpdatedToast.body = qsTr("Message updated successfully")
        messageUpdatedToast.show()

        messageUpdateCount++
    }

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
                        text: qsTr("For Valentine's day, send an anonymous love message to random BlackBerry 10 users, because... well, because we're so lovely! Who knows, you might receive some love too.\n\nYou can write your love message right now, you can change it only once if you regret your first message.")
                        horizontalAlignment: HorizontalAlignment.Center
                        textStyle.textAlign: TextAlign.Center
                        textStyle.color: Color.White
                        multiline: true
                    }
                    TextArea {
                        id: messageTextArea
                        property bool isEmpty: text == ""
                        text: _app.getSettings("message", "")
                        hintText: qsTr("ie: You're lovely!")
                        enabled: messageUpdateCount < 2
                        content.flags: TextContentFlag.EmoticonsOff | TextContentFlag.ActiveTextOff
                        maximumLength: 1024
                        horizontalAlignment: HorizontalAlignment.Center
                        onTextChanging: {
                            isEmpty = (text == "")
                        }
                    }
                    Button {
                        text: qsTr("Update")
                        horizontalAlignment: HorizontalAlignment.Center
                        enabled: !messageTextArea.isEmpty && (messageUpdateCount < 2)
                        onClicked: { 
                            if (messageUpdateCount == 1) {
                                lastUpdateDialog.show()
                            }
                            else {
                                progressIndicatorContainer.visible = true
                                _app.sendToHL("UPDATE_MESSAGE", messageTextArea.text) 
                            }
                        }
                        attachedObjects: [
                            SystemDialog {
                                id: lastUpdateDialog
                                title: qsTr("Last update")
                                body: qsTr("You're allowed to only one modification, this new message will be your final message, you won't be able to modify it anymore. Continue?")
                                onFinished: {
                                    if (result == SystemUiResult.ConfirmButtonSelection) {
                                        progressIndicatorContainer.visible = true
                                        _app.sendToHL("UPDATE_MESSAGE", messageTextArea.text) 
                                    }
                                }
                            }
                        ]
                    }
                    
                    Container {
                        horizontalAlignment: HorizontalAlignment.Center
                        topPadding: ui.du(5)
                        Label {
                            text: qsTr("You'll start receiving Love messages from other BlackBerry 10 users on February 14th, randomly picked and sent to your Hub between 8am and 9pm.")
                            horizontalAlignment: HorizontalAlignment.Center
                            textStyle.textAlign: TextAlign.Center
                            textStyle.color: Color.White
                            textStyle.fontSize: FontSize.XSmall
                            multiline: true
                        }
                        Label {
                            text: "<u>" + qsTr("(Clean language and anonymity enforced)") + "</u>"
                            textFormat: TextFormat.Html
                            horizontalAlignment: HorizontalAlignment.Center
                            textStyle.textAlign: TextAlign.Center
                            textStyle.color: Color.DarkBlue
                            multiline: true
                            gestureHandlers: [ TapHandler { onTapped: { profanityToast.show() } } ]
                            attachedObjects: [
                                SystemToast {
                                    id: profanityToast
                                    button.label: qsTr("OK")
                                    body: qsTr("Your text will be both automatically and manually screened for offensive language and personal identifiable information.")
                                    onFinished: { profanityToast2.show() }
                                },
                                SystemToast {
                                    id: profanityToast2
                                    button.label: qsTr("OK")
                                    body: qsTr("If you try to post such things, you'll be banned from using this app.")
                                    onFinished: { contactToast.show() }
                                },
                                SystemToast {
                                    id: contactToast
                                    button.label: qsTr("OK")
                                    body: qsTr("Contact info@cellninja.ca at any time if you find any other user's message offensive.")
                                }
                            ]
                        }
                    }

                }
            }
        }
        
        Container {
            id: progressIndicatorContainer
            visible: false
            layout: DockLayout {}
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            topPadding: ui.du(5)
            bottomPadding: topPadding
            leftPadding: topPadding
            rightPadding: topPadding
            Container {
                id: updateStepsProgressContainerParent
                layout: DockLayout {}
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                topPadding: ui.du(2)
                bottomPadding: topPadding
                leftPadding: topPadding
                rightPadding: topPadding
                background: Color.Black
            }
            attachedObjects: [
                ComponentDefinition {
                    id: updateStepsProgressContainer
                    UpdateStepsProgressContainer {}
                }
            ]
            onVisibleChanged: {
                if (visible) {
                    var control = updateStepsProgressContainer.createObject()
                    updateStepsProgressContainerParent.add(control)
                }
                else {
                    for (var i = 0; i < updateStepsProgressContainerParent.controls.length; i++) {
                        var control = updateStepsProgressContainerParent.controls[i]
                        updateStepsProgressContainerParent.remove(control)
                        control.destroy()
                    }
                }
            }
        }
    }
}
