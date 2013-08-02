#include "displayvolume.h"

cFlatDisplayVolume::cFlatDisplayVolume(void) {
    muted = false;

    labelHeight = fontHeight;

    CreateFullOsd();
    TopBarCreate();
    int width = osdWidth / 4 * 3;
    int left = osdWidth - width;
    left /= 2;
    
    ProgressBarCreate(left, osdHeight - 50 - ProgressBarHeight(), width,
        Theme.Color(clrVolumeProgressFg), Theme.Color(clrVolumeProgressBarFg), Theme.Color(clrVolumeProgressBg));
    labelPixmap = osd->CreatePixmap(1, cRect(0, osdHeight - 50 - progressBarHeight - labelHeight - marginItem, osdWidth, labelHeight));
    muteLogoPixmap = osd->CreatePixmap(2, cRect(0, osdHeight - 50 - progressBarHeight - labelHeight - marginItem, osdWidth, labelHeight));
}

cFlatDisplayVolume::~cFlatDisplayVolume() {
    osd->DestroyPixmap(labelPixmap);
    osd->DestroyPixmap(muteLogoPixmap);
}

void cFlatDisplayVolume::SetVolume(int Current, int Total, bool Mute) {
    labelPixmap->Fill(clrTransparent);
    muteLogoPixmap->Fill(clrTransparent);
    
    cString label = cString::sprintf("%s: %d", tr("Volume"), Current);
    cString maxLabel = cString::sprintf("%s: %d", tr("Volume"), 555);
    int maxlabelWidth = font->Width(maxLabel);
    int left = osdWidth / 2 - maxlabelWidth / 2;

    if (Mute) {
        labelPixmap->DrawText(cPoint(left, 0), *label, Theme.Color(clrVolumeFont), Theme.Color(clrVolumeBg),
            font, maxlabelWidth + marginItem*2 + labelHeight, fontHeight, taLeft);
        if( imgLoader.LoadIcon("mute", labelHeight, labelHeight) ) {
            muteLogoPixmap->DrawImage( cPoint(left + maxlabelWidth + marginItem, 0), imgLoader.GetImage() );
        }
    } else {
        labelPixmap->DrawText(cPoint(left, 0), *label, Theme.Color(clrVolumeFont), Theme.Color(clrVolumeBg),
            font, maxlabelWidth, fontHeight, taLeft);
    }
    ProgressBarDraw(Current, Total);
}

void cFlatDisplayVolume::Flush(void) {
    TopBarUpdate();
    osd->Flush();
}
