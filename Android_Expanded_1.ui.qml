import QtQuick
import QtQuick.Controls

Rectangle {
    id: android_Expanded_1
    width: 1280
    height: 800
    color: "#ffffff"
    property alias cOMFORTText: cOMFORT.text
    property alias sPORTText: sPORT.text
    property alias eCOText: eCO.text
    property alias aMText: aM.text

    Image {
        id: rectangle_1
        anchors.left: parent.left
        anchors.top: parent.top
        source: "assets/rectangle_1.png"
    }

    Image {
        id: neon_blue_flat_wireframe_grid_vanishing_checkered_floor_landscap
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 163
        source: "assets/neon_blue_flat_wireframe_grid_vanishing_checkered_floor_landscap.png"
    }

    Image {
        id: _blue_background_png_1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: -2
        source: "assets/_blue_background_png_1.png"
    }

    Image {
        id: _blue_background_png_2
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: -7
        source: "assets/_blue_background_png_2.png"
    }

    Image {
        id: _F_490577775_FJWG0sGo0OLoeBEcAIP4KSYjx1ofQ9o9_1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: -108
        anchors.topMargin: -72
        source: "assets/_F_490577775_FJWG0sGo0OLoeBEcAIP4KSYjx1ofQ9o9_1.png"
    }

    Image {
        id: pngtree_road_street_straight_turn_14700873_1_1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 202
        anchors.topMargin: 193
        source: "assets/pngtree_road_street_straight_turn_14700873_1_1.png"
    }

    Image {
        id: ellipse_3
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 834
        anchors.topMargin: 212
        source: "assets/ellipse_3.png"
    }

    Image {
        id: sagsinyal_on
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 163
        anchors.topMargin: 55
        source: "assets/sagsinyal_on.png"
        // visible: serialHandler.rightSignal

    }

    Image {
        id: solsinyal_off
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 78
        anchors.topMargin: 55
        source: "assets/solsinyal_off.png"
        // visible: serialHandler.leftSignal
    }

    Image {
        id: rectangle_3
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 415
        source: "assets/rectangle_3.png"
    }

    Image {
        id: rectangle_4
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 433
        source: "assets/rectangle_4.png"
    }

    Text {
        id: aM
        width: 241
        height: 41
        color: "#ffffff"
        text: qsTr("8:12 AM")
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 519
        anchors.topMargin: 13
        font.letterSpacing: 8.625
        font.pixelSize: 32
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignTop
        wrapMode: Text.NoWrap
        font.weight: Font.Normal
        font.family: "Digital Numbers"
    }

    Image {
        id: image_1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 48
        anchors.topMargin: 195
        source: "assets/image_1.png"
    }

    Image {
        id: rectangle_10
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: -141
        anchors.topMargin: 570
        source: "assets/rectangle_10.png"
    }

    Image {
        id: rectangle_16
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 656
        anchors.topMargin: 570
        source: "assets/rectangle_16.png"
    }

    Image {
        id: group_2
        x: 157
        y: 257
        source: "assets/group_2.png"
    }

    Image {
        id: rectangle_5
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 1423
        anchors.topMargin: 867
        source: "assets/rectangle_5.png"
    }

    Image {
        id: rectangle_8
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: -141
        anchors.topMargin: 679
        source: "assets/rectangle_8.png"
    }

    Image {
        id: p_R_N_D
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 869
        anchors.topMargin: 132
        source: "assets/p_R_N_D.png"
    }

/*    Image {
        id: km
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 519
        anchors.topMargin: 299
        source: "assets/km.png"
    }
*/
    Text {
        id: km
        text: serialHandler.menzilDisplay
        color: "#FFFFFF"
        font.family: "Audiowide"
        font.pixelSize: 48
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 519
        anchors.topMargin: 299
        antialiasing: true
    }
    Image {
        id: element
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 1026
        anchors.topMargin: 6
        source: "assets/element.png"
    }

/*    Image {
        id: element1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 179
        anchors.topMargin: 259
        source: "assets/element1.png"
    }
*/
    Text {
        id: element1
        text: serialHandler.bataryaValue
        color: "#FFA500"
        font.family: "Audiowide"
        font.pixelSize: 48
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 230
        anchors.topMargin: 259
        // Sayı değiştikçe pürüzsüz görünmesi için antialiasing
        antialiasing: true
    }
/*    Image {
        id: kW
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 929
        anchors.topMargin: 330
        source: "assets/kW.png"
    }
*/
    Text {
        id: kw
        text: serialHandler.kwDisplay // JavaScript yok, sadece hazır metni çekiyor
        color: "#FFFFFF"
        font.family: "Audiowide"
        font.pixelSize: 48
        // Konumlandırma         // Eski resminin koordinatlarını koruyoruz
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 929
        anchors.topMargin: 330
        // Sayı değiştikçe pürüzsüz görünmesi için antialiasing
        antialiasing: true
    }
    Image {
        id: kWh
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 938
        anchors.topMargin: 397
        source: "assets/kWh.png"
    }

    Image {
        id: km1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 975
        anchors.topMargin: 481
        source: "assets/km1.png"
    }

    Image {
        id: subtract
        x: 870
        y: 242
        source: "assets/subtract.png"
    }

    Image {
        id: rectangle_11
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 536
        source: "assets/rectangle_11.png"
    }

    Image {
        id: rectangle_12
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 553
        source: "assets/rectangle_12.png"
    }

    Image {
        id: a_1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 206
        anchors.topMargin: 738
        source: "assets/a_1.png"
    }

    Image {
        id: seat_belt_2_1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 1191
        anchors.topMargin: 720
        source: "assets/seat_belt_2_1.png"
    }

    Image {
        id: seat_belt_1_1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 1140
        anchors.topMargin: 720
        source: "assets/seat_belt_1_1.png"
    }

    Image {
        id: danger_1_1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 845
        anchors.topMargin: 720
        source: "assets/danger_1_1.png"
    }

    Image {
        id: car_indicator_3_2_1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 302
        anchors.topMargin: 718
        source: "assets/car_indicator_3_2_1.png"
    }

    Image {
        id: car_indicator_2_1_1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 913
        anchors.topMargin: 720
        source: "assets/car_indicator_2_1_1.png"
    }

    Image {
        id: car_indicator_1_1_1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 33
        anchors.topMargin: 717
        source: "assets/car_indicator_1_1_1.png"
    }

    Image {
        id: car_indicator_2_1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 100
        anchors.topMargin: 717
        source: "assets/car_indicator_2_1.png"
    }

    Image {
        id: car_indicator_2_2
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 168
        anchors.topMargin: 717
        source: "assets/car_indicator_2_2.png"
    }

    Image {
        id: km2
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 737
        anchors.topMargin: 677
        source: "assets/km2.png"
    }

    /*Image {
        id: element2
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 519
        anchors.topMargin: 592
        source: "assets/element2.png"
    }*/

    Text {
            id: speedValueText
            // ARTIK BURAYA serialHandler.speedValue YAZIYORUZ
            text: serialHandler.speedValue
            color: "#ffffff"

            // Font ayarları
            font.family: "Audiowide"
            font.pixelSize: 128
            font.weight: Font.Normal

            // Konumlandırma (Figma X:536, Y:577 değerlerine göre)
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 536
            anchors.topMargin: 577

            // Metni kutu içinde ortala
            width: 223 // Figma'daki W değeri
            height: 138 // Figma'daki H değeri
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

    Image {
        id: rEADY
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 577
        anchors.topMargin: 717
        source: "assets/rEADY.png"
    }

    Image {
        id: car_indicator_3_1_1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 234
        anchors.topMargin: 717
        source: "assets/car_indicator_3_1_1.png"
    }

    Image {
        id: element3
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 372
        anchors.topMargin: 705
        source: "assets/element3.png"
    }

    Image {
        id: element4
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 1081
        anchors.topMargin: 720
        source: "assets/element4.png"
    }

    Image {
        id: dashboard_1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 1033
        anchors.topMargin: 723
        source: "assets/dashboard_1.png"
    }

    Image {
        id: ellipse_6
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 931
        anchors.topMargin: 298
        source: "assets/ellipse_6.png"
    }

    Rectangle {
        id: rectangle_13
        width: 181
        height: 35
        color: "#d9d9d9"
        radius: 11
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 140
        anchors.topMargin: 480
    }

    Image {
        id: rectangle_14
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 115
        anchors.topMargin: 506
        source: "assets/rectangle_14.png"
    }

    Rectangle {
        id: rectangle_15
        width: 181
        height: 35
        color: "#d9d9d9"
        radius: 11
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 140
        anchors.topMargin: 574
    }

    Text {
        id: sPORT
        width: 97
        height: 31
        color: "#000000"
        text: qsTr("SPORT")
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 183
        anchors.topMargin: 482
        font.pixelSize: 24
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignTop
        wrapMode: Text.NoWrap
        font.weight: Font.Normal
        font.family: "Audiowide"
    }

    Text {
        id: eCO
        width: 58
        height: 31
        color: "#000000"
        text: qsTr("ECO")
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 199
        anchors.topMargin: 529
        font.pixelSize: 24
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignTop
        wrapMode: Text.NoWrap
        font.weight: Font.Normal
        font.family: "Audiowide"
    }

    Text {
        id: cOMFORT
        width: 138
        height: 31
        color: "#000000"
        text: qsTr("COMFORT")
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 162
        anchors.topMargin: 576
        font.pixelSize: 24
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignTop
        wrapMode: Text.NoWrap
        font.weight: Font.Normal
        font.family: "Audiowide"
    }
}

/*##^##
Designer {
    D{i:0;uuid:"f6d1cd8c-ab7b-5538-a9c9-79344b9ea07b"}D{i:1;uuid:"11a70a82-5f51-54af-88b1-605d290c9ea5"}
D{i:2;uuid:"f8372b4b-04c8-5b89-9f10-47c2bd4f593a"}D{i:3;uuid:"992dc76d-50b7-584b-8f98-9c7241651517"}
D{i:4;uuid:"d5cfa3fd-c10a-5e8e-b0c2-874f0fa90ead"}D{i:5;uuid:"8cb9f161-de6c-5d5e-8a5b-e7431b385405"}
D{i:6;uuid:"63dd71d9-1744-57c7-bf6c-831efb7beb65"}D{i:7;uuid:"a5e4b998-0724-5544-a7e2-504d58277e24"}
D{i:8;uuid:"825b705d-1b58-5dc3-98cd-3de82543c145"}D{i:9;uuid:"06e106ec-a238-582b-8d7e-813650fbc79b"}
D{i:10;uuid:"14205b99-b5a3-546a-bbf3-649c63490ac6"}D{i:11;uuid:"37809dda-b6e1-5321-9f63-b07991699a6e"}
D{i:12;uuid:"2493b861-25dd-59d0-8c8b-697175cea125"}D{i:13;uuid:"9919822e-6910-5cf1-9333-5c849d58d497"}
D{i:14;uuid:"8837ce62-b536-5dc4-9f95-1ed8d52b4173"}D{i:15;uuid:"bc55a223-2174-5fc8-b386-444374f4f9c0"}
D{i:16;uuid:"3823b889-cd70-5783-b9d3-6d821e480a76"}D{i:17;uuid:"8edac497-a330-5801-9838-29f6511ee913"}
D{i:18;uuid:"9480569d-82bc-5810-aa5b-80457b322638"}D{i:19;uuid:"9b57eff0-8153-5991-a85b-69364b0dd5d2"}
D{i:20;uuid:"23661db1-b1a2-582a-af58-245ebdae2e2d"}D{i:21;uuid:"94003543-d2b1-53ef-9de7-84d964c93f74"}
D{i:22;uuid:"6eedcffe-d4c7-5571-bf02-1c40a89e0c74"}D{i:23;uuid:"5c15e952-50c3-5368-b1f9-2079c99a3198"}
D{i:24;uuid:"9439f21b-700d-513c-8b99-d36ee5278202"}D{i:25;uuid:"26a932ef-d71f-5ff9-a6f4-32bb09d7ba77"}
D{i:26;uuid:"1c2188c9-7484-5929-af27-a8838fcd6644"}D{i:27;uuid:"92c88973-8338-5f75-ac49-41ad3ade6c93"}
D{i:28;uuid:"a0e89c28-8b29-592f-a702-afda34091e61"}D{i:29;uuid:"dff7b95d-c6e8-5327-98d1-632a98c00ac0"}
D{i:30;uuid:"39024620-9422-5e6d-80bf-13862fb73662"}D{i:31;uuid:"11d207b4-a5b8-5ef4-a682-8800f4cb8d0c"}
D{i:32;uuid:"eccad3f7-38eb-57a7-b4a0-950ede5c4de2"}D{i:33;uuid:"385365a7-e7ad-5ece-83ea-e824c15b340e"}
D{i:34;uuid:"070b4090-9a2f-5da6-a7d1-284733bf614d"}D{i:35;uuid:"e9805de8-4850-5579-b4d0-242b4272506e"}
D{i:36;uuid:"97649ce6-e377-5ddb-a5a7-615f14b440ba"}D{i:37;uuid:"bd9c340b-966e-5213-b0cf-64d81138dbea"}
D{i:38;uuid:"9aef5513-6970-532d-a359-cbcc15f1402d"}D{i:39;uuid:"106c4611-982e-5ec1-a34e-aeecf80d8f7d"}
D{i:40;uuid:"e46457da-b5f5-5254-a605-99a3335df0f5"}D{i:41;uuid:"d11b51c0-f5c8-52c5-a151-76f85198d4d0"}
D{i:42;uuid:"ed805369-a005-5cd5-b7f4-80a15263f370"}D{i:43;uuid:"5c133346-026c-5dca-8be9-3d4d2c9f16be"}
D{i:44;uuid:"fac4ef19-96c2-5925-bd0e-3f8edf2142ec"}D{i:45;uuid:"edebcc7f-3626-5df7-8061-6a8fd79bccda"}
D{i:46;uuid:"dcdb5d6f-d8ea-590d-a1d0-03fab4aa12ef"}D{i:47;uuid:"da47bb1d-033a-5064-aeb4-a12fd2e08db8"}
D{i:48;uuid:"9b37e64b-03af-5eb6-ad26-93eec2bec772"}D{i:49;uuid:"d48f2067-f59f-546c-9660-d96e16ffee37"}
D{i:50;uuid:"2ab75a22-7515-5639-8544-254322c6413b"}D{i:51;uuid:"12f5d62b-20b8-52fa-a54e-3bd53c8b875a"}
}
##^##*/

