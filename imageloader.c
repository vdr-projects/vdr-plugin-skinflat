#include "imageloader.h"
#include <math.h>
#include <string>
#include <dirent.h>
#include <iostream>

using namespace Magick;

cImageLoader::cImageLoader() {
    InitializeMagick(NULL);
    logoPathDefault = cString::sprintf("%s/logos/", PLUGINPATH);
    iconPathDefault = cString::sprintf("%s/icons/", PLUGINPATH);
    epgImagePathDefault = cString::sprintf("%s/epgImages/", PLUGINPATH);
    
    logoExtension = "png";
}

cImageLoader::~cImageLoader() {
}

bool cImageLoader::LoadLogo(const char *logo, int width=-1, int height=-1) {
    if( width == -1 )
        width = logoWidth;
    if( height == -1 )
        height = logoHeight;

    if( (width == 0) || (height==0) )
        return false;
    std::string logoLower = logo;
    toLowerCase(logoLower);
    bool success = false;
    if( logoPathSet ) {
        success = LoadImage(logoLower.c_str(), logoPath, logoExtension);
    }
    if( !success ) {
        success = LoadImage(logoLower.c_str(), logoPathDefault, logoExtension);
    }
    if( !success )
        return false;

    if( height != 0 || width != 0 ) {
        buffer.sample( Geometry(width, height) );
    }
    return true;
}

int cImageLoader::Height(void) {
    Geometry geo = buffer.size();
    return geo.height();
}

int cImageLoader::Width(void) {
    Geometry geo = buffer.size();
    return geo.width();
}

bool cImageLoader::LoadIcon(const char *cIcon, int size) {
	if (size==0)
        return false;
    bool success = false;
    if (iconPathSet) {
        cString iconThemePath = cString::sprintf("%s%s/", *iconPath, Setup.OSDTheme);
        success = LoadImage(cString(cIcon), iconThemePath, "png");
        if (!success) {
            success = LoadImage(cString(cIcon), iconPath, "png");
        }
    }
    if (!success) {
        cString iconThemePath = cString::sprintf("%s%s/", *iconPathDefault, Setup.OSDTheme);
        printf("%s\n", *iconThemePath);
        success = LoadImage(cString(cIcon), iconThemePath, "png");
        if (!success) {
            success = LoadImage(cString(cIcon), iconPathDefault, "png");
        }
    }
    if (!success)
        return false;
    buffer.sample(Geometry(size, size));
    return true;
}

bool cImageLoader::LoadIcon(const char *cIcon, int width, int height, bool preserveAspect) {
  try {
    if ((width == 0)||(height==0))
        return false;
    bool success = false;
    if (iconPathSet) {
        cString iconThemePath = cString::sprintf("%s%s/", *iconPath, Setup.OSDTheme);
        success = LoadImage(cString(cIcon), iconThemePath, "png");
        if (!success) {
            success = LoadImage(cString(cIcon), iconPath, "png");
        }
    }
    if (!success) {
        cString iconThemePath = cString::sprintf("%s%s/", *iconPathDefault, Setup.OSDTheme);
        success = LoadImage(cString(cIcon), iconThemePath, "png");
        if (!success) {
            success = LoadImage(cString(cIcon), iconPathDefault, "png");
        }
    }
    if (!success)
        return false;
    if (preserveAspect) {
        buffer.sample(Geometry(width, height));
    } else {
        cString geometry = cString::sprintf("%dx%d!", width, height);
        buffer.scale(Geometry(*geometry));
    }
    return true;
  }
  catch (...) {
    return false;
  }
}

bool cImageLoader::LoadEPGImage(int eventID) {
    int width = epgImageWidth;
    int height = epgImageHeight;
    if ((width == 0)||(height==0))
        return false;
    bool success = false;
    if (epgImagePathSet) {
        success = LoadImage(cString::sprintf("%d", eventID), epgImagePath, "jpg");
    }
    if (!success) {
        success = LoadImage(cString::sprintf("%d", eventID), epgImagePathDefault, "jpg");
    }
    if (!success)
        return false;
    if (height != 0 || width != 0) {
        buffer.sample( Geometry(width, height));
    }
    return true;
}

bool cImageLoader::LoadAdditionalEPGImage(cString name) {
    int width = epgImageWidthLarge;
    int height = epgImageHeightLarge;
    if ((width == 0)||(height==0))
        return false;
    bool success = false;
    if (epgImagePathSet) {
        success = LoadImage(name, epgImagePath, "jpg");
    }
    if (!success) {
        success = LoadImage(name, epgImagePathDefault, "jpg");
    }
    if (!success)
        return false;
    if (height != 0 || width != 0) {
        buffer.sample( Geometry(width, height));
    }
    return true;
}

bool cImageLoader::LoadRecordingImage(cString Path) {
    int width = epgImageWidth;
    int height = epgImageHeight;
    if ((width == 0)||(height==0))
        return false;
    cString recImage("");
    if (FirstImageInFolder(Path, "jpg", &recImage)) {
        recImage = cString::sprintf("/%s", *recImage);
        if (!LoadImage(*recImage, Path, "jpg"))
            return false;
        buffer.sample( Geometry(width, height));
        return true;
    }
    return false;
}

bool cImageLoader::LoadAdditionalRecordingImage(cString path, cString name) {
    int width = epgImageWidthLarge;
    int height = epgImageHeightLarge;
    if ((width == 0)||(height==0))
        return false;
    if (LoadImage(name, path, "jpg")) {
        buffer.sample( Geometry(width, height));
        return true;
    }
    return false;
}

void cImageLoader::DrawBackground(tColor back, tColor blend, int width, int height, bool mirror) {
    Color Back = Argb2Color(back);
    Color Blend = Argb2Color(blend);
    Image tmp(Geometry(width, height), Blend);
    double arguments[9] = {0.0,(double)height,0.0,-1*(double)width,0.0,0.0,1.5*(double)width,0.0,1.0};
    tmp.sparseColor(MatteChannel, BarycentricColorInterpolate, 9, arguments);
    Image tmp2(Geometry(width, height), Back);
    tmp.composite(tmp2, 0, 0, OverlayCompositeOp);
    if (mirror)
        tmp.flop();
    buffer = tmp;
}

void cImageLoader::DrawBackground2(tColor back, tColor blend, int width, int height) {
    Color Back = Argb2Color(back);
    Color Blend = Argb2Color(blend);
    Image tmp(Geometry(width, height), Blend);
    double arguments[9] = {0.0,(double)height,0.0,-0.5*(double)width,0.0,0.0,0.75*(double)width,0.0,1.0};
    tmp.sparseColor(MatteChannel, BarycentricColorInterpolate, 9, arguments);
    Image tmp2(Geometry(width, height), Back);
    tmp.composite(tmp2, 0, 0, OverlayCompositeOp);
    buffer = tmp;
}

void cImageLoader::Colorize(tColor col)
{
    // opacity
    // 0 = opaque
    // QuantumRange = trans
    buffer.colorize(QuantumRange / 2, Argb2Color(col) );
}

cImage cImageLoader::GetImage() {
    int w, h;
    w = buffer.columns();
    h = buffer.rows();
    cImage image (cSize(w, h));
    const PixelPacket *pixels = buffer.getConstPixels(0, 0, w, h);
    for (int iy = 0; iy < h; ++iy) {
        for (int ix = 0; ix < w; ++ix) {
            tColor col = (~int(pixels->opacity * 255 / MaxRGB) << 24) 
            | (int(pixels->green * 255 / MaxRGB) << 8) 
            | (int(pixels->red * 255 / MaxRGB) << 16) 
            | (int(pixels->blue * 255 / MaxRGB) );
            image.SetPixel(cPoint(ix, iy), col);
            ++pixels;
        }
    }
    return image;
}

Color cImageLoader::Argb2Color(tColor col) {
    tIndex alpha = (col & 0xFF000000) >> 24;
    tIndex red = (col & 0x00FF0000) >> 16;
    tIndex green = (col & 0x0000FF00) >> 8;
    tIndex blue = (col & 0x000000FF);
    Color color(MaxRGB*red/255, MaxRGB*green/255, MaxRGB*blue/255, MaxRGB*(0xFF-alpha)/255);
    return color;
}

void cImageLoader::toLowerCase(std::string &str) {
    const int length = str.length();
    for(int i=0; i < length; ++i) {
        str[i] = std::tolower(str[i]);
    }
}

bool cImageLoader::LoadImage(cString FileName, cString Path, cString Extension) {
    try {
        cString File = cString::sprintf("%s%s.%s", *Path, *FileName, *Extension);
        dsyslog("imageloader: trying to load: %s", *File);
        //printf("imageloader: trying to load: %s\n", *File);
        buffer.read(*File);
        dsyslog("imageloader: %s sucessfully loaded", *File);
        //printf("imageloader: %s sucessfully loaded\n", *File);
    } catch (...) {     
        return false;
    }
    return true;
}

bool cImageLoader::FirstImageInFolder(cString Path, cString Extension, cString *recImage) {
    DIR *folder;
    struct dirent *file;
    folder = opendir(Path);
    while ( (file = readdir(folder)) ) {
        if (endswith(file->d_name, *Extension)) {
            std::string fileName = file->d_name;
            if (fileName.length() > 4)
                fileName = fileName.substr(0, fileName.length() - 4);
            else
                return false;
            *recImage = fileName.c_str();
            return true;
        }
    }
    return false;
}
