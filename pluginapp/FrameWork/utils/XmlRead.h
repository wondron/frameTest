#pragma once
#include <iostream>
#include <map>
#include <string>
#include "tinyxml2.h"

struct xmlInfo{
    float           value;
    float			min;
    float           max;
    std::string     desc;
};
struct xmlSwitchInfo{
    bool            value;
    std::string     desc;
};

class XmlRead
{
	public:
		XmlRead();
		~XmlRead();
      std::map<std::string,std::map<std::string, xmlInfo>> parseXML(const char* algorithmFilePath);
      std::map<std::string,std::map<std::string, xmlSwitchInfo>> parseSwitchXML(const char* algorithmFilePath);
      std::map<std::string, xmlInfo> parseXML(const char* algorithmFilePath , const char* algorithmName);
      std::map<std::string, xmlSwitchInfo> parseSwitchXML(const char* algorithmFilePath , const char* algorithmName);
      bool saveXML(const char *algorithmFilePath,const std::map<std::string,std::map<std::string,float>> &info );
      bool saveXML(const char *algorithmFilePath,const std::map<std::string,std::map<std::string,bool>> &info );

private:
      tinyxml2::XMLElement* queryUserNodeByName(tinyxml2::XMLElement* root,const std::string& userName);

};
