#include "duanhangroup.h"
#include "qobject.h"
#include "qvector.h"

class duanhanGroupPrivate
{
    public:
    duanhanGroupPrivate(duanhanGroup* parent) : p(parent)
    {
        Q_UNUSED(p);
    }

    qreal getMaxMinRatio()
    {
        return 1;
    };

public:
    qreal radio;
    QVector<qreal> lenths;
    qreal min;
    qreal max;
    int size;

private:
    duanhanGroup *p;
};

duanhanGroup::duanhanGroup() : d(new duanhanGroupPrivate(this))
{
    bigR = 630;
    smallR = 480;
}

bool duanhanGroup::getRegion(HObject img, HObject &obj, HObject &roiImage)
{
    HTuple number;
    HTuple hv_Exception;
    HObject threRegion, connectRegion, slctRegion, transRegion; //获取圆心所用对象
    HObject circleSmall, circleBig, ringRegion;                 //获取检测区域所使用
    HObject laplaceImage, closeRegion, openRegion;              //拉普拉斯所用变量
    HObject polarTransRegion;                                   //极坐标所用
    try {
        Threshold(img, &threRegion, 160, 255);
        Connection(threRegion, &connectRegion);
        SelectShape(connectRegion, &slctRegion, (HTuple("area").Append("row1")),
                    "and", (HTuple(80076).Append(400)), (HTuple(500000).Append(874.38)));
        ShapeTrans(slctRegion, &transRegion, "outer_circle");

        SelectShape(transRegion, &slctRegion, "ra", "or", 0, 400);
        AreaCenter(slctRegion, &hv_Area, &hv_Row, &hv_Column);
        GenCircle(&circleSmall, hv_Row, hv_Column,  smallR);
        GenCircle(&circleBig, hv_Row, hv_Column, bigR);
        Difference(circleBig, circleSmall, &ringRegion);
        ReduceDomain(img, ringRegion, &roiImage);

        Laplace(roiImage, &laplaceImage, "absolute", 3, "n_8_isotropic");
        Threshold(laplaceImage, &threRegion, 65, 255);
        ClosingCircle(threRegion, &closeRegion, 15);
        OpeningCircle(closeRegion, &openRegion, 4);
        Connection(openRegion, &connectRegion);
        SelectShape(connectRegion, &obj, "area", "or", 10558.44, 1000000);
        CountObj(obj, &number);

        if(number.I() < 1) return false;
        PolarTransRegion(obj, &polarTransRegion, hv_Row, hv_Column,
                         -1, 5.28, bigR, smallR, 1400, bigR - smallR, "nearest_neighbor");
        Connection(polarTransRegion, &connectRegion);
        ShapeTrans(connectRegion, &transRegion, "rectangle1");
        SelectShape(transRegion, &obj, "width", "or", 160, 50000);
        CountObj(obj, &number);
        return number.I() > 0;
    } catch (HException &HDevExpDefaultException) {
        HDevExpDefaultException.ToHTuple(&hv_Exception);
        return false;
    }
}

bool duanhanGroup::getPoints(HObject roiImg, HObject slctRegion)
{
    return false;
}
