#include "displayvolume.h"

cFlatDisplayVolume::cFlatDisplayVolume(void) {
    muted = false;

    progressBarHeight = 20;
    labelHeight = fontHeight;

    CreateFullOsd();
    TopBarCreate();

    progressBarPixmap = osd->CreatePixmap(1, cRect(0, osdHeight - 50 - progressBarHeight, osdWidth, progressBarHeight));
    labelPixmap = osd->CreatePixmap(1, cRect(0, osdHeight - 50 - progressBarHeight - labelHeight - marginItem, osdWidth, labelHeight));
}

cFlatDisplayVolume::~cFlatDisplayVolume() {
    osd->DestroyPixmap(labelPixmap);
    osd->DestroyPixmap(progressBarPixmap);
}

void cFlatDisplayVolume::SetVolume(int Current, int Total, bool Mute) {
    labelPixmap->Fill(clrTransparent);
    cString label = cString::sprintf("%s: %d", tr("Volume"), Current);
    cString maxLabel = cString::sprintf("%s: %d", tr("Volume"), 555);
    int maxlabelWidth = font->Width(maxLabel);
    int left = osdWidth / 2 - maxlabelWidth / 2;

    labelPixmap->DrawText(cPoint(left, 0), *label, Theme.Color(clrVolumeFont), Theme.Color(clrVolumeBg),
            font, maxlabelWidth, fontHeight, taCenter);
    if (Mute) {
        if( imgLoader.LoadIcon("mute", labelHeight, labelHeight) ) {
            labelPixmap->DrawImage( cPoint(left + maxlabelWidth + marginItem, 0), imgLoader.GetImage() );
        }
    }
    DrawProgressBar(Current, Total);
}

void cFlatDisplayVolume::DrawProgressBar(int Current, int Total) {
    int top = progressBarHeight / 2 - 3;
    int barFullWidth = osdWidth * 3 / 4;
    int left = osdWidth - barFullWidth;
    left /= 2;
    double percentLeft = ((double)Current) / (double)Total;

    progressBarPixmap->Fill( clrTransparent );
    progressBarPixmap->DrawRectangle(cRect( left, 0, barFullWidth, progressBarHeight), Theme.Color(clrVolumeBg));

    if (Current > 0) {
        progressBarPixmap->DrawRectangle(cRect( left, top + 2, barFullWidth, 2), Theme.Color(clrVolumeProgressBg));
        progressBarPixmap->DrawRectangle(cRect( left, top, (barFullWidth * percentLeft), 6), Theme.Color(clrVolumeProgressFg));
    }
}

void cFlatDisplayVolume::Flush(void) {
    TopBarUpdate();
    osd->Flush();
}
