#pragma once

#define X_DISPLAY_MISSING

#include <vdr/osd.h>
#include <vdr/skins.h>
#include <vdr/plugin.h>

#include <Magick++.h>

#define PLUGINPATH (cPlugin::ConfigDirectory(PLUGIN_NAME_I18N))

using namespace Magick;
 
class cImageLoader {
public:
    cImageLoader();
    ~cImageLoader();
    cImage GetImage();
    bool LoadLogo(const char *logo, int width, int height);
    bool LoadIcon(const char *cIcon, int size);
    bool LoadIcon(const char *cIcon, int width, int height, bool preserveAspect = true);
    bool LoadEPGImage(int eventID);
    bool LoadAdditionalEPGImage(cString name);
    bool LoadRecordingImage(cString Path);
    bool LoadAdditionalRecordingImage(cString path, cString name);
    void DrawBackground(tColor back, tColor blend, int width, int height, bool mirror = false);
    void DrawBackground2(tColor back, tColor blend, int width, int height);
    void Colorize(tColor col);
    int Height(void);
    int Width(void);

private:
    int epgImageWidthLarge, epgImageHeightLarge;
    int epgImageWidth, epgImageHeight;
    int logoWidth, logoHeight;
    cString logoExtension;
    cString logoPath, logoPathDefault, iconPath, iconPathDefault, epgImagePath, epgImagePathDefault;
    bool logoPathSet, iconPathSet, epgImagePathSet;

    Image buffer;
    Color Argb2Color(tColor col);
    void toLowerCase(std::string &str);
    bool LoadImage(cString FileName, cString Path, cString Extension);
	bool FirstImageInFolder(cString Path, cString Extension, cString *recImage);
};
