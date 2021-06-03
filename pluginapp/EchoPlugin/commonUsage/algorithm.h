#pragma once
#include "halconcpp/HalconCpp.h"
#include "comStruct.h"
#include "cerror.h"
#include "qpoint.h"
#include "comStruct.h"

namespace CSHDetect {

#ifndef CONSTIMG
#define CONSTIMG const HalconCpp::HObject&
#endif

#ifndef RHOBJ
#define RHOBJ HalconCpp::HObject&
#endif

class Algorithm
{
public:
    Algorithm();

    static CError objIsEmpty(CONSTIMG obj);
    static CError tupleisEmpty(const HalconCpp::HTuple& tuple);
    static CircleInfo getAverCircle(QList<CircleInfo> info);

    static QPointF getCrossPoint(qreal pX, qreal pY, qreal lX1, qreal lY1, qreal lX2, qreal lY2);
    static LineInfo getCrossInfo(qreal pX, qreal pY, const LineInfo& info);

    static const QString getErrDescri(const int errIndex);

    static HalconCpp::HObject dynThre(CONSTIMG dst, const int threVal, const int meanSize);
};
}
