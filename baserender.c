#include "baserender.h"
#include "flat.h"

cFlatBaseRender::cFlatBaseRender(void) {
    font = cFont::CreateFont(Setup.FontOsd, Setup.FontOsdSize );
    fontSml = cFont::CreateFont(Setup.FontSml, Setup.FontSmlSize);
    fontFixed = cFont::CreateFont(Setup.FontFix, Setup.FontFixSize);

    fontHeight = font->Height();
    fontSmlHeight = fontSml->Height();

    topBarTitle = "";
    topBarLastDate = "";
    topBarUpdateTitle = false;
    topBarHeight = 0;

    marginOsd = 5;
    marginItem = 10;

    progressBarHeight = 20;
    
    buttonsHeight = 0;
    
    topBarPixmap = NULL;
    buttonsPixmap = NULL;
    messagePixmap = NULL;
    contentPixmap = NULL;
    progressBarPixmap = NULL;
}

cFlatBaseRender::~cFlatBaseRender(void) {
    delete font;
    delete fontSml;
    delete fontFixed;

    if( osd )
    {
        if( topBarPixmap )
            osd->DestroyPixmap(topBarPixmap);
        if( buttonsPixmap )
            osd->DestroyPixmap(buttonsPixmap);
        if( messagePixmap )
            osd->DestroyPixmap(messagePixmap);
        if( contentPixmap )
            osd->DestroyPixmap(contentPixmap);
        if( progressBarPixmap )
            osd->DestroyPixmap(progressBarPixmap);

        delete osd;
    }
}

void cFlatBaseRender::CreateFullOsd(void) {
    CreateOsd(cOsd::OsdLeft() + marginOsd, cOsd::OsdTop() + marginOsd, cOsd::OsdWidth() - marginOsd*2, cOsd::OsdHeight() - marginOsd*2);
}

void cFlatBaseRender::CreateOsd(int left, int top, int width, int height) {
    osdWidth = width;
    osdHeight = height;

    osd = cOsdProvider::NewOsd(left, top);
    if (osd) {
        tArea Area = { 0, 0, width, height,  32 };
        if (osd->SetAreas(&Area, 1) == oeOk) {  
            return;
        }
    }
    return;
}

void cFlatBaseRender::TopBarCreate(void) {
    if( fontHeight > fontSmlHeight*2 )
        topBarHeight = fontHeight;
    else
        topBarHeight = fontSmlHeight * 2;

    topBarPixmap = osd->CreatePixmap(1, cRect(0, 0, osdWidth, topBarHeight));
    topBarPixmap->Fill(clrTransparent);
}

void cFlatBaseRender::TopBarSetTitle(cString title) {
    topBarTitle = title;
    topBarUpdateTitle = true;
}

// sollte bei jedum "Flush" aufgerufen werden!
void cFlatBaseRender::TopBarUpdate(void) {
    cString curDate = DayDateTime();
    if ( strcmp(curDate, topBarLastDate) || topBarUpdateTitle ) {
        topBarUpdateTitle = false;
        topBarLastDate = curDate;

        int fontTop = (topBarHeight - fontHeight) / 2;
        topBarPixmap->Fill(Theme.Color(clrTopBarBg));
        topBarPixmap->DrawText(cPoint(marginItem*2, fontTop), topBarTitle, Theme.Color(clrTopBarFont), Theme.Color(clrTopBarBg), font);
        
        time_t t;
        time(&t);
        
        cString time = TimeString(t);
        int timeWidth = font->Width(*time) + marginItem*2;
        topBarPixmap->DrawText(cPoint(osdWidth - timeWidth, fontTop), time, Theme.Color(clrTopBarTimeFont), Theme.Color(clrTopBarBg), font);
        
        cString weekday = WeekDayNameFull(t);
        int weekdayWidth = fontSml->Width(*weekday);
        
        cString date = ShortDateString(t);
        int dateWidth = fontSml->Width(*date);

        int fullWidth = max(weekdayWidth, dateWidth);
        
        topBarPixmap->DrawText(cPoint(osdWidth - timeWidth - fullWidth - marginItem*2, 0), weekday, Theme.Color(clrTopBarDateFont), Theme.Color(clrTopBarBg), fontSml, 0, taCenter);
        topBarPixmap->DrawText(cPoint(osdWidth - timeWidth - fullWidth - marginItem*2, fontSmlHeight), date, Theme.Color(clrTopBarDateFont), Theme.Color(clrTopBarBg), fontSml, 0, taCenter);
    }
}

void cFlatBaseRender::ButtonsCreate(void) {
    buttonsHeight = fontHeight + 30;
    buttonsWidth = osdWidth;

    buttonsPixmap = osd->CreatePixmap(1, cRect(0, osdHeight - buttonsHeight, buttonsWidth, buttonsHeight));
    buttonsPixmap->Fill(clrTransparent);
}

void cFlatBaseRender::ButtonsSet(const char *Red, const char *Green, const char *Yellow, const char *Blue) {
    int buttonWidth = (buttonsWidth / 4) - marginItem;

    buttonsPixmap->Fill(clrTransparent);

    int x = 0;
    buttonsPixmap->DrawText(cPoint(x, 0), Red, Theme.Color(clrButtonFont), Theme.Color(clrButtonBg), font, buttonWidth, 0, taCenter);
    buttonsPixmap->DrawRectangle(cRect(x, fontHeight + 10, buttonWidth, 6), Theme.Color(clrButtonRed));

    x += buttonWidth + marginItem;
    buttonsPixmap->DrawText(cPoint(x, 0), Green, Theme.Color(clrButtonFont), Theme.Color(clrButtonBg), font, buttonWidth, 0, taCenter);
    buttonsPixmap->DrawRectangle(cRect(x, fontHeight + 10, buttonWidth, 6), Theme.Color(clrButtonGreen));

    x += buttonWidth + marginItem;
    buttonsPixmap->DrawText(cPoint(x, 0), Yellow, Theme.Color(clrButtonFont), Theme.Color(clrButtonBg), font, buttonWidth, 0, taCenter);
    buttonsPixmap->DrawRectangle(cRect(x, fontHeight + 10, buttonWidth, 6), Theme.Color(clrButtonYellow));

    x += buttonWidth + marginItem;
    buttonsPixmap->DrawText(cPoint(x, 0), Blue, Theme.Color(clrButtonFont), Theme.Color(clrButtonBg), font, buttonWidth, 0, taCenter);
    buttonsPixmap->DrawRectangle(cRect(x, fontHeight + 10, buttonWidth, 6), Theme.Color(clrButtonBlue));
}

void cFlatBaseRender::MessageCreate(void) {
    messageHeight = fontHeight;
    int top = (osdHeight - messageHeight) / 2;
    messagePixmap = osd->CreatePixmap(2, cRect(0, top, osdWidth, messageHeight));
    messagePixmap->Fill(clrTransparent);
}

void cFlatBaseRender::MessageSet(eMessageType Type, const char *Text) {
    tColor col = Theme.Color(clrMessageStatus);
    switch (Type) {
        case mtStatus: 
            col = Theme.Color(clrMessageStatus);
            break;
        case mtInfo:
            col = Theme.Color(clrMessageInfo);
            break;
        case mtWarning:
            col = Theme.Color(clrMessageWarning);
            break;
        case mtError:
            col = Theme.Color(clrMessageError);
            break;
    }
    messagePixmap->Fill(Theme.Color(clrMessageBg));

    messagePixmap->DrawRectangle(cRect( 0, 0, messageHeight, messageHeight), col);
    messagePixmap->DrawRectangle(cRect( osdWidth - messageHeight, 0, messageHeight, messageHeight), col);

    int textWidth = font->Width(Text);
    messagePixmap->DrawText(cPoint((osdWidth - textWidth) / 2, 0), Text, Theme.Color(clrMessageFont), Theme.Color(clrMessageBg), font);
}

void cFlatBaseRender::MessageClear(void) {
    messagePixmap->Fill(clrTransparent);
}

void cFlatBaseRender::ContentCreate(int Left, int Top, int Width, int Height) {
    contentHasScrollbar = false;
    contentShown = false;

    contentLeft = Left;
    contentTop = Top;
    contentWidth = Width;
    contentHeight = Height;
    int lines = ContentVisibleLines();
    contentHeight = lines * fontHeight;
}

void cFlatBaseRender::ContentSet(const char *Text, tColor ColorFg, tColor ColorBg) {
    contentWrapper.Set(Text, font, contentWidth);
    contentColorFg = ColorFg;
    contentColorBg = ColorBg;

    int contentWrapperHeight = (contentWrapper.Lines()+1) * fontHeight;

    if( contentWrapperHeight > contentHeight ) {
        contentDrawPortHeight = contentWrapperHeight;
        contentHasScrollbar = true;
    } else {
        contentDrawPortHeight = contentHeight;
        contentHasScrollbar = false;
    }

    contentPixmap = osd->CreatePixmap(2, cRect(contentLeft, contentTop, contentWidth, contentHeight),
            cRect(0, 0, contentWidth, contentDrawPortHeight));

    contentPixmap->Fill(contentColorBg);

    contentDraw();
    contentShown = true;
}

bool cFlatBaseRender::ContentScrollable(void) {
    return contentHasScrollbar;
}

double cFlatBaseRender::ScrollbarSize(void) {
    return (double)contentHeight / (double)contentDrawPortHeight;
}

int cFlatBaseRender::ContentScrollTotal(void) {
    return contentWrapper.Lines();
}

int cFlatBaseRender::ContentGetHeight(void) {
    return contentHeight;
}

int cFlatBaseRender::ContentScrollOffset(void) {
    double offset;
    if ( ((-1)*contentPixmap->DrawPort().Point().Y() + contentHeight + fontHeight) > contentDrawPortHeight)
        offset = (double)1 - ScrollbarSize();
    else
        offset = (double)((-1)*contentPixmap->DrawPort().Point().Y())/(double)((-1)*contentPixmap->DrawPort().Point().Y() + contentHeight);
    return ContentScrollTotal() * offset;
}

int cFlatBaseRender::ContentVisibleLines(void) {
    return (contentHeight / fontHeight);
}

bool cFlatBaseRender::ContentScroll(bool Up, bool Page) {
    int aktHeight = contentPixmap->DrawPort().Point().Y();
    int totalHeight = contentPixmap->DrawPort().Height();
    int screenHeight = contentPixmap->ViewPort().Height();
    int lineHeight = fontHeight;
    bool scrolled = false;
    if (Up) {
        if (Page) {
            int newY = aktHeight + screenHeight;
            if (newY > 0)
                newY = 0;
            contentPixmap->SetDrawPortPoint(cPoint(0, newY));
            scrolled = true;
        } else {
            if (aktHeight < 0) {
                contentPixmap->SetDrawPortPoint(cPoint(0, aktHeight + lineHeight));
                scrolled = true;
            }
        }
    } else {
        if (Page) {
            int newY = aktHeight - screenHeight;
            if ((-1)*newY > totalHeight - screenHeight)
                newY = (-1)*(totalHeight - screenHeight);
            contentPixmap->SetDrawPortPoint(cPoint(0, newY));
            scrolled = true;
        } else {
            if (totalHeight - ((-1)*aktHeight + lineHeight) > screenHeight) {
                contentPixmap->SetDrawPortPoint(cPoint(0, aktHeight - lineHeight));
                scrolled = true;
            }
        }
    }
    return scrolled;
}

bool cFlatBaseRender::ContentIsShown(void) {
    return contentShown;
}

void cFlatBaseRender::ContentClear(void) {
    if( contentPixmap )
        contentPixmap->Fill(clrTransparent);
    contentShown = false;
}

void cFlatBaseRender::contentDraw(void) {
    int linesText = contentWrapper.Lines();
    int currentHeight = 0;
    for (int i=0; i < linesText; i++) {
        //printf("drawLine: %d %s\n", i, contentWrapper.GetLine(i));
        currentHeight = (i)*fontHeight;
        contentPixmap->DrawText(cPoint(0, currentHeight), contentWrapper.GetLine(i), contentColorFg, contentColorBg, font, contentWidth);
    }
}

void cFlatBaseRender::ProgressBarCreate(int Left, int Top, int Width, tColor ColorFg, tColor ColorBarFg, tColor ColorBg) {
    progressBarTop = Top;
    progressBarWidth = Width;

    progressBarColorFg = ColorFg;
    progressBarColorBarFg = ColorBarFg;
    progressBarColorBg = ColorBg;
    
    progressBarPixmap = osd->CreatePixmap(2, cRect(Left, Top, Width, progressBarHeight));
    progressBarPixmap->Fill(clrTransparent);
}

void cFlatBaseRender::ProgressBarDraw(int Current, int Total) {
    int top = progressBarHeight / 2 - 3;
    double percentLeft = ((double)Current) / (double)Total;

    progressBarPixmap->Fill( progressBarColorBg );

    progressBarPixmap->DrawRectangle(cRect( 0, top + 2, progressBarWidth, 2), progressBarColorFg);
    if (Current > 0)
        progressBarPixmap->DrawRectangle(cRect( 0, top, (progressBarWidth * percentLeft), 6), progressBarColorBarFg);
}

int cFlatBaseRender::ProgressBarHeight(void) {
    return progressBarHeight;
}


void cFlatBaseRender::ProgressBarDrawMarks(int Current, int Total, const cMarks *Marks, tColor Color, tColor ColorCurrent) {
    progressBarColorMark = Color;
    progressBarColorMarkCurrent = ColorCurrent;
    int posMark = 0, posMarkLast = 0;
    int posCurrent;
    
    int top = progressBarHeight / 2 - 3;
    progressBarPixmap->Fill( progressBarColorBg );
    
    // the small line
    progressBarPixmap->DrawRectangle(cRect( 0, top + 2, progressBarWidth, 2), progressBarColorFg);

    if( Marks ) {
        bool Start = true;
        for( const cMark *m = Marks->First(); m; m = Marks->Next(m) ) {
            posMark = ProgressBarMarkPos( m->Position(), Total );
            posCurrent = ProgressBarMarkPos( Current, Total );

            ProgressBarDrawMark(posMark, posMarkLast, posCurrent, Start, m->Position() == Current);
            posMarkLast = posMark;
            Start = !Start;
        }
        if( !Start ) {
            progressBarPixmap->DrawRectangle(cRect( posMarkLast, top, progressBarWidth - posMarkLast, 6), progressBarColorBarFg);
            if( posCurrent > posMarkLast )
                progressBarPixmap->DrawRectangle(cRect( posMarkLast, top, posCurrent - posMarkLast, 6), progressBarColorMarkCurrent);
        }
    } else {
        ProgressBarDraw(Current, Total);
    }
}

int cFlatBaseRender::ProgressBarMarkPos(int P, int Total) {
    return P * progressBarWidth / Total;
}

void cFlatBaseRender::ProgressBarDrawMark(int posMark, int posMarkLast, int posCurrent, bool Start, bool isCurrent)
{
    int top = progressBarHeight / 2 - 3;
    
    if( Start ) {
        if( posCurrent > posMark )
            progressBarPixmap->DrawRectangle(cRect( posMarkLast, top + 2, posMark - posMarkLast, 2), progressBarColorMarkCurrent);
        else {
            // marker
            progressBarPixmap->DrawRectangle(cRect( posCurrent - 3, top, 6, 6), progressBarColorMarkCurrent);
            if( posCurrent > posMarkLast )
                progressBarPixmap->DrawRectangle(cRect( posMarkLast, top + 2, posCurrent - posMarkLast, 2), progressBarColorMarkCurrent);
        }
        
        if( isCurrent )
            progressBarPixmap->DrawRectangle(cRect( posMark-5, 0, 10, 3), progressBarColorMarkCurrent);
        else
            progressBarPixmap->DrawRectangle(cRect( posMark-3, 0, 6, 3), progressBarColorMark);
    } else {
        if( posCurrent > posMark )
            progressBarPixmap->DrawRectangle(cRect( posMarkLast, top, posMark - posMarkLast, 6), progressBarColorMarkCurrent);
        else {
            progressBarPixmap->DrawRectangle(cRect( posMarkLast, top, posMark - posMarkLast, 6), progressBarColorBarFg);
            if( posCurrent > posMarkLast )
                progressBarPixmap->DrawRectangle(cRect( posMarkLast, top, posCurrent - posMarkLast, 6), progressBarColorMarkCurrent);
        }

        if( isCurrent )
            progressBarPixmap->DrawRectangle(cRect( posMark-5, progressBarHeight - 3, 10, 3), progressBarColorMarkCurrent);
        else
            progressBarPixmap->DrawRectangle(cRect( posMark-3, progressBarHeight - 3, 6, 3), progressBarColorMark);
    }
    
    if( posCurrent == posMark )
        progressBarPixmap->DrawRectangle(cRect( posMark-2, 0, 4, progressBarHeight), progressBarColorMarkCurrent);
    else
        progressBarPixmap->DrawRectangle(cRect( posMark-1, 0, 2, progressBarHeight), progressBarColorMark);

    if( posCurrent == posMarkLast )
        progressBarPixmap->DrawRectangle(cRect( posMarkLast-2, 0, 4, progressBarHeight), progressBarColorMarkCurrent);
    else
        progressBarPixmap->DrawRectangle(cRect( posMarkLast-1, 0, 2, progressBarHeight), progressBarColorMark);
        
}
