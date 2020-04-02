#ifndef FLATBUFFERS_JSON_PARSER_H_
#define FLATBUFFERS_JSON_PARSER_H_

// Library Headers
#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/idl.h>

// Stl Headers
#include <cstddef>
#include <cstdint>

namespace flatbuffers {

/**
 * @brief      Wrap a flatbuffers::Parser and automatically
 *
 * @tparam     T      Root flatbuffers Table that will be parsed by this class
 * @tparam     Types  All generated resources files required to parse T.
 *                    Order should be from less inclusion to the more inclusion
 *                    Resources class are generated with 'flat2h'
 */
template<class T, class... Types>
class JsonParser
{
    static_assert(sizeof...(Types), "JsonParser required at least 1 generated resource class to parser T");
private:
    Parser _parser;
    bool _initialized = false;
    bool _strictJson = true;

private:
    bool initializeParser()
    {
        _initialized = true;

        constexpr auto size = sizeof...(Types);

        static const char* directory[size] = { Types::directory()... };
        static const char* data[size] = { Types::data()... };
        static const char* path[size] = { Types::path()... };

        const char* includePaths[] = { directory[0], nullptr };
        for (auto i = 0; i < size; ++i)
        {
            if (!_parser.Parse(data[i], includePaths, path[i]))
                return false;
        }
        return true;
    }
public:
    bool strictJson() const
    {
        return _strictJson;
    }

    void setStrictJson(const bool strictJson)
    {
        _strictJson = strictJson;
    }

    bool parse(const char* json)
    {
        if(!_initialized)
        {
            if (!initializeParser())
                return false;
            _initialized = true;
        }

        constexpr std::size_t size = sizeof...(Types);
        static const char* directory[size] = { Types::directory()... };
        const char* includePaths[] = { directory[0], nullptr };

        return _parser.Parse(json, includePaths);
    }

    std::string error() const { return _parser.error_; }

    const uint8_t* buffer() const { return _parser.builder_.GetBufferPointer(); }
    std::size_t length() const { return _parser.builder_.GetSize(); }

    const T* flatbuffer() const { return buffer() ? flatbuffers::GetRoot<T>(buffer()) : nullptr; }
    bool isValid() const
    {
        return Verifier(buffer(), length()).VerifyBuffer<T>(nullptr);
    }

    bool generateText(const uint8_t* buffer, std::string& output)
    {
        if (!_initialized)
        {
            if (!initializeParser())
                return false;
            _initialized = true;
        }
        _parser.opts.strict_json = _strictJson;
        
        return GenerateText(_parser, buffer, &output);
    }

    bool generateTextFromTable(const flatbuffers::Table* table, std::string& output)
    {
        if (!_initialized)
        {
            if (!initializeParser())
                return false;
            _initialized = true;
        }

        return GenerateText(_parser, table.getD, &output);
    }
};

}

#endif
