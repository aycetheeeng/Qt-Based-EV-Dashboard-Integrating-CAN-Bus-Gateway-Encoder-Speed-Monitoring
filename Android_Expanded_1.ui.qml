import QtQuick
import QtQuick.Controls

Rectangle {
    id: ekran_yeni2
    width: 1280
    height: 800
    color: "#ffffff"
    property alias cOMFORTText: cOMFORT.text
    property alias sPORTText: sPORT.text
    property alias saat_textText: saat_text.text
    property alias eCOText: eCO.text
    property alias powered_by_ayceText: powered_by_ayce.text

    Image {
        id: arkaplan_ekran_1
        width: 1280
        height: 801
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 0
        source: "assets/arkaplan_ekran_1.png"
    }

    Image {
        id: rectangle_1
        visible: false
        anchors.left: parent.left
        anchors.top: parent.top
        source: "assets/rectangle_1.png"
    }

    Image {
        id: neon_blue_flat_wireframe_grid
        visible: false
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 163
        source: "assets/neon_blue_flat_wireframe_grid.png"
    }

    Image {
        id: _blue_background_png_1
        visible: false
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: -2
        source: "assets/_blue_background_png_1.png"
    }

    Image {
        id: _blue_background_png_2
        visible: false
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: -7
        source: "assets/_blue_background_png_2.png"
    }

    Image {
        id: _F_490577775_FJWG0sGo0OLoeBEcAIP4KSYjx1ofQ9o9_1
        visible: false
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: -108
        anchors.topMargin: -72
        source: "assets/_F_490577775_FJWG0sGo0OLoeBEcAIP4KSYjx1ofQ9o9_1.png"
    }

    Image {
        id: pngtree_road_street_straight_turn_14700873_1_1
        visible: false
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
        id: rectangle_3
        visible: false
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 415
        source: "assets/rectangle_3.png"
    }

    Image {
        id: rectangle_4
        visible: false
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 433
        source: "assets/rectangle_4.png"
    }

    Text {
        id: saat_text
        width: 241
        height: 41
        color: "#ffffff"
        text: qsTr("8:12 AM")
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 546
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
        visible: false
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: -141
        anchors.topMargin: 570
        source: "assets/rectangle_10.png"
    }

    Image {
        id: rectangle_16
        visible: false
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
        visible: false
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 1423
        anchors.topMargin: 867
        source: "assets/rectangle_5.png"
    }

    Image {
        id: rectangle_8
        visible: false
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: -141
        anchors.topMargin: 679
        source: "assets/rectangle_8.png"
    }

    Image {
        id: sagsinyal_on
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 867
        anchors.topMargin: 24
        source: "assets/sagsinyal_on.png"
        visible: serialHandler.rightSignal && serialHandler.globalBlink

        // YANIP SÖNME EFEKTİ
        //opacity: 1.0

       /* Timer {
            interval: 350
            running: serialHandler.rightSignal
            repeat: true

            onTriggered: sagsinyal_on.opacity = (sagsinyal_on.opacity === 1.0 ? 0.0 : 1.0)
            } */
    }

    Image {
        id: solsinyal_off
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 325
        anchors.topMargin: 24
        source: "assets/solsinyal_off.png"
        visible: serialHandler.leftSignal && serialHandler.globalBlink

        // YANIP SÖNME EFEKTİ
        //opacity: 1.0

      /*  Timer {
            interval: 350
            running: serialHandler.leftSignal
            repeat: true

            onTriggered: solsinyal_off.opacity = (solsinyal_off.opacity === 1.0 ? 0.0 : 1.0)
            } */

    }

    Image {
        id: n_on
        x: 1063
        y: 120
        source: "assets/N_on.png.png"
        layer.enabled: false
        fillMode: Image.PreserveAspectFit
        visible: serialHandler.n_signal
    }

    Image {
        id: n_off_png
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 1059
        anchors.topMargin: 120
        source: "assets/n_off_png.png"
        visible: !serialHandler.n_signal
    }

    Image {
        id: r_on_png
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 962
        anchors.topMargin: 120
        source: "assets/r_on_png.png"
        visible: serialHandler.r_signal
    }

    Image {
        id: r_off_png
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 962
        anchors.topMargin: 120
        source: "assets/r_off_png.png"
        visible: !serialHandler.r_signal
    }

    Image {
        id: p_on_png
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 865
        anchors.topMargin: 120
        source: "assets/p_on_png.png"
        visible: serialHandler.p_signal
    }

    Image {
        id: p_off_png
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 865
        anchors.topMargin: 120
        source: "assets/p_off_png.png"
        visible: !serialHandler.p_signal
    }

    Image {
        id: d_on
        x: 1164
        y: 120
        source: "assets/D_on.png.png"
        fillMode: Image.PreserveAspectFit
        visible: serialHandler.d_signal
    }

    Image {
        id: d_off_png
        x: 1019
        y: 690
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 1160
        anchors.topMargin: 120
        source: "assets/d_off_png.png"
        visible: !serialHandler.d_signal
    }

    Image {
        id: kalan_km_text
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 519
        anchors.topMargin: 299
        source: "assets/kalan_km_text.png"
    }

    Image {
        id: tarih_text
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 23
        anchors.topMargin: 1
        source: "assets/tarih_text.png"
    }

    Image {
        id: batarya_yuzdesi_kirmizi_text
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 179
        anchors.topMargin: 259
        source: "assets/batarya_yuzdesi_kirmizi_text.png"
    }

    Image {
        id: batarya_yuzdesi_turuncu_text
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 179
        anchors.topMargin: 259
        source: "assets/batarya_yuzdesi_turuncu_text.png"
    }

    Image {
        id: batarya_yuzdesi_sari_text
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 179
        anchors.topMargin: 259
        source: "assets/batarya_yuzdesi_sari_text.png"
    }

    Image {
        id: batarya_yuzdesi_yesil_text
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 179
        anchors.topMargin: 259
        source: "assets/batarya_yuzdesi_yesil_text.png"
    }

    Image {
        id: batarya_yuzdesi_gri_text
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 179
        anchors.topMargin: 259
        source: "assets/batarya_yuzdesi_gri_text.png"
    }

    Image {
        id: kW_text
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 929
        anchors.topMargin: 330
        source: "assets/kW_text.png"
    }

    Image {
        id: kWh_renkli_text
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 938
        anchors.topMargin: 397
        source: "assets/kWh_renkli_text.png"
    }

    Image {
        id: kWh_renksiz_text
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 938
        anchors.topMargin: 397
        source: "assets/kWh_renksiz_text.png"
    }

    Image {
        id: _km_png
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 975
        anchors.topMargin: 481
        source: "assets/_km_png.png"
    }

    Image {
        id: cember_renkli
        x: 870
        y: 242
        source: "assets/cember_renkli.png"
    }

    Image {
        id: cember_renksiz
        x: 870
        y: 242
        source: "assets/cember_renksiz.png"
    }

    Image {
        id: rectangle_11
        visible: false
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 536
        source: "assets/rectangle_11.png"
    }

    Image {
        id: rectangle_12
        visible: false
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 553
        source: "assets/rectangle_12.png"
    }

    Image {
        id: a_otomatiklamba_acik
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 206
        anchors.topMargin: 738
        source: "assets/a_otomatiklamba_acik.png"
    }

    Image {
        id: a_otomatiklamba_kapali
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 206
        anchors.topMargin: 738
        source: "assets/a_otomatiklamba_kapali.png"
    }

    Image {
        id: kemer2
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 1191
        anchors.topMargin: 720
        source: "assets/kemer2.png"
    }

    Image {
        id: kemer1
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 1140
        anchors.topMargin: 720
        source: "assets/kemer1.png"
    }

    Image {
        id: danger_acik
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 845
        anchors.topMargin: 720
        source: "assets/danger_acik.png"
    }

    Image {
        id: danger_kapali
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 845
        anchors.topMargin: 720
        source: "assets/danger_kapali.png"
    }

    Image {
        id: arkasis
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 302
        anchors.topMargin: 717
        source: "assets/arkasis.png"
    }

    Image {
        id: lastik
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 913
        anchors.topMargin: 720
        source: "assets/lastik.png"
    }

    Image {
        id: lamba_acik
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 37
        anchors.topMargin: 717
        source: "assets/lamba_acik.png"
        visible: serialHandler.lambaAcik
    }

    Image {
        id: lamba_kapali
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 33
        anchors.topMargin: 717
        source: "assets/lamba_kapali.png"
        visible: !serialHandler.lambaAcik
    }

    Image {
        id: uzunlar_acik
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 100
        anchors.topMargin: 717
        source: "assets/uzunlar_acik.png"
    }

    Image {
        id: uzunlar_kapali
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 104
        anchors.topMargin: 717
        source: "assets/uzunlar_kapali.png"
    }

    Image {
        id: otomatiklamba_acik
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 168
        anchors.topMargin: 717
        source: "assets/otomatiklamba_acik.png"
    }

    Image {
        id: otomatiklamba_kapali
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 172
        anchors.topMargin: 717
        source: "assets/otomatiklamba_kapali.png"
    }

    Image {
        id: km_h
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 737
        anchors.topMargin: 677
        source: "assets/km_h.png"
    }

    Image {
        id: velocity_text
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 519
        anchors.topMargin: 592
        source: "assets/velocity_text.png"
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
        id: onsis_acik
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 234
        anchors.topMargin: 717
        source: "assets/onsis_acik.png"
    }

    Image {
        id: onsis_kapali
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 234
        anchors.topMargin: 717
        source: "assets/onsis_kapali.png"
    }

    Image {
        id: park_acik
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 372
        anchors.topMargin: 705
        source: "assets/park_acik.png"
        visible: true
    }

    Image {
        id: park_kapali
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 372
        anchors.topMargin: 705
        source: "assets/park_kapali.png"
        visible: false
    }

    Image {
        id: kapi_acik
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 1081
        anchors.topMargin: 720
        source: "assets/kapi_acik.png"
    }

    Image {
        id: kapi_kapali
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 1085
        anchors.topMargin: 720
        source: "assets/kapi_kapali.png"
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
        id: dashboard_2
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 1033
        anchors.topMargin: 723
        source: "assets/dashboard_2.png"
    }

    Image {
        id: surukleme_imleci
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 931
        anchors.topMargin: 298
        source: "assets/surukleme_imleci.png"
    }

    Image {
        id: aktif_comfort
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 115
        anchors.topMargin: 553
        source: "assets/aktif_comfort.png"
        visible: serialHandler.aktifComfort
    }

    Image {
        id: aktif_eco
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 115
        anchors.topMargin: 506
        source: "assets/aktif_eco.png"
        visible: serialHandler.aktifEco
    }

    Image {
        id: aktif_sport
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 115
        anchors.topMargin: 459
        source: "assets/aktif_sport.png"
        visible: serialHandler.aktifSport
    }

    Rectangle {
        id: inaktif_comfort
        width: 181
        height: 35
        color: "#d9d9d9"
        radius: 11
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 140
        anchors.topMargin: 574
        visible: !serialHandler.aktifComfort
    }

    Rectangle {
        id: inaktif_eco
        width: 181
        height: 35
        color: "#d9d9d9"
        radius: 11
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 140
        anchors.topMargin: 527
        visible: !serialHandler.aktifEco
    }

    Rectangle {
        id: inaktif_sport
        width: 181
        height: 35
        color: "#d9d9d9"
        radius: 11
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 140
        anchors.topMargin: 480
        visible: !serialHandler.aktifSport
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

    Rectangle {
        id: acilis_ekrani_siyah
        width: 1280
        height: 800
        visible: false
        color: "#000000"
        anchors.left: parent.left
        anchors.top: parent.top
    }

    Image {
        id: acilis_ekrani_kenargri
        visible: false
        anchors.left: parent.left
        anchors.top: parent.top
        source: "assets/acilis_ekrani_kenargri.png"
    }

    Image {
        id: acilis_ekrani_kenarsiyah
        visible: false
        anchors.left: parent.left
        anchors.top: parent.top
        source: "assets/acilis_ekrani_kenarsiyah.png"
    }

    Image {
        id: wELCOME
        visible: false
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 375
        anchors.topMargin: 367
        source: "assets/wELCOME.png"
    }

    Text {
        id: powered_by_ayce
        width: 229
        height: 31
        visible: false
        color: "#ffffff"
        text: qsTr("powered by ayce")
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 706
        anchors.topMargin: 457
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
    D{i:0;uuid:"f6d1cd8c-ab7b-5538-a9c9-79344b9ea07b"}D{i:1;uuid:"f8372b4b-04c8-5b89-9f10-47c2bd4f593a"}
D{i:2;uuid:"992dc76d-50b7-584b-8f98-9c7241651517"}D{i:3;uuid:"d5cfa3fd-c10a-5e8e-b0c2-874f0fa90ead"}
D{i:4;uuid:"8cb9f161-de6c-5d5e-8a5b-e7431b385405"}D{i:5;uuid:"63dd71d9-1744-57c7-bf6c-831efb7beb65"}
D{i:6;uuid:"a5e4b998-0724-5544-a7e2-504d58277e24"}D{i:7;uuid:"14205b99-b5a3-546a-bbf3-649c63490ac6"}
D{i:8;uuid:"37809dda-b6e1-5321-9f63-b07991699a6e"}D{i:12;uuid:"bc55a223-2174-5fc8-b386-444374f4f9c0"}
D{i:13;uuid:"3823b889-cd70-5783-b9d3-6d821e480a76"}D{i:14;uuid:"8edac497-a330-5801-9838-29f6511ee913"}
D{i:15;uuid:"9480569d-82bc-5810-aa5b-80457b322638"}D{i:16;uuid:"825b705d-1b58-5dc3-98cd-3de82543c145"}
D{i:17;uuid:"06e106ec-a238-582b-8d7e-813650fbc79b"}D{i:19;uuid:"350a5821-ddc6-556a-a6e3-61021225c484"}
D{i:21;uuid:"25599f25-5f13-5549-be8c-ba300ff5f0e2"}D{i:22;uuid:"9b57eff0-8153-5991-a85b-69364b0dd5d2"}
D{i:23;uuid:"23661db1-b1a2-582a-af58-245ebdae2e2d"}D{i:24;uuid:"94003543-d2b1-53ef-9de7-84d964c93f74"}
D{i:25;uuid:"a864342f-0642-5601-a8f6-186c19652190"}D{i:28;uuid:"6eedcffe-d4c7-5571-bf02-1c40a89e0c74"}
D{i:29;uuid:"933dbff4-7717-57a9-90e1-7f04d3ab2303"}D{i:30;uuid:"ff13d403-f2e3-588c-9fab-a22cb36a1c88"}
D{i:31;uuid:"870dd03b-7078-50d4-bcd4-b3ce4522e99a"}D{i:32;uuid:"5c15e952-50c3-5368-b1f9-2079c99a3198"}
D{i:33;uuid:"9439f21b-700d-513c-8b99-d36ee5278202"}D{i:34;uuid:"80d25572-3326-5f2d-ab9a-ebf394307a67"}
D{i:35;uuid:"26a932ef-d71f-5ff9-a6f4-32bb09d7ba77"}D{i:36;uuid:"76f8be6b-2b12-585e-8119-8065db2b30f0"}
D{i:37;uuid:"a9e80b77-1dc5-59c5-aacc-1da540525d84"}D{i:38;uuid:"92c88973-8338-5f75-ac49-41ad3ade6c93"}
D{i:39;uuid:"a0e89c28-8b29-592f-a702-afda34091e61"}D{i:40;uuid:"dff7b95d-c6e8-5327-98d1-632a98c00ac0"}
D{i:41;uuid:"39391092-2a39-5e4c-a838-5605f258714d"}D{i:42;uuid:"39024620-9422-5e6d-80bf-13862fb73662"}
D{i:43;uuid:"11d207b4-a5b8-5ef4-a682-8800f4cb8d0c"}D{i:44;uuid:"eccad3f7-38eb-57a7-b4a0-950ede5c4de2"}
D{i:45;uuid:"101fe834-7c50-5665-b04f-5b41b4a07f3c"}D{i:46;uuid:"385365a7-e7ad-5ece-83ea-e824c15b340e"}
D{i:47;uuid:"070b4090-9a2f-5da6-a7d1-284733bf614d"}D{i:48;uuid:"e601edfe-3588-5f4a-8315-60dd426391d3"}
D{i:49;uuid:"e9805de8-4850-5579-b4d0-242b4272506e"}D{i:50;uuid:"97649ce6-e377-5ddb-a5a7-615f14b440ba"}
D{i:51;uuid:"c8e4f716-2894-5560-bf3c-8f9b045134ec"}D{i:52;uuid:"c21d4bab-02fb-52ec-9c2e-78471163dfc9"}
D{i:53;uuid:"9dc2f9fb-f4f1-5441-942c-7ea0a196e348"}D{i:54;uuid:"9aef5513-6970-532d-a359-cbcc15f1402d"}
D{i:55;uuid:"106c4611-982e-5ec1-a34e-aeecf80d8f7d"}D{i:56;uuid:"e46457da-b5f5-5254-a605-99a3335df0f5"}
D{i:57;uuid:"d11b51c0-f5c8-52c5-a151-76f85198d4d0"}D{i:58;uuid:"33bb1b15-f137-55c4-8284-675761282293"}
D{i:59;uuid:"ed805369-a005-5cd5-b7f4-80a15263f370"}D{i:60;uuid:"e9e5044d-46b7-5b0a-ac56-fe4d49e87c4a"}
D{i:61;uuid:"5c133346-026c-5dca-8be9-3d4d2c9f16be"}D{i:62;uuid:"b11718ea-a413-5bed-aae4-fdb361f22462"}
D{i:63;uuid:"fac4ef19-96c2-5925-bd0e-3f8edf2142ec"}D{i:64;uuid:"d1a4392b-4e6c-59ad-bd41-b391e0c69c53"}
D{i:65;uuid:"edebcc7f-3626-5df7-8061-6a8fd79bccda"}D{i:66;uuid:"69a8ae41-a162-5ca0-8c6e-110b23d9a589"}
D{i:67;uuid:"da47bb1d-033a-5064-aeb4-a12fd2e08db8"}D{i:68;uuid:"a9f1494b-c207-573f-a8e3-b550bbe18d5f"}
}
##^##*/

