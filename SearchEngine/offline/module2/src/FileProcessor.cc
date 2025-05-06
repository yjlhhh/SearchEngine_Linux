
#include "FileProcessor.hpp"
#include "Configuration.hpp"
#include "tinyxml2.h"
#include "Mylogger.hpp"
//#include "GlobalDefine.hpp"
#include <iostream>
#include <regex>

namespace wd
{


void FileProcessor::process(const std::string & filename)
{
    using namespace tinyxml2;
	string id;
	string link;
	string title;
	string content;

    XMLDocument doc;
    doc.LoadFile(filename.c_str());
    if(doc.ErrorID()) {
        LogError("open file %s  error: %d", filename.c_str(), doc.ErrorID());
        return ;
    }

    XMLElement *itemNode = doc.FirstChildElement("rss")->FirstChildElement("channel")->FirstChildElement("item");
    if(itemNode == nullptr) {   return;  }

    do {
        XMLElement *titleNode = itemNode->FirstChildElement("title");
        XMLElement *linkNode = itemNode->FirstChildElement("link");
        XMLElement *descriptionNode = itemNode->FirstChildElement("description");
        XMLElement *contentNode = itemNode->FirstChildElement("content:encoded");

        if(titleNode) {  title = titleNode->GetText();    }
        if(linkNode) {  link = linkNode->GetText(); } 
        if(contentNode) {
            content = contentNode->GetText();
        } else if(descriptionNode) {
            content = descriptionNode->GetText();
        }

        //regex reg("<[^>]+>|&nbsp|&amp?|&lt|&gt");
        if(!content.empty()) {
            regex reg("<[^>]+>");
            content = regex_replace(content, reg, "");
        }

        /* cout << "title = " << title << endl; */
        /* cout << "link = " << link << endl; */
        /* cout << "content = \n" << content << endl; */

        string page;
        id = std::to_string(_vecPages.size() + 1);
        page.append("<doc>\n<docid>")
            .append(id).append("</docid>\n<docurl>")
            .append(link).append("</docurl>\n<doctitle>")
            .append(title).append("</doctitle>\n<doccontent>")
            .append(content).append("</doccontent>\n</doc>\n");
        _vecPages.push_back(std::move(page));
    }while((itemNode = itemNode->NextSiblingElement())!= nullptr);
}

}// end of namespace wd
