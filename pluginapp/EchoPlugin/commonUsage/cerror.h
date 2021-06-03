#pragma once
#include <QString>

namespace CSHDetect {
class CError
{
public:
    CError(int i= 0);
    CError(int i, QString info);
    ~CError(){}

    /**
     * @brief msg： 获取错误信息
     */
    QString msg() const {return errInfo;}

    /**
     * @brief code : 获取错误序列
     */
    int code() const {return errIndex;}

    /**
     * @brief setInfo: 设置错误信息
     */
    void setInfo(const QString &info);

    /**
     * @brief isWrong: 判定结果是都正确
     */
    bool isWrong() const {return errIndex != 0;}

    operator int();//用于int类型的直接赋值

private:
    int errIndex;
    QString errInfo;
};
}

