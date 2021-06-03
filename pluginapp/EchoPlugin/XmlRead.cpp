#include "XmlRead.h"
//#include "setting.h"

XmlRead::XmlRead()
{
}

XmlRead::~XmlRead()
{
}

std::map<std::string,std::map<std::string, xmlInfo>> XmlRead::parseXML(const char* algorithmFilePath)
{
    std::map<std::string,std::map<std::string, xmlInfo>> resMap;
    std::map<std::string, xmlInfo> nodeMap; //存放结果，key对应参数名，value对应参数值
	resMap.clear();
    nodeMap.clear();

	//新建一个xmldocument对象
	tinyxml2::XMLDocument docXml;

	//调用对象的LoadFile函数加载文件，并将结果返回给loadRes
    tinyxml2::XMLError loadRes = docXml.LoadFile(algorithmFilePath);

	if (tinyxml2::XML_SUCCESS == loadRes)
	{
		//获取XML的root节点
        tinyxml2::XMLElement* root = docXml.RootElement();
        if (NULL == root)
        {
            std::cout << "Failed to load file: No root element." << std::endl;
            docXml.Clear();
            exit(1);
        }
        tinyxml2::XMLElement *p = root->FirstChildElement("Task");

        while (p)
        {
            const tinyxml2::XMLAttribute* parentNode = p->FirstAttribute();
            const char* xmlAlgorithmName = parentNode->Name();
            const char* xmlAlgorithmValue = parentNode->Value();
            std::string strAlgorithmName = xmlAlgorithmName;
            std::string strAlgorithmValue = xmlAlgorithmValue;
            if(strAlgorithmName == "name"){
                parentNode = parentNode->Next();
                const char* xmlTypeName = parentNode->Name();
                const char* xmlTypeValue = parentNode->Value();
                std::string strTypeName = xmlTypeName;
                std::string strTypeValue = xmlTypeValue;
                if(strTypeName == "type"){
                    if(strTypeValue == "float"){
                        tinyxml2::XMLElement* child = p->FirstChildElement();
                        nodeMap.clear();
                        while (child)
                        {
                            const tinyxml2::XMLAttribute* pNode = child->FirstAttribute();
                            std::string key = "";
                            std::string value = "";
                            xmlInfo m_xmlInfo;
                            while (pNode)
                            {
                                const char* attribName = pNode->Name();
                                const char* attribVal = pNode->Value();

                                std::string sName = attribName;
                                if ("name" == sName) //如果属性名字为“name”
                                {
                                    key = attribVal;
                                }
                                if ("value" == sName) //如果属性名字为“value”
                                {
                                    value = attribVal;
                                    if (key != "")
                                    {
                                        m_xmlInfo.value = std::stof(value);
                                    }
                                }
                                if ("min" == sName) //如果属性名字为“value”
                                {
                                    value = attribVal;
                                    if (key != "")
                                    {
                                        m_xmlInfo.min = std::stof(value);
                                    }
                                }
                                if ("max" == sName) //如果属性名字为“value”
                                {
                                    value = attribVal;
                                    if (key != "")
                                    {
                                        m_xmlInfo.max = std::stof(value);
                                    }
                                }
                                if ("desc" == sName) //如果属性名字为“value”
                                {
                                    m_xmlInfo.desc = attribVal;
                                    break;
                                }
                                pNode = pNode->Next();
                            }
                            nodeMap.insert(std::pair<std::string, xmlInfo>(key,m_xmlInfo));
                            child = child->NextSiblingElement();
                        }
                        if(!nodeMap.empty()){
                            resMap.insert(std::pair<std::string,std::map<std::string, xmlInfo>>(strAlgorithmValue,nodeMap));
                        }

                        p = p->NextSiblingElement();
                    }else {
                        p = p->NextSiblingElement();
                    }
                }else{
                   p = p->NextSiblingElement();
                }


            }else{
                p = p->NextSiblingElement();
            }
        }

	}
	else
	{
//        SysLog::log(LOG_ALARM,"",QObject::tr("配方文件打开错误，请检查xml是否存在"));
//        QMessageBox msgBox(QMessageBox::NoIcon,QString(QObject::tr("警告")),QObject::tr("配方文件打开错误，请检查xml是否存在"));
//        msgBox.setStyleSheet("QLabel {font:15px;color:#4d85db;}");
//        msgBox.addButton(QObject::tr("确定"), QMessageBox::AcceptRole);
//        msgBox.exec();
//        std::cout << "Error open file." << std::endl;
        //exit(1);
	}

    return resMap;
}

std::map<std::string, std::map<std::string, xmlSwitchInfo> > XmlRead::parseSwitchXML(const char *algorithmFilePath)
{
    std::map<std::string,std::map<std::string, xmlSwitchInfo>> resMap;
    std::map<std::string, xmlSwitchInfo> nodeMap; //存放结果，key对应参数名，value对应参数值
    resMap.clear();
    nodeMap.clear();

    //新建一个xmldocument对象
    tinyxml2::XMLDocument docXml;

    //调用对象的LoadFile函数加载文件，并将结果返回给loadRes
    tinyxml2::XMLError loadRes = docXml.LoadFile(algorithmFilePath);

    if (tinyxml2::XML_SUCCESS == loadRes)
    {
        //获取XML的root节点
        tinyxml2::XMLElement* root = docXml.RootElement();
        if (NULL == root)
        {
            std::cout << "Failed to load file: No root element." << std::endl;
            docXml.Clear();
            exit(1);
        }
        tinyxml2::XMLElement *p = root->FirstChildElement("Task");

        while (p)
        {
            const tinyxml2::XMLAttribute* parentNode = p->FirstAttribute();
            const char* xmlAlgorithmName = parentNode->Name();
            const char* xmlAlgorithmValue = parentNode->Value();
            std::string strAlgorithmName = xmlAlgorithmName;
            std::string strAlgorithmValue = xmlAlgorithmValue;
            if(strAlgorithmName == "name"){
                parentNode = parentNode->Next();
                const char* xmlTypeName = parentNode->Name();
                const char* xmlTypeValue = parentNode->Value();
                std::string strTypeName = xmlTypeName;
                std::string strTypeValue = xmlTypeValue;
                if(strTypeName == "type"){
                    if(strTypeValue == "switch"){
                        tinyxml2::XMLElement* child = p->FirstChildElement();
                        nodeMap.clear();
                        while (child)
                        {
                            const tinyxml2::XMLAttribute* pNode = child->FirstAttribute();
                            std::string key = "";
                            std::string value = "";
                            xmlSwitchInfo m_xmlInfo;
                            while (pNode)
                            {
                                const char* attribName = pNode->Name();
                                const char* attribVal = pNode->Value();

                                std::string sName = attribName;
                                if ("name" == sName) //如果属性名字为“name”
                                {
                                    key = attribVal;
                                }
                                if ("value" == sName) //如果属性名字为“value”
                                {
                                    value = attribVal;
                                    if (key != "")
                                    {
                                        if(value.compare("true") == 0){
                                            m_xmlInfo.value = true;
                                        }else {
                                            m_xmlInfo.value = false;
                                        }
                                    }

                                }
                                if ("desc" == sName) //如果属性名字为“value”
                                {
                                    m_xmlInfo.desc = attribVal;
                                    break;
                                }
                                pNode = pNode->Next();
                            }
                            nodeMap.insert(std::pair<std::string, xmlSwitchInfo>(key,m_xmlInfo));
                            child = child->NextSiblingElement();
                        }
                        if(!nodeMap.empty()){
                            resMap.insert(std::pair<std::string,std::map<std::string, xmlSwitchInfo>>(strAlgorithmValue,nodeMap));
                        }

                        p = p->NextSiblingElement();
                    }else {
                        p = p->NextSiblingElement();
                    }
                }else{
                   p = p->NextSiblingElement();
                }


            }else{
                p = p->NextSiblingElement();
            }
        }

    }
    else
    {

    }

    return resMap;
}

std::map<std::string, xmlInfo> XmlRead::parseXML(const char *algorithmFilePath, const char *algorithmName)
{
   std::map<std::string, xmlInfo> resMap;
    resMap.clear();

    //新建一个xmldocument对象
    tinyxml2::XMLDocument docXml;

    //调用对象的LoadFile函数加载文件，并将结果返回给loadRes
    tinyxml2::XMLError loadRes = docXml.LoadFile(algorithmFilePath);

    if (tinyxml2::XML_SUCCESS == loadRes)
    {
        //获取XML的root节点
        tinyxml2::XMLElement* root = docXml.RootElement();
        if (NULL == root)
        {
            std::cout << "Failed to load file: No root element." << std::endl;
            docXml.Clear();
            exit(1);
        }
        tinyxml2::XMLElement *p = root->FirstChildElement("Task");
        while (p)
        {
            const tinyxml2::XMLAttribute* parentNode = p->FirstAttribute();
            const char* xmlAlgorithmName = parentNode->Name();
            const char* xmlAlgorithmValue = parentNode->Value();
            std::string strAlgorithmName = xmlAlgorithmName;
            std::string strAlgorithmValue = xmlAlgorithmValue;
            if(strAlgorithmName == "name"){
                if(strAlgorithmValue == algorithmName){
                    parentNode = parentNode->Next();
                    const char* xmlTypeName = parentNode->Name();
                    const char* xmlTypeValue = parentNode->Value();
                    std::string strTypeName = xmlTypeName;
                    std::string strTypeValue = xmlTypeValue;
                    if(strTypeName == "type"){
                        if(strTypeValue == "float"){
                            tinyxml2::XMLElement* child = p->FirstChildElement();
                            //const tinyxml2::XMLAttribute* attrib = p->FirstAttribute();
                            //std::cout << attrib->Name() << ": " << attrib->Value() << std::endl; //输出Task的name和value
                            while (child)
                            {
                                //const char* name = child->Name(); //例如："regionW"
                                //std::string key = name;
                                //const char* val = child->GetText(); //例如："500"
                                //std::string value = val;
                                //resMap[key] = std::stof(value); //string转float
                                //child = child->NextSiblingElement();
                                const tinyxml2::XMLAttribute* pNode = child->FirstAttribute();
                                std::cout << pNode->Name() << ": " << pNode->Value() << std::endl; //输出属性的name和value
                                std::string key = "";
                                std::string value = "";
                                xmlInfo m_xmlInfo;
                                while (pNode)
                                {
                                    const char* attribName = pNode->Name();
                                    const char* attribVal = pNode->Value();

                                    std::string sName = attribName;
                                    if ("name" == sName) //如果属性名字为“name”
                                    {
                                        key = attribVal;
                                    }
                                    if ("value" == sName) //如果属性名字为“value”
                                    {
                                        value = attribVal;
                                        if (key != "")
                                        {
                                            m_xmlInfo.value = std::stof(value);
                                        }
                                    }
                                    if ("min" == sName) //如果属性名字为“value”
                                    {
                                        value = attribVal;
                                        if (key != "")
                                        {
                                            m_xmlInfo.min = std::stof(value);
                                        }
                                    }
                                    if ("max" == sName) //如果属性名字为“value”
                                    {
                                        value = attribVal;
                                        if (key != "")
                                        {
                                            m_xmlInfo.max = std::stof(value);
                                        }
                                    }
                                    if ("desc" == sName) //如果属性名字为“value”
                                    {
                                        m_xmlInfo.desc = attribVal;
                                        break;
                                    }
                                    pNode = pNode->Next();
                                }
                                resMap.insert(std::pair<std::string, xmlInfo>(key,m_xmlInfo));
                                child = child->NextSiblingElement();
                            }
                            break;

                        }else{
                            p = p->NextSiblingElement();
                        }
                    }else{
                       p = p->NextSiblingElement();
                    }
                }else{
                    p = p->NextSiblingElement();
                }


            }else{
                p = p->NextSiblingElement();
            }
        }
    }
    else
    {

    }

    return resMap;

}

std::map<std::string, xmlSwitchInfo> XmlRead::parseSwitchXML(const char *algorithmFilePath, const char *algorithmName)
{
    std::map<std::string, xmlSwitchInfo> resMap;
     resMap.clear();

     //新建一个xmldocument对象
     tinyxml2::XMLDocument docXml;

     //调用对象的LoadFile函数加载文件，并将结果返回给loadRes
     tinyxml2::XMLError loadRes = docXml.LoadFile(algorithmFilePath);

     if (tinyxml2::XML_SUCCESS == loadRes)
     {
         //获取XML的root节点
         tinyxml2::XMLElement* root = docXml.RootElement();
         if (NULL == root)
         {
             std::cout << "Failed to load file: No root element." << std::endl;
             docXml.Clear();
             exit(1);
         }
         tinyxml2::XMLElement *p = root->FirstChildElement("Task");
         while (p)
         {
             const tinyxml2::XMLAttribute* parentNode = p->FirstAttribute();
             const char* xmlAlgorithmName = parentNode->Name();
             const char* xmlAlgorithmValue = parentNode->Value();
             std::string strAlgorithmName = xmlAlgorithmName;
             std::string strAlgorithmValue = xmlAlgorithmValue;
             if(strAlgorithmName == "name"){
                 if(strAlgorithmValue == algorithmName){
                     parentNode = parentNode->Next();
                     const char* xmlTypeName = parentNode->Name();
                     const char* xmlTypeValue = parentNode->Value();
                     std::string strTypeName = xmlTypeName;
                     std::string strTypeValue = xmlTypeValue;
                     if(strTypeName == "type"){
                         if(strTypeValue == "switch"){
                             tinyxml2::XMLElement* child = p->FirstChildElement();
                             //const tinyxml2::XMLAttribute* attrib = p->FirstAttribute();
                             //std::cout << attrib->Name() << ": " << attrib->Value() << std::endl; //输出Task的name和value
                             while (child)
                             {
                                 //const char* name = child->Name(); //例如："regionW"
                                 //std::string key = name;
                                 //const char* val = child->GetText(); //例如："500"
                                 //std::string value = val;
                                 //resMap[key] = std::stof(value); //string转float
                                 //child = child->NextSiblingElement();
                                 const tinyxml2::XMLAttribute* pNode = child->FirstAttribute();
                                 std::cout << pNode->Name() << ": " << pNode->Value() << std::endl; //输出属性的name和value
                                 std::string key = "";
                                 std::string value = "";
                                 xmlSwitchInfo m_xmlInfo;
                                 while (pNode)
                                 {
                                     const char* attribName = pNode->Name();
                                     const char* attribVal = pNode->Value();

                                     std::string sName = attribName;
                                     if ("name" == sName) //如果属性名字为“name”
                                     {
                                         key = attribVal;
                                     }
                                     if ("value" == sName) //如果属性名字为“value”
                                     {
                                         value = attribVal;
                                         if (key != "")
                                         {
                                             if(value.compare("true") == 0){
                                                 m_xmlInfo.value = true;
                                             }else {
                                                 m_xmlInfo.value = false;
                                             }
                                         }

                                     }

                                     if ("desc" == sName) //如果属性名字为“value”
                                     {
                                         m_xmlInfo.desc = attribVal;
                                         break;
                                     }
                                     pNode = pNode->Next();
                                 }
                                 resMap.insert(std::pair<std::string, xmlSwitchInfo>(key,m_xmlInfo));
                                 child = child->NextSiblingElement();
                             }
                             break;

                         }else{
                             p = p->NextSiblingElement();
                         }
                     }else{
                        p = p->NextSiblingElement();
                     }
                 }else{
                     p = p->NextSiblingElement();
                 }


             }else{
                 p = p->NextSiblingElement();
             }
         }
     }
     else
     {

     }

     return resMap;
}

bool XmlRead::saveXML(const char *algorithmFilePath,const std::map<std::string, std::map<std::string, float> > &info)
{

    tinyxml2::XMLDocument docXml;
    bool result = false;

    //调用对象的LoadFile函数加载文件，并将结果返回给loadRes
    tinyxml2::XMLError loadRes = docXml.LoadFile(algorithmFilePath);

    if (tinyxml2::XML_SUCCESS == loadRes)
    {
        //获取XML的root节点
        tinyxml2::XMLElement* root = docXml.RootElement();
        if (NULL == root)
        {
            std::cout << "Failed to load file: No root element." << std::endl;
            docXml.Clear();
            exit(1);
        }
        //tinyxml2::XMLElement *p = root->FirstChildElement("Task");
        for(auto iter = info.cbegin();iter != info.cend();iter++){
            tinyxml2::XMLElement* userNode = queryUserNodeByName(root,iter->first);

            if(userNode != NULL){
                for(auto childIter = iter->second.cbegin();childIter != iter->second.cend();childIter++){
                    tinyxml2::XMLElement* childNode = queryUserNodeByName(userNode,childIter->first);
                    if(childNode != NULL){
                        float value = childIter->second;
                        float xmlValue = std::stof(childNode->Attribute("value"));
                        if(value != xmlValue){
                            childNode->SetAttribute("value",value);
                            result = true;
                        }
                    }
                }
            }
        }
        docXml.SaveFile(algorithmFilePath);
    }
    else
    {

    }



    return result;
}

bool XmlRead::saveXML(const char *algorithmFilePath, const std::map<std::string, std::map<std::string, bool> > &info)
{
    tinyxml2::XMLDocument docXml;
    bool result = false;

    //调用对象的LoadFile函数加载文件，并将结果返回给loadRes
    tinyxml2::XMLError loadRes = docXml.LoadFile(algorithmFilePath);

    if (tinyxml2::XML_SUCCESS == loadRes)
    {
        //获取XML的root节点
        tinyxml2::XMLElement* root = docXml.RootElement();
        if (NULL == root)
        {
            std::cout << "Failed to load file: No root element." << std::endl;
            docXml.Clear();
            exit(1);
        }
        //tinyxml2::XMLElement *p = root->FirstChildElement("Task");
        for(auto iter = info.cbegin();iter != info.cend();iter++){
            tinyxml2::XMLElement* userNode = queryUserNodeByName(root,iter->first);

            if(userNode != NULL){
                for(auto childIter = iter->second.cbegin();childIter != iter->second.cend();childIter++){
                    tinyxml2::XMLElement* childNode = queryUserNodeByName(userNode,childIter->first);
                    if(childNode != NULL){
                        bool value = childIter->second;
                        std::string strXMLValue = childNode->Attribute("value");
                        bool xmlValue;
                        if(strXMLValue.compare("true") == 0){
                           xmlValue = true;
                           if(value != xmlValue){
                               childNode->SetAttribute("value",value);
                               result = true;
                           }
                        }else if(strXMLValue.compare("false") == 0){
                            xmlValue = false;
                            if(value != xmlValue){
                                childNode->SetAttribute("value",value);
                                result = true;
                            }
                        }else{
                            childNode->SetAttribute("value",value);
                            result = true;
                        }

                    }
                }
            }
        }
        docXml.SaveFile(algorithmFilePath);
    }
    else
    {

    }
    return result;
}

tinyxml2::XMLElement *XmlRead::queryUserNodeByName(tinyxml2::XMLElement *root, const std::string &userName)
{
    tinyxml2::XMLElement* userNode = root->FirstChildElement();
    while(userNode!= NULL){
        const tinyxml2::XMLAttribute* parentNode = userNode->FirstAttribute();
        const char* rootName = parentNode->Name();
        const char* rootValue = parentNode->Value();
        std::string strmName = rootName;
        std::string strValue = rootValue;
        if(strValue==userName)
            break;
        userNode = userNode->NextSiblingElement();
    }
    return userNode;
}


