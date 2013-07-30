#pragma once

#include "flat.h"
#include "imageloader.h"

class cFlatBaseRender
{
    protected:
        cOsd *osd;

        int osdWidth, osdHeight;
        int marginOsd;
        int marginItem;

        // Standard fonts
        cFont *font;
        cFont *fontSml;
        cFont *fontFixed;
        int fontHeight;
        int fontSmlHeight;

        // TopBar
        cPixmap *topBarPixmap;
        cString topBarTitle;
        bool topBarUpdateTitle;
        cString topBarLastDate;
        int topBarHeight;

        // Progressbar
        cPixmap *progressBarPixmap;
        int progressBarHeight, progressBarTop, progressBarWidth;
        int progressBarCurrent, progressBarTotal;
        tColor progressBarColorFg, progressBarColorBarFg, progressBarColorBg;
        tColor progressBarColorMark, progressBarColorMarkCurrent;
        
        // Buttons rot, grün, gelb, blau
        cPixmap *buttonsPixmap;
        int buttonsWidth, buttonsHeight;

        // Nachricht
        cPixmap *messagePixmap;
        int messageWidth, messageHeight;

        // Mehrzeiliger Content mit Scrollbalken
        cPixmap *contentPixmap;
        int contentLeft, contentTop, contentHeight, contentWidth;
        int contentDrawPortHeight; // gesamthöhe des Textes
        bool contentHasScrollbar;
        bool contentShown;
        tColor contentColorFg, contentColorBg;
        cTextWrapper contentWrapper;

        void contentDraw(void);
        double ScrollbarSize(void);

        void ProgressBarDrawMark(int X, bool Start, bool Current);
        int ProgressBarMarkPos(int P);

    public:
        cImageLoader imgLoader;

        cFlatBaseRender(void);
        ~cFlatBaseRender(void);

        void CreateFullOsd(void);
        void CreateOsd(int Left, int Top, int Width, int Height);

        void TopBarCreate(void);
        void TopBarSetTitle(cString title);
        void TopBarUpdate(void);

        void ButtonsCreate(void);
        void ButtonsSet(const char *Red, const char *Green = NULL, const char *Yellow = NULL, const char *Blue = NULL);

        void MessageCreate(void);
        void MessageSet(eMessageType Type, const char *Text);
        void MessageClear(void);

        void ProgressBarCreate(int Left, int Top, int Width, tColor ColorFg, tColor ColorBarFg, tColor ColorBg);
        void ProgressBarDraw(int Current, int Total);
        int ProgressBarHeight(void);
        void ProgressBarDrawMarks(const cMarks *Marks, tColor Color, tColor ColorCurrent);
        
        void ContentCreate(int Left, int Top, int Width, int Height);
        void ContentSet(const char *Text, tColor ColorFg, tColor ColorBg);
        bool ContentIsShown(void);
        bool ContentScrollable(void);
        int ContentScrollTotal(void);
        int ContentScrollOffset(void);
        int ContentVisibleLines(void);
        int ContentGetHeight(void);
        bool ContentScroll(bool Up, bool Page);
        void ContentClear(void);
};
