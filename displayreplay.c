#include "displayreplay.h"

cFlatDisplayReplay::cFlatDisplayReplay(bool ModeOnly) {
    progressBarHeight = 20;
    labelHeight = fontHeight;
    current = "";
    total = "";
    CreateFullOsd();
    TopBarCreate();
    MessageCreate();

    labelPixmap = osd->CreatePixmap(1, cRect(0, osdHeight - labelHeight, osdWidth, labelHeight));
    progressBarPixmap = osd->CreatePixmap(1, cRect(0, osdHeight - labelHeight - progressBarHeight, osdWidth, progressBarHeight));

    labelPixmap->Fill(Theme.Color(clrReplayBg));
    progressBarPixmap->Fill(clrTransparent);
}

cFlatDisplayReplay::~cFlatDisplayReplay() {
    osd->DestroyPixmap(labelPixmap);
    osd->DestroyPixmap(progressBarPixmap);
}

void cFlatDisplayReplay::SetRecording(const cRecording *Recording) {
    const cRecordingInfo *recInfo = Recording->Info();
    SetTitle( recInfo->Title() );
    cString info = "";
    if (recInfo->ShortText())
        info = cString::sprintf("%s - %s %s", recInfo->ShortText(), *ShortDateString(Recording->Start()), *TimeString(Recording->Start()));
    else
        info = cString::sprintf("%s %s", *ShortDateString(Recording->Start()), *TimeString(Recording->Start()));

    labelPixmap->DrawText(cPoint(0, 0), info, Theme.Color(clrReplayFont), Theme.Color(clrReplayBg), font, osdWidth);
}

void cFlatDisplayReplay::SetTitle(const char *Title) {
    TopBarSetTitle(Title);
}

void cFlatDisplayReplay::SetMode(bool Play, bool Forward, int Speed) {
    if( Setup.ShowReplayMode ) {
        int left = osdWidth - (labelHeight * 4 + marginItem * 3);
        left /= 2;

        labelPixmap->DrawRectangle(cRect( left - font->Width("33") - marginItem, 0, labelHeight*4 + marginItem*6 + font->Width("33")*2, labelHeight), Theme.Color(clrReplayBg) );
        labelPixmap->DrawRectangle(cRect( left - marginItem, 0, labelHeight*4 + marginItem*4, labelHeight), clrTransparent );

        cString rewind, pause, play, forward;
        cString speed;
        
        if( Speed == -1 ) {
            if( Play ) {
                rewind = "rewind";
                pause = "pause";
                play = "play_sel";
                forward = "forward";
            } else {
                rewind = "rewind";
                pause = "pause_sel";
                play = "play";
                forward = "forward";
            }                
        } else {
            speed = cString::sprintf("%d", Speed);
            if( Forward ) {
                rewind = "rewind";
                pause = "pause";
                play = "play";
                forward = "forward_sel";
                labelPixmap->DrawText(cPoint(left + labelHeight*4 + marginItem*4, 0), speed, Theme.Color(clrReplayFont), Theme.Color(clrReplayBg), font);
            } else {
                rewind = "rewind_sel";
                pause = "pause";
                play = "play";
                forward = "forward";
                labelPixmap->DrawText(cPoint(left - font->Width(speed) - marginItem, 0), speed, Theme.Color(clrReplayFont), Theme.Color(clrReplayBg), font);
            }                
        }

        if( imgLoader.LoadIcon(*rewind, labelHeight, labelHeight) )
            labelPixmap->DrawImage( cPoint(left, 0), imgLoader.GetImage() );
        if( imgLoader.LoadIcon(*pause, labelHeight, labelHeight) )
            labelPixmap->DrawImage( cPoint(left + labelHeight + marginItem, 0), imgLoader.GetImage() );
        if( imgLoader.LoadIcon(*play, labelHeight, labelHeight) )
            labelPixmap->DrawImage( cPoint(left + labelHeight*2 + marginItem*2, 0), imgLoader.GetImage() );
        if( imgLoader.LoadIcon(*forward, labelHeight, labelHeight) )
            labelPixmap->DrawImage( cPoint(left + labelHeight*3 + marginItem*3, 0), imgLoader.GetImage() );

    }
}

void cFlatDisplayReplay::DrawProgressBar(int Current, int Total) {
    int top = progressBarHeight / 2 - 3;
    int barFullWidth = progressBarPixmap->ViewPort().Width();
    double percentLeft = ((double)Current) / (double)Total;

    progressBarPixmap->Fill( Theme.Color(clrReplayBg) );

    if (Current > 0) {
        progressBarPixmap->DrawRectangle(cRect( 0, top + 2, barFullWidth, 2), Theme.Color(clrReplayProgressBg));
        progressBarPixmap->DrawRectangle(cRect( 0, top, (barFullWidth * percentLeft), 6), Theme.Color(clrReplayProgressFg));
    }
    if( Total > 0 )
        DrawProgressBarMarks(Current, Total);
}

void cFlatDisplayReplay::DrawProgressBarMarks(int Current, int Total) {
    if( marks ) {
        bool Start = true;
        for( const cMark *m = marks->First(); m; m = marks->Next(m) ) {
            int p1 = DrawProgressBarMarkPos( m->Position(), Total );
            DrawProgressBarMark(p1, Start, m->Position() == Current);
            Start = !Start;
        }
    }
}

int cFlatDisplayReplay::DrawProgressBarMarkPos(int P, int Total) {
    return P * progressBarPixmap->ViewPort().Width() / Total;
}

void cFlatDisplayReplay::DrawProgressBarMark(int X, bool Start, bool Current)
{
    if( Start )
        if( Current )
            progressBarPixmap->DrawRectangle(cRect( X-5, 0, 10, 3), Theme.Color(clrReplayMarkCurrentFg));
        else
            progressBarPixmap->DrawRectangle(cRect( X-3, 0, 6, 3), Theme.Color(clrReplayMarkFg));            
    else
        if( Current )
            progressBarPixmap->DrawRectangle(cRect( X-5, progressBarHeight - 3, 10, 3), Theme.Color(clrReplayMarkCurrentFg));
        else
            progressBarPixmap->DrawRectangle(cRect( X-3, progressBarHeight - 3, 6, 3), Theme.Color(clrReplayMarkFg));
    
    if( Current )
        progressBarPixmap->DrawRectangle(cRect( X-1, 0, 2, progressBarHeight), Theme.Color(clrReplayMarkCurrentFg));
    else
        progressBarPixmap->DrawRectangle(cRect( X-1, 0, 2, progressBarHeight), Theme.Color(clrReplayMarkFg));
}

void cFlatDisplayReplay::SetProgress(int Current, int Total) {
    DrawProgressBar(Current, Total);
}

void cFlatDisplayReplay::SetCurrent(const char *Current) {
    current = Current;
    UpdateInfo();
}

void cFlatDisplayReplay::SetTotal(const char *Total) {
    total = Total;
    UpdateInfo();
}

void cFlatDisplayReplay::UpdateInfo(void) {
    cString Info = cString::sprintf("%s - %s", *current, *total);
    int right = osdWidth - font->Width(Info);
    labelPixmap->DrawText(cPoint(right, 0), Info, Theme.Color(clrReplayFont), Theme.Color(clrReplayBg), font, font->Width(Info), fontHeight);
}

void cFlatDisplayReplay::SetJump(const char *Jump) {
    int left = osdWidth - font->Width(Jump);
    left /= 2;
    
    labelPixmap->DrawText(cPoint(left, 0), Jump, Theme.Color(clrReplayFont), Theme.Color(clrReplayBg), font, font->Width(Jump), fontHeight, taCenter);
}

void cFlatDisplayReplay::SetMessage(eMessageType Type, const char *Text) {
    if (Text)
        MessageSet(Type, Text);
    else
        MessageClear();
}

void cFlatDisplayReplay::Flush(void) {
    TopBarUpdate();
    osd->Flush();
}
