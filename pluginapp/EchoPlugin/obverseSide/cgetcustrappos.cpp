#include "cgetcustrappos.h"
#include "XmlRead.h"
#include "../commonUsage/commonDef.h"
#include "QDebug"

CGetCuStrapPos::CGetCuStrapPos(const int wid, const int height)
{
    m_width = wid;
    m_height = height;
}

CGetCuStrapPos::CGetCuStrapPos(const QString name)
{
    taskName = name;
}

void CGetCuStrapPos::setImgSize(const int width, const int height)
{
    m_width = width;
    m_height = height;
}

CError CGetCuStrapPos::testPos(int& position, const HalconCpp::HObject& desImg, HalconCpp::HObject& resObj,
                              const int widHeight, const int maxThre, const int direction)
{
    try {
        CError res;

        if(maxThre >= 255 || maxThre <= 0)
            return CError(PAMVALUE, "CGetCuStrapPos::detect::maxThre value is out of range");

        if(widHeight <= 0)
            return CError(PAMVALUE, "CGetCuStrapPos::detect::width value is under 0");

        HObject region, connectReg;
        HTuple row, col, area;
        CHECKERR(Algorithm::objIsEmpty(desImg));

        Threshold(desImg, &region, maxThre, 255);
        Connection(region, &connectReg);
        SelectShapeStd(connectReg, &region, "max_area", 70);
        res = Algorithm::objIsEmpty(region);
        if(res.isWrong())
            return CError(EMPTYOBJ, "CGetCuStrapPos::detect get empty obj");

        resObj = region.Clone();

        AreaCenter(region, &area, &row, &col);

        if(direction == UPDOWN)
            position = row.D() > widHeight/2 ? DOWN : UP;
         else
            position = col.D() > widHeight/2 ? RIGHT : LEFT;

        return 0;

    }  catch (...) {
        return CError(UNEXCEPTION, "CGetCuStrapPos::detect(5 pam) unexcepted happen");
    }
}

CError CGetCuStrapPos::testPos(int &position, const HObject &desImg, HalconCpp::HObject &resObj, const int maxThre, const int direction)
{
    try {
        CError res;
        if(maxThre >= 255 || maxThre <= 0)
            return CError(PAMVALUE, "CGetCuStrapPos::detect::maxThre value is out of range");

        if(m_width <= 0 || m_height <= 0)
            return CError(PAMVALUE, "CGetCuStrapPos width and height is not setting");

        HObject region, connectReg;
        HTuple row, col, area;
        res = Algorithm::objIsEmpty(desImg);
        CHECKERR(res);

        Threshold(desImg, &region, maxThre, 255);
        Connection(region, &connectReg);
        SelectShapeStd(connectReg, &region, "max_area", 70);
        res = Algorithm::objIsEmpty(desImg);
        if(res.isWrong())
            return CError(EMPTYOBJ, "get empty obj");
        resObj = region.Clone();

        AreaCenter(region, &area, &row, &col);

        if(direction == UPDOWN)
            position = row.D() > m_height/2 ? DOWN : UP;
         else
            position = col.D() > m_width/2 ? RIGHT : LEFT;

        return 0;

    }  catch (...) {
        return CError(UNEXCEPTION, "CGetCuStrapPos::detect(4 pam) unexcepted happen");
    }
}

CError CGetCuStrapPos::detect(const HObject &desImg)
{
    try {
        HObject showObj;
        CError err;

        int CuPosition = 99;
        int widtOrHeight = (strapDirection == UPDOWN) ? m_height : m_width;

        err = testPos(CuPosition, desImg, showObj, widtOrHeight, maxThre, strapDirection);
        CHECKERR(err);

        if(CuPos != CuPosition)
            return CError(CUPOS, "CU positin is Error");

        return 0;

    }  catch (...) {
        return CError(UNEXCEPTION, "CGetCuStrapPos::detect(4 pam) unexcepted happen");
    }
}

CError CGetCuStrapPos::pamRead(const char *xmlfilePath)
{
    XmlRead xmlRead;

    try {
        std::map<std::string, xmlInfo> res = xmlRead.parseXML(xmlfilePath, taskName.toLocal8Bit().data());

        auto iterMBB = res.find("cuPos");
        if (iterMBB != res.end()) {
            CuPos = iterMBB->second.value;
        }

        auto iterMBA = res.find("maxThre");
        if (iterMBA != res.end()) {
            maxThre = iterMBA->second.value;
        }

        auto iterMBC = res.find("strapDirection");
        if (iterMBC != res.end()) {
            strapDirection = iterMBC->second.value;
        }
        return 0;

    } catch (...) {
        return CError(PAMREAD, "task: CGetCuStrapPos, pamRead failed!");
    }
}

