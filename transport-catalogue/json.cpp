#include "json.h"

using namespace std;

namespace json {

    namespace {

        Node LoadNode(istream& input);

        Node LoadArray(istream& input) {
            Array result;

            int iters = 0;

            char c;
            input >> c;

            if (c == ']')
            {
                return Node(move(result));
            }

            input.putback(c);

            for (; input >> c && c != ']';) {

                if (c != ',') {
                    input.putback(c);
                }

                result.push_back(LoadNode(input));
                ++iters;
            }


            if (iters == 0 || c != ']')
            {

                throw ParsingError("invalid array");
            }

            return Node(move(result));
        }


        Node LoadString(std::istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // Поток закончился до того, как встретили закрывающую кавычку?
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    // Встретили закрывающую кавычку
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        // Поток завершился сразу после символа обратной косой черты
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        // Встретили неизвестную escape-последовательность
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    s.push_back(ch);
                }
                ++it;
            }

            return Node{ s };
        }


        Node LoadDict(istream& input) {
            Dict result;

            char c;
            input >> c;
            if (c == '}')
            {
                return Node(move(result));
            }

            input.putback(c);

            int iters = 0;

            for (; input >> c && c != '}';) {
                if (c == ',') {
                    input >> c;
                }

                string key = LoadString(input).AsString();
                input >> c;

                if (c != ':') {
                    throw ParsingError("invalid dictionary");
                }

                result.insert({ move(key), LoadNode(input) });
                ++iters;
            }
            
            if (iters == 0) {
                input.putback(c);
                throw ParsingError("invalid dictionary");
            }

            return Node(move(result));
        }

        std::string LoadLetters(std::istream& input) {
            std::string result;

            while (std::isalpha(static_cast<unsigned char>(input.peek())))
                result.push_back(static_cast<char>(input.get()));

            return result;
        }


        Node LoadNull(std::istream& input) {
            if (auto value = LoadLetters(input); value == "null"s)
                return Node{ nullptr };

            throw ParsingError(R"(Incorrect format for Null Node parsing. "null" expected)"s);
        }


        Node LoadBool(std::istream& input) {
            std::string value = LoadLetters(input);
            if (value == "true"s)
                return Node{ true };
            if (value == "false"s)
                return Node{ false };

            throw ParsingError(R"(Incorrect format for Boolean Node parsing. "true" or "false" expected)"s);
        }


        using Number = std::variant<int, double>;

        Number LoadNumber(std::istream& input) {
            using namespace std::literals;

            std::string parsed_num;

            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // Считывает одну или более цифр в parsed_num из input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // Сначала пробуем преобразовать строку в int
                    try {
                        return std::stoi(parsed_num);
                    }
                    catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }


        Node LoadNode(istream& input) {
            char c;
            if (!(input >> c))
                throw ParsingError("Incorrect format for Node parsing. Unexpected EOF"s);

            switch (c) {
            case 'n':
                input.putback(c);
                return LoadNull(input);
            case '[':
                return LoadArray(input);
            case '{':
                return LoadDict(input);
            case '"':
                return LoadString(input);
            case 't':
                input.putback(c);
                return LoadBool(input);
            case 'f':
                input.putback(c);
                return LoadBool(input);
            default:
                input.putback(c);
                Number num = LoadNumber(input);
                if (std::holds_alternative<int>(num)) {
                    return Node{ std::get<int>(num) };
                }
                else {
                    return Node{ std::get<double>(num) };
                }
            }
        }

    }  // namespace

    //-------------------------------------Node-----------------------------------------
   
    bool Node::IsNull() const {
        return std::holds_alternative<std::nullptr_t>(*this);
    }

    bool Node::IsArray() const {
        return std::holds_alternative<Array>(*this);
    }

    bool Node::IsMap() const {
        return std::holds_alternative<Dict>(*this);
    }

    bool Node::IsBool() const {
        return std::holds_alternative<bool>(*this);
    }

    bool Node::IsInt() const {
        return std::holds_alternative<int>(*this);
    }

    bool Node::IsDouble() const {
        return std::holds_alternative<double>(*this) || std::holds_alternative<int>(*this);
    }

    bool Node::IsString() const {
        return std::holds_alternative<std::string>(*this);
    }

    bool Node::IsPureDouble() const {
        return std::holds_alternative<double>(*this);
    }

    const Array& Node::AsArray() const {
        if (IsArray()) {
            return std::get<Array>(*this);
        }
        throw std::logic_error("different type of value");
    }

    const Dict& Node::AsMap() const {
        if (IsMap()) {
            return std::get<Dict>(*this);
        }
        throw std::logic_error("different type of value");
    }

    bool Node::AsBool() const {
        if (IsBool()) {
            return std::get<bool>(*this);
        }
        throw std::logic_error("different type of value");
    }

    int Node::AsInt() const {
        if (IsInt()) {
            return std::get<int>(*this);
        }
        throw std::logic_error("different type of value");
    }

    double Node::AsDouble() const {
        if (IsPureDouble()) {
            return std::get<double>(*this);
        }
        else if (IsInt()) {
            return static_cast<double>(std::get<int>(*this));
        }
        throw std::logic_error("different type of value");
    }

    const std::string& Node::AsString() const {
        if (IsString()) {
            return std::get<std::string>(*this);
        }
        throw std::logic_error("different type of value");
    }

    const Node::variant& Node::GetValue() const {
        return *this;
    }

    bool Node::operator==(const Node& another) const {
        return GetValue() == another.GetValue();
    }


    //-------------------------------------VariantPrint-----------------------------------------
    void VariantPrint::operator() (std::nullptr_t) const {
        out << "null";
    }

    void VariantPrint::operator() (const Array arr) const {
        out << "[";
        bool first = true;
        for (const auto& elem : arr) {
            if (first) {
                std::visit(VariantPrint{ out }, Node(elem).GetValue());
                first = false;
                continue;
            }
            out << ", ";
            std::visit(VariantPrint{ out }, Node(elem).GetValue());
        }
        out << "]";
    }

    void VariantPrint::operator() (const Dict dict) const {
        out << "{";
        bool first = true;
        for (const auto& [key, value] : dict) {
            if (first) {
                std::visit(VariantPrint{ out }, Node(key).GetValue());
                out << ": ";
                std::visit(VariantPrint{ out }, Node(value).GetValue());
                first = false;
                continue;
            }
            out << ", ";
            std::visit(VariantPrint{ out }, Node(key).GetValue());
            out << ": ";
            std::visit(VariantPrint{ out }, Node(value).GetValue());
        }
        out << "}";
    }

    void VariantPrint::operator() (const bool flag) const {
        out << (flag ? "true" : "false");
    }

    void VariantPrint::operator() (const int value) const {
        out << value;
    }

    void VariantPrint::operator() (const double value) const {
        out << value;
    }

    void VariantPrint::operator() (const std::string str) const {
        out << "\"";
        for (const auto& lit : str) {
            switch (lit) {
            case '\r':
                out << "\\r"s;
                break;
            case '\n':
                out << "\\n"s;
                break;
            case '\"':
                out << "\\\"";
                break;
            case '\\':
                out << "\\\\";
                break;

            default:
                out << lit;
                break;

            }
        }
        out << "\"";
    }

    //-------------------------------------Document-----------------------------------------    
    Document::Document(Node root)
        : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    void Print(const Document& doc, std::ostream& output) {
        std::visit(VariantPrint{ output }, (doc.GetRoot().GetValue()));
    }

}  // namespace json