#pragma once

#include "halconcpp/HalconCpp.h"
#include <QRect>
#include <QVector>
#include <QString>
#include <QPair>
#include <QMap>

#ifndef HOBJ
#define HOBJ  HalconCpp::HObject
#endif

struct RegionS{
    int width;
    int height;
    int TapeNum;

    HOBJ m_oriImg;
    HOBJ m_RImg;
    HOBJ m_GImg;
    HOBJ m_BImg;
    HOBJ m_hsvHImg;
    HOBJ m_hsvSImg;
    HOBJ m_hsvVImg;

    HOBJ BlackBangRegion;
    HOBJ DarkRegion;
    HOBJ WhiteRegion;
    HOBJ HSVRoi;
    HOBJ DeepRedRegion;
    HOBJ BlueRegion;
    HOBJ WeldRoi;
    HOBJ TapeRegion;
};

struct ReverRegionS{
    int width;
    int height;
    int batDire;

    HOBJ m_oriImg;

    HOBJ batteryRegion;
    HOBJ midRegion;
    HOBJ blueTapesReg;
    HOBJ dblTapeReg;

    HOBJ hanregion;
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
    double angle;    // 角度制
    HalconCpp::HTuple startRow;
    HalconCpp::HTuple startCol;
    HalconCpp::HTuple endRow;
    HalconCpp::HTuple endCol;
};

struct RectInfo
{
    int XBia;
    int YBia;

    int len1;
    int len2;
    int row;
    int col;
    double phi = 0;
};

struct MeasureposPam{
    int recLen1;
    int recLen2;
    int transition = 0;
    int pntSelect = 0;
    double sigma = 1;
    int threshold = 20;
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
