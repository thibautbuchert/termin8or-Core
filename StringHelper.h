#pragma once
#include "Math.h"
#include <string>
#include <algorithm>

namespace str
{
  std::string rep_char(char c, int num)
  {
    std::string ret;
    for (int i=0; i<num; ++i)
      ret += c;
    return ret;
  }
  
  // #FIXME: Align, not adjust.
  enum class Adjustment { Left, Center, Right, LeftInteger };
  std::string adjust_str(const std::string& str, Adjustment adj, int width, int start_idx = 0, char empty_char = ' ')
  {
    if (str.size() > width)
      return str.substr(0, width);
    
    auto offset_str = rep_char(empty_char, start_idx);
  
    int remaining = width - static_cast<int>(str.size());
    switch (adj)
    {
      case Adjustment::Left: return offset_str + str + rep_char(empty_char, remaining);
      case Adjustment::Center:
      {
        std::string result;
        if (width % 2 == 0)
        {
          int remain_half = remaining / 2;
          auto spaces = rep_char(empty_char, remain_half);
          result = offset_str + spaces + str + spaces;
        }
        else
        {
          // Prefer slightly left adjustment over right adjustment.
          int remain_left = (remaining - 1) / 2;
          int remain_right = (remaining + 1) / 2;
          auto spaces_left = rep_char(empty_char, remain_left);
          auto spaces_right = rep_char(empty_char, remain_right);
          result = offset_str + spaces_left + str + spaces_right;
        }
        result = result.substr(0, width);
        return result;
      }
      case Adjustment::Right:
      {
        auto result = offset_str + rep_char(empty_char, remaining) + str;
        result = result.substr(0, width);
        return result;
      }
      case Adjustment::LeftInteger:
      {
        int number_idx = static_cast<int>(str.size()) - 1;
        for (int i = 0; i <= 9; ++i)
        {
          auto needle = std::to_string(i);
          auto idx = static_cast<int>(str.find(needle));
          math::minimize<int>(number_idx, idx, { 0, str.size(), Range::Closed });
        }
        //  0               s          w
        // "bla:            1234       "
        auto result = rep_char(empty_char, width);
        for (size_t i = 0; i < std::min(start_idx, number_idx); ++i)
          result[i] = str[i];
        for (size_t i = 0; i < str.length(); ++i)
          if (start_idx + i < width && number_idx + i < str.length())
            result[start_idx + i] = str[number_idx + i];
        result = result.substr(0, width);
        return result;
      }
    }
  }

  // char or wchar_t
  template<typename char_t>
  char_t to_lower(char_t ch)
  {
    return std::tolower(ch);
  }

  // char or wchar_t
  template<typename char_t>
  char_t to_upper(char_t ch)
  {
    return std::toupper(ch);
  }

  bool is_wovel(char ch)
  {
    auto chl = tolower(ch);
    const char ch_aring = '\xE5';
    const char ch_auml = '\xE4';
    const char ch_ouml = '\xF6';
    std::vector<char> wovels { 'a', 'o', 'u', ch_aring, 'e', 'i', 'y', ch_auml, ch_ouml };
    return std::find(wovels.begin(), wovels.end(), chl) != wovels.end();
  }

  std::string cat(const std::vector<std::string>& strings)
  {
    std::string ret;
    for (const auto& str : strings)
      ret += str;
    return ret;
  }

  enum class BracketType { None, Parentheses, SquareBrackets, Braces, MatrixStyle };

  template<typename Cont>
  std::string row_vector(const Cont& c, BracketType bracket = BracketType::SquareBrackets, const std::string& separator = ", ")
  {
    std::string ret;
    switch (bracket)
    {
      case BracketType::None: break;
      case BracketType::Parentheses: ret = "("; break;
      case BracketType::SquareBrackets: ret = "["; break;
      case BracketType::Braces: ret = "{"; break;
      case BracketType::MatrixStyle: ret = "("; break;
    }
    ret += " ";
    ret += std::to_string(c.front());
    auto N = c.size();
    for (size_t e_idx = 1; e_idx < N; ++e_idx)
      ret += separator + std::to_string(c[e_idx]);
    ret += " ";
    switch (bracket)
    {
      case BracketType::None: break;
      case BracketType::Parentheses: ret += ")"; break;
      case BracketType::SquareBrackets: ret += "]"; break;
      case BracketType::Braces: ret += "}"; break;
      case BracketType::MatrixStyle: ret += ")"; break;
    }
    ret += "\n";
    return ret;
  }

  template<typename Cont>
  std::string column_vector(const Cont& c, BracketType bracket = BracketType::SquareBrackets)
  {
    std::vector<std::string> lines;
    size_t max_width = 0;
    int N = static_cast<int>(c.size());
    int l_idx = 0;
    for (const auto& v : c)
    {
      std::string str;
      switch (bracket)
      {
        case BracketType::None: break;
        case BracketType::Parentheses: str = "("; break;
        case BracketType::SquareBrackets: str = "["; break;
        case BracketType::Braces: str = "{"; break;
        case BracketType::MatrixStyle: str = N == 1 ? "(" : (l_idx == 0 ? "/" : (l_idx == N - 1 ? "\\" : "|")); break;
      }
      str += " ";
      str += std::to_string(v);
      lines.emplace_back(str);
      math::maximize(max_width, lines.back().size());
      l_idx++;
    }
    
    l_idx = 0;
    for (auto& str : lines)
    {
      str = adjust_str(str, Adjustment::Left, static_cast<int>(max_width));
      str += " ";
      switch (bracket)
      {
        case BracketType::None: break;
        case BracketType::Parentheses: str += ")"; break;
        case BracketType::SquareBrackets: str += "]"; break;
        case BracketType::Braces: str += "}"; break;
        case BracketType::MatrixStyle: str += N == 1 ? ")" : (l_idx == 0 ? "\\" : (l_idx == N - 1 ? "/" : "|")); break;
      }
      str += "\n";
      l_idx++;
    }
    
    return cat(lines);
  }

}
