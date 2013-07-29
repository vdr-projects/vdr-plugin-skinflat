#pragma once

#include "baserender.h"

class cFlatDisplayMenu : public cFlatBaseRender,  public cSkinDisplayMenu {
    private:

        cPixmap *menuPixmap;
        int menuWidth;

        int contentTop;
    
        cPixmap *scrollbarPixmap;
        int scrollBarTop, scrollBarWidth, scrollBarHeight;

        int itemHeight;

        void DrawScrollbar(int Total, int Offset, int Shown, int Top, int Height, bool CanScrollUp, bool CanScrollDown);
        int ItemsHeight(void);
    public:
        cFlatDisplayMenu(void);
        virtual ~cFlatDisplayMenu();
        virtual void Scroll(bool Up, bool Page);
        virtual int MaxItems(void);
        virtual void Clear(void);
        virtual void SetTitle(const char *Title);
        virtual void SetButtons(const char *Red, const char *Green = NULL, const char *Yellow = NULL, const char *Blue = NULL);
        virtual void SetMessage(eMessageType Type, const char *Text);
        virtual void SetItem(const char *Text, int Index, bool Current, bool Selectable);
        virtual void SetScrollbar(int Total, int Offset);
        virtual void SetEvent(const cEvent *Event);
        virtual void SetRecording(const cRecording *Recording);
        virtual void SetText(const char *Text, bool FixedFont);
        virtual int GetTextAreaWidth(void) const;
        virtual const cFont *GetTextAreaFont(bool FixedFont) const;
        virtual void Flush(void);
};
