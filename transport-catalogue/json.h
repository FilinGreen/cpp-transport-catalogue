#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {
    using namespace std::literals;
    class Node;
   
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    //---------------------------------------Node-------------------------------------------
    class Node : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
    public:
  
        using variant::variant;

        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;
        bool IsBool() const;
        bool IsInt() const;
        bool IsDouble() const;
        bool IsString() const;
        bool IsPureDouble() const;


        const Array& AsArray() const;
        const Dict& AsMap() const;
        bool AsBool() const;
        int AsInt() const;
        double AsDouble() const;
        const std::string& AsString() const;

        const variant& GetValue() const;

        bool operator==(const Node& another) const;
    };

    inline bool operator!=(const Node& left, const Node& right) {
        return !(left == right);
    }

    //-----------------------------------VariantPrint---------------------------------------    
    struct VariantPrint {
        std::ostream& out;

        void operator() (std::nullptr_t) const;
        void operator() (const Array) const;
        void operator() (const Dict) const;
        void operator() (const bool) const;
        void operator() (const int) const;
        void operator() (const double) const;
        void operator() (const std::string) const;
    };

    //-------------------------------------Document-----------------------------------------
    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;


    private:
        Node root_;
    };

    Document Load(std::istream& input);
    inline bool operator==(const Document& left, const Document& right) {
        return left.GetRoot() == right.GetRoot();
    }

    inline bool operator!=(const Document& left, const Document& right) {
        return !(left == right);
    }

    //--------------------------------------Print------------------------------------------
    void Print(const Document& doc, std::ostream& output);

}  // namespace json
