#include "algorithm.h"
#include "QList"
#include "qline.h"
#include "commonDef.h"

using namespace CSHDetect;
using namespace HalconCpp;

Algorithm::Algorithm()
{

}

CError Algorithm::objIsEmpty(const HalconCpp::HObject &obj)
{
    try {
        bool ini = obj.IsInitialized();
        if (!ini) {
            return 1;
        }

        HObject null;
        HTuple number;
        GenEmptyObj(&null);
        TestEqualObj(obj, null, &number); //判定是否和空对象相等。

        ini = number == 1 ? 1 : 0;
        return ini;
    }  catch (...) {
        return CError(2, "Algorithm unexpected happened!");
    }
}

CError Algorithm::tupleisEmpty(const HTuple &tuple)
{
    try {
        HTuple hv_Length;
        TupleLength(tuple, &hv_Length);
        bool ini = (hv_Length.I() == 0);
        return ini;
    }  catch (...) {
        return CError(2, "Algorithm unexpected happened!");
    }
}

CircleInfo Algorithm::getAverCircle(QList<CircleInfo> info)
{
    CircleInfo res;
    int num = 0;
    double totalX = 0;
    double totalY = 0;
    double totalR = 0;
    for(auto i : info){
        totalX += i.X;
        totalY += i.Y;
        totalR += i.Radius;
        num ++;
    }

    res.X = totalX / num;
    res.Y = totalY / num;
    res.Radius = totalR / num;
    return res;
}

QPointF Algorithm::getCrossPoint(qreal pX, qreal pY, qreal lX1, qreal lY1, qreal lX2, qreal lY2)
{
    qreal lK = (lY2 - lY1)/(lX2 - lX1);
    qreal pK = -1/(lK);
    qreal lB = lY1 - lK * lX1;
    qreal pB = pY - pK * pX;

    qreal X = (pB - lB)/(lK - pK);
    qreal Y = lK * X + lB;

    return QPointF(X, Y);
}

LineInfo Algorithm::getCrossInfo(qreal pX, qreal pY, const LineInfo &info)
{
    LineInfo data = info;
    qreal lX1 = info.startCol;
    qreal lY1 = info.startRow;
    qreal lX2 = info.endCol;
    qreal lY2 = info.endRow;

    qreal lK = (lY2 - lY1)/(lX2 - lX1);
    qreal pK = -1/(lK);
    qreal lB = lY1 - lK * lX1;
    qreal pB = pY - pK * pX;

    qreal X = (pB - lB)/(lK - pK);
    qreal Y = lK * X + lB;

    data.pX = pX;
    data.pY = pY;
    data.crossX = X;
    data.crossY = Y;
    data.distance = QLineF(pX, pY, X, Y).length();

    return data;
}

const QString Algorithm::getErrDescri(const int errIndex)
{
    QString info;
    switch (errIndex) {
    case LACKAREA:
        info = "lackarea";
        break;
    case CUPOS:
        info = "cupos";
        break;
    case REGIONNUM:
        info = "regionnum";
        break;
    default:
        info = "Other";
    }
    return info;
}

HObject Algorithm::dynThre(const HObject &dst, const int threVal, const int meanSize)
{
    HObject ImageMean, dynImg;
    MeanImage(dst, &ImageMean, meanSize, meanSize);
    DynThreshold(dst, ImageMean, &dynImg, threVal, "light");
    return dynImg;
}
