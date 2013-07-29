#pragma once

#include "baserender.h"

class cFlatDisplayVolume : public cFlatBaseRender, public cSkinDisplayVolume {
    private:
        bool muted;

        cPixmap *progressBarPixmap;
        int progressBarHeight;

        cPixmap *labelPixmap;
        int labelHeight;

        void DrawProgressBar(int Current, int Total);
    public:
        cFlatDisplayVolume(void);
        virtual ~cFlatDisplayVolume();
        virtual void SetVolume(int Current, int Total, bool Mute);
        virtual void Flush(void);
};
