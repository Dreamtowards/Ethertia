//
// Created by Dreamtowards on 2022/5/4.
//

#ifndef ETHERTIA_STRINGS_H
#define ETHERTIA_STRINGS_H

#include <vector>
#include <string>
#include <algorithm>

class Strings
{
public:

    static std::string erase(std::string& str, char ch) {
        str.erase(std::remove(str.begin(), str.end(), ch), str.end());
        return str;
    }

    static std::string join(const std::string& delimiter, const std::vector<std::string>& ls) {
        std::stringstream ss;
        for (const std::string& s : ls) {
            if (ss.tellp() > 0)
                ss << delimiter;
            ss << s;
        }
        return ss.str();
    }



    template<typename... ARGS>
    static std::string fmt(const std::string& pat, ARGS... args) {
        std::stringstream ss;
        _fmt(ss, pat, args...);
        return ss.str();
    }

    template<typename OUT>
    static void _fmt(OUT& out, const std::string& pat) {
        out << pat;
    }

    template<typename OUT, typename A, typename... ARGS>
    static void _fmt(OUT& out, const std::string& pat, A a, ARGS... args) {
        int beg = pat.find('{');
        int end = pat.find('}', beg);
        int padding = 0;
        if (beg != -1 && beg+1 != end) {
            std::string slen = pat.substr(beg+1, end-(beg+1));
            padding = std::stoi(slen);
        }

        // pre
        out << (beg==-1? pat : pat.substr(0,beg));
        // val
        long vbeg = out.tellp();
        out << a;
        if (padding) {
            int vlen = (long)out.tellp() - vbeg;
            int pad = padding - vlen;
            for (int i = 0; i < pad; ++i) {
                out << ' ';
            }
        }

        // post
        _fmt(out, beg==-1? "" : pat.substr(end+1), args...);
    }


    template<typename T>
    static std::string str(T v) {
        std::stringstream ss;
        ss << v;
        return ss.str();
    }





    static std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
        std::vector<std::string> ls;
        u64 beg = 0;
        u64 end;
        while ((end = str.find(delimiter, beg)) != std::string::npos) {
            ls.push_back(str.substr(beg, end-beg));
            beg = end + delimiter.length();
        }
        ls.push_back(str.substr(beg));
        return ls;
    }


    static size_t findChar(const std::string_view& str, char ch, int beg) {
        for (int i = beg; i < str.length(); ++i) {
            if (str[i] == ch)
                return i;
        }
        return std::string::npos;
    }
    static size_t findNonChar(const std::string_view& str, char ch, int beg) {
        for (int i = beg; i < str.length(); ++i) {
            if (str[i] != ch)
                return i;
        }
        return std::string::npos;
    }

    static std::vector<std::string> splitConnected(const std::string& str, char ch) {
        std::vector<std::string> ls;
        u64 beg = findNonChar(str, ch, 0);
        if (beg == std::string::npos) return {};

        u64 end;
        while ((end = findChar(str, ch, beg)) != std::string::npos) {
            ls.push_back(str.substr(beg, end-beg));
            beg = findNonChar(str, ch, end);
            if (beg == std::string::npos)
                return ls;
        }
        ls.push_back(str.substr(beg));
        return ls;
    }
    static std::vector<std::string> splitSpaces(const std::string& str) {
        return splitConnected(str, ' ');
    }

    // split command parts by spaces. quoted by " or ' will be seen as a whole part.
//    static std::vector<std::string> splitCommand(const std::string& str) {
//
//    }
};

#endif //ETHERTIA_STRINGS_H
