#include "mq_utils.h"

#include <iomanip>
#include "mq_exception.h"

using namespace std;
using namespace mq::http::sdk;

string MQUtils::getLastNodeFromURL(const string& inputURL)
{
    if (inputURL == "")
    {
        return "";
    }

    size_t pos = inputURL.find_last_of("/");
    if (pos == string::npos)
    {
        return inputURL;
    }

    return inputURL.substr(pos + 1);
}

string MQUtils::escapeJson(const std::string &s) {
    std::ostringstream o;
    for (std::string::const_iterator c = s.begin(); c != s.end(); c++) {
        switch (*c) {
        case '"': o << "\\\""; break;
        case '\\': o << "\\\\"; break;
        case '\b': o << "\\b"; break;
        case '\f': o << "\\f"; break;
        case '\n': o << "\\n"; break;
        case '\r': o << "\\r"; break;
        case '\t': o << "\\t"; break;
        default:
            if ('\x00' <= *c && *c <= '\x1f') {
                o << "\\u"
                  << std::hex << std::setw(4) << std::setfill('0') << (int)*c;
            } else {
                o << *c;
            }
        }
    }
    return o.str();
}

std::string MQUtils::toJsonStr(const std::map<std::string, std::string>& param)
{
    string str = "{";
	std::map<std::string, std::string>::const_reverse_iterator lastIter = param.rbegin();
    for (std::map<std::string, std::string>::const_iterator iter = param.begin();
            iter != param.end(); iter++)
    {
        str += "\"" + MQUtils::escapeJson(iter->first) + "\":\"" 
                        + MQUtils::escapeJson(iter->second) + "\"";
        if (iter->first != lastIter->first)
        {
            str += ",";
        }
    }
    str += "}";
    return str;
}

bool MQUtils::checkContainSpecialChar(const std::string& str)
{
    return str.find(":") != std::string::npos
        || str.find("|") != std::string::npos
        || str.find("'") != std::string::npos
        || str.find("&") != std::string::npos
        || str.find("\"") != std::string::npos
        || str.find("<") != std::string::npos
        || str.find(">") != std::string::npos;
}

void MQUtils::mapToString(const std::map<std::string, std::string>& param, std::string& o)
{
    std::map<std::string, std::string>::const_reverse_iterator lastIter = param.rbegin();
    for (std::map<std::string, std::string>::const_iterator iter = param.begin();
            iter != param.end(); iter++)
    {
        if (MQUtils::checkContainSpecialChar(iter->first) || MQUtils::checkContainSpecialChar(iter->second))
        {
            std::string errorMsg = "Message property[";
            errorMsg += iter->first;
            errorMsg += ":";
            errorMsg += iter->second;
            errorMsg += "] can't contains: & \" ' < > : |";
            MQ_THROW(MQExceptionBase, errorMsg);
        }
        o += iter->first;
        o += ":";
        o += iter->second;
        o += "|";
    }
}

void MQUtils::stringToMap(const std::string& param, std::map<std::string, std::string>& map)
{
    if (param.length() <= 0)
    {
        return;
    }
    size_t kvStart = 0, kvEnd;
    while ((kvEnd = param.find ("|", kvStart)) != string::npos)
    {
        std::string kv = param.substr(kvStart, kvEnd - kvStart);
        size_t index = kv.find(":");
        map[kv.substr(0, index)] = kv.substr(index + 1, kv.size());

        kvStart = kvEnd + 1;
        if (kvStart >= param.size())
        {
            break;
        }
    }
}

unsigned char ToHex(unsigned char x) 
{ 
    return  x > 9 ? x + 55 : x + 48; 
}

void MQUtils::urlEncode(const std::string& input, std::string& output)
{
    size_t length = input.length();
    for (size_t i = 0; i < length; i++)
    {
        if (isalnum((unsigned char)input[i]) || 
                (input[i] == '-') ||
                (input[i] == '_') || 
                (input[i] == '.') || 
                (input[i] == '~'))
            output += input[i];
        else if (input[i] == ' ')
            output += "+";
        else
        {
            output += '%';
            output += ToHex((unsigned char)input[i] >> 4);
            output += ToHex((unsigned char)input[i] % 16);
        }
    }
}

