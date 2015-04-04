/*
  Copyright (C) 2013 Jolla Ltd.
  Contact: Thomas Perl <thomas.perl@jollamobile.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.xkcd 1.0


Page {
    id: page

    property ComicController comicController;
    property ComicModel comicModel;
    property Comic currentComic: comicController.currentComic

    // To enable PullDownMenu, place our content in a SilicaFlickable
    SilicaFlickable {
        anchors.fill: parent

        pressDelay: 0

        id: imageFlickable

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
        PullDownMenu {
            MenuItem {
                text: qsTr("Previous comic (" + String(currentComic.id - 1) + ")")
                onClicked: if(currentComic.id > 1) comicController.previousComic();
            }
            MenuItem {
                text: qsTr("Next comic (" + String(currentComic.id + 1) + ")")
                onClicked: comicController.nextComic();
            }
        }

        // Tell SilicaFlickable the height of its content.
        contentHeight: column.height

        // Place our content in a Column.  The PageHeader is always placed at the top
        // of the page, followed by our content.
        Column {
            id: column

            width: page.width
            height: page.height
            spacing: Theme.paddingLarge

            PageHeader {
                title: "#" + currentComic.id
                //visible: !flick.enableZoom
            }
            Label {
                x: Theme.paddingLarge
                text: currentComic.name
                color: Theme.secondaryHighlightColor
                font.pixelSize: Theme.fontSizeExtraLarge
                wrapMode: Text.Wrap
                //visible: comicImage.status == Image.Ready && !flick.enableZoom
            }


            Item {
                //anchors.topMargin: Theme.paddingLarge
                //anchors.top: lastLabel.bottom
                //anchors.verticalCenter: {if(flick.enableZoom) parent.verticalCenter; else undefined;}
                width: parent.width
                height: {
                    if(!flick.enableZoom)
                        500; // TODO: get rid of this constant
                    else
                        page.height;
                }

                //anchors.horizontalCenter: parent.horizontalCenter
                SilicaFlickable {
                    flickableDirection: SilicaFlickable.HorizontalAndVerticalFlick
                    clip: true
                    width: parent.width
                    height: parent.height
                    property bool enableZoom: false;
                    interactive: enableZoom;
                    //anchors.verticalCenter: {if(enableZoom) parent.verticalCenter; else undefined;}
                    //anchors.horizontalCenter: parent.horizontalCenter
                    contentWidth:
                    {
                        if(!enableZoom)
                            Math.min(parent.width - 2*Theme.paddingSmall,flick.height*comicImage.sourceSize.width/comicImage.sourceSize.height)
                        else
                            parent.width
                    }
                    x: Theme.paddingSmall
                    contentHeight: contentWidth/comicImage.sourceSize.width*comicImage.sourceSize.height
                    id: flick
                    enabled: true;

                    Behavior on x {
                        SmoothedAnimation{}
                    }

                    signal clicked

                    onClicked: {
                        enableZoom = ! enableZoom;
                    }

                    onEnableZoomChanged: {
                        if(!enableZoom)
                        {
                            contentX = 0;
                            contentY = 0;
                            x = Theme.paddingSmall
                            //resizeContent(parent.width - 2*Theme.paddingSmall, contentWidth/comicImage.sourceSize.width*comicImage.sourceSize.height);
                        }
                        else
                        {
                            x = 0;
                        }
                        returnToBounds();
                    }

                    boundsBehavior: Flickable.StopAtBounds

                    PinchArea {
                        width: Math.max(flick.contentWidth, flick.width)
                        height: Math.max(flick.contentHeight, flick.height)
                        //anchors.horizontalCenter: parent.horizontalCenter


                        property real initialWidth
                        property real initialHeight
                        onPinchStarted: {
                            initialWidth = flick.contentWidth
                            initialHeight = flick.contentHeight
                        }

                        onPinchUpdated: {
                            if(!flick.enableZoom)
                                return;
                            // adjust content pos due to drag
                            flick.contentX += pinch.previousCenter.x - pinch.center.x
                            flick.contentY += pinch.previousCenter.y - pinch.center.y

                            // resize content
                            flick.resizeContent(initialWidth * pinch.scale, initialHeight * pinch.scale, pinch.center)
                        }

                        onPinchFinished: {
                            if(!flick.enableZoom)
                                return;
                            // Move its content within bounds.
                            if((flick.width > (initialWidth * pinch.scale)) && (flick.height > (flick.width*comicImage.sourceSize.height/comicImage.sourceSize.width)))
                            {
                                flick.resizeContent(flick.width, flick.width*comicImage.sourceSize.height/comicImage.sourceSize.width, pinch.center)
                            }

                            flick.returnToBounds()
                        }
                        Item {
                            width: flick.contentWidth
                            height: flick.contentHeight
                            //anchors.horizontalCenter: column.horizontalCenter
                            Image {
                                //anchors.horizontalCenter: parent.horizontalCenter
                                //anchors {left:parent.left; right: parent.right;}
                                //anchors.horizontalCenter: parent.horizontalCenter
                                fillMode: Image.PreserveAspectFit
                                anchors.fill: parent
                                source: currentComic.url
                                asynchronous: true

                                onSourceChanged: {
                                    flick.enableZoom = false;
                                    altLabel.opened = false;
                                }

                                id: comicImage

                                MouseArea {
                                    anchors.fill: parent
                                    drag.axis: Drag.XAndYAxis

                                    //preventStealing: flick.enableZoom

                                    property int prevx;
                                    property int prevy;

                                    property int pressx;
                                    property int pressy;

                                    onPressed: {
                                        console.log("Comic pressed")
                                        pressx = prevx = mouse.x;
                                        pressy = prevy = mouse.y;
                                    }

                                    onClicked: {
                                        var movementSquared = Math.pow(mouse.x - pressx,2) + Math.pow(mouse.y - pressy,2)
                                        if(!flick.enableZoom || (movementSquared < 100))
                                        {
                                            console.log("Movement was " + String(movementSquared));
                                            flick.clicked();
                                            console.log("Comic clicked");
                                        }
                                    }
                                    onReleased: {
                                        console.log("Released");
                                        flick.returnToBounds();
                                    }
                                    onPressAndHold : {
                                        console.log("Press and hold")
                                        altLabel.opened = true
                                    }
                                }
                            }
                        }
                    }
                }
            }
            BusyIndicator {
                anchors.horizontalCenter: parent.horizontalCenter
                running: comicImage.status != Image.Ready
            }
            Label {
                text: currentComic.alt
                anchors.horizontalCenter: parent.horizontalCenter
                wrapMode: Text.Wrap
                width: parent.width - 2*Theme.paddingLarge
                property bool opened: false;
                property bool visibility: opened && !flick.enableZoom
                x: Theme.paddingLarge
                id: altLabel
                height: 0;
                opacity: 0.0;

                onVisibilityChanged: {
                    if(visibility)
                    {
                        opacity = 1.0;
                        height = undefined;
                    }
                    else
                    {
                        opacity = 0.0;
                        height = 0;
                        opened = false;
                    }
                }

                Behavior on opacity {
                    SmoothedAnimation{}
                }
                Behavior on height {
                    SmoothedAnimation{}
                }
            }
        }
    }
    onStatusChanged: {
        if (status === PageStatus.Active && pageStack.depth === 1) {

            pageStack.pushAttached("ComicListPage.qml", {
                                       comicController: comicController
                                   }
                                   );
        }
    }
}


