#pragma once

#include "halconcpp/HalconCpp.h"
#include <QRect>
#include <QVector>
#include <QString>
#include <QPair>
#include <QMap>

struct RegionS{
    int width;
    int height;

    HalconCpp::HObject m_oriImg;
    HalconCpp::HObject m_RImg;
    HalconCpp::HObject m_GImg;
    HalconCpp::HObject m_BImg;
    HalconCpp::HObject m_hsvHImg;
    HalconCpp::HObject m_hsvSImg;
    HalconCpp::HObject m_hsvVImg;

    HalconCpp::HObject BlackBangRegion;
    HalconCpp::HObject DarkRegion;
    HalconCpp::HObject WhiteRegion;
    HalconCpp::HObject HSVRoi;
    HalconCpp::HObject DeepRedRegion;
    HalconCpp::HObject BlueRegion;
    HalconCpp::HObject WeldRoi;
    HalconCpp::HObject TapeRegion;
};

struct ReverRegionS{
    int width;
    int height;

    HalconCpp::HObject m_oriImg;

    HalconCpp::HObject batteryRegion;
    HalconCpp::HObject midRegion;
    HalconCpp::HObject blueTapesReg;
    HalconCpp::HObject dblTapeReg;
};

struct CircleInfo{
    double X;
    double Y;
    double Radius;
};

struct LineInfo{
    double crossX;
    double crossY;
    double pX;
    double pY;
    double distance;
    HalconCpp::HTuple startRow;
    HalconCpp::HTuple startCol;
    HalconCpp::HTuple endRow;
    HalconCpp::HTuple endCol;
};

struct DetectRectInfo_t {
    QRect rect;
    DetectRectInfo_t():rect(QRect(0,0,0,0))
    {

    }
};

struct DetectCircleInfo_t {
    QPoint center;
    int radius;
    DetectCircleInfo_t():center(QPoint(0,0)),radius(0)
    {

    }
};

struct DetectLineInfo_t{
    QPoint startPoint;
    QPoint endPoint;
    double linewidth;
    double lineheight;
    DetectLineInfo_t():startPoint(QPoint(0,0)),endPoint(QPoint(0,0))
    {

    }
};

struct DetectPointInfo_t{
    QPoint point;
    DetectPointInfo_t():point(QPoint(0,0))
    {

    }
};
struct DetectTextInfo_t{
    QPair<QString,QString> text;
    DetectTextInfo_t():text(QPair<QString,QString>("",""))
    {
    }
};

struct DetectDefectInfo_t{
    QString clsName;
    int defectscope;
    bool result;
    bool otherImageDefect;
    DetectTextInfo_t detectAttribute;
    QVector<DetectRectInfo_t> Rects;
    QVector<DetectCircleInfo_t > Circles;
    QVector<DetectLineInfo_t> Lines;
    QVector<DetectPointInfo_t> Point;
    DetectDefectInfo_t():clsName(""), defectscope(0),result(true),otherImageDefect(false)
    {
    }

    bool operator ==(DetectDefectInfo_t other) {
        if(clsName == other.clsName && result == other.result && otherImageDefect == other.otherImageDefect){
            return true;
        }else{
            return false;
        }
    }
};



struct DetectResult_t {
     QVector<DetectRectInfo_t> detectRects;
     QVector<DetectCircleInfo_t > detectCircles;
     QVector<DetectLineInfo_t> detectLines;
     QVector<DetectPointInfo_t> detectPoint;
     QVector<DetectDefectInfo_t> detectDefect;
     DetectTextInfo_t detectResult;
     DetectTextInfo_t imageInfo;
     bool bGood;
     bool bTotolGood;
     DetectResult_t() {
         bGood = true;
         bTotolGood = true;
     }

     DetectResult_t operator +=(DetectResult_t other) {
         bGood = other.bGood && bGood;
         detectRects += other.detectRects;
         detectCircles += other.detectCircles;
         detectLines += other.detectLines;
         detectPoint += other.detectPoint;
         detectDefect += other.detectDefect;
         return *this;
     }

};

struct ImageData_t {
    const char* Data;
    int Width;
    int Height;
    int Channel;
    int depth;
    int nViewIndex;
    int nImageIndex;
    QString strQrCode;
    bool b3d;
};
