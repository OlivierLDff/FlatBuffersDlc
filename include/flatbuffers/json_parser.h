#ifndef FLATBUFFERS_JSON_PARSER_H_
#define FLATBUFFERS_JSON_PARSER_H_

// Library Headers
#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/idl.h>

// Stl Headers
#include <string>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace flatbuffers {

class JsonParser
{
public:
    virtual ~JsonParser() = default;

    virtual bool strictJson() const = 0;
    virtual void setStrictJson(const bool strictJson) = 0;
    virtual int indentStep() const = 0;
    virtual void setIndentStep(const int indentStep) = 0;

    virtual bool parse(const char* json) = 0;
    virtual std::string error() const = 0;

    virtual const std::uint8_t* buffer() const = 0;
    virtual std::size_t size() const = 0;

    virtual const void* root() const = 0;
    virtual bool isValid() const = 0;
    virtual void reset() = 0;

    virtual bool generateText(const uint8_t* buffer, std::string& output) = 0;
};

/**
 * @brief      Wrap a flatbuffers::Parser and automatically
 *
 * @tparam     T      Root flatbuffers Table that will be parsed by this class
 * @tparam     Types  All generated resources files required to parse T.
 *                    Order should be from less inclusion to the more inclusion
 *                    Resources class are generated with 'flat2h'
 */
template<class T, class... Types>
class TJsonParser : public JsonParser
{
    static_assert(sizeof...(Types) >= 1, "TJsonParser required at least 1 generated resource class to parser T");
private:
    Parser _parser;
    bool _initialized = false;
    bool _strictJson = true;
    int _indentStep = -1;
    static std::shared_ptr<TJsonParser<T, Types ...>> _instance;

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
    bool strictJson() const override final
    {
        return _strictJson;
    }

    void setStrictJson(const bool strictJson) override final
    {
        _strictJson = strictJson;
    }

    int indentStep() const override final
    {
        return _indentStep;
    }

    void setIndentStep(const int indentStep) override final
    {
        _indentStep = indentStep < 0 ? -1 : indentStep;
    }

    bool parse(const char* json) override final
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

        _parser.opts.strict_json = _strictJson;
        if(!_parser.Parse(json, includePaths))
        {
            // When an error occured, it is safer to reinitialized the parser
            _initialized = true;
            return false;
        }
        return true;
    }

    std::string error() const override final { return _parser.error_; }

    const std::uint8_t* buffer() const override final { return _parser.builder_.GetBufferPointer(); }
    std::size_t size() const override final { return _parser.builder_.GetSize(); }


    const void* root() const override final { return buffer() ? flatbuffers::GetRoot<T>(buffer()) : nullptr; }
    const T* flatbuffer() const { return buffer() ? flatbuffers::GetRoot<T>(buffer()) : nullptr; }
    bool isValid() const override final
    {
        return Verifier(buffer(), size()).VerifyBuffer<T>(nullptr);
    }

    void reset() override { _initialized = false; }

    bool generateText(const std::uint8_t* buffer, std::string& output) override final
    {
        if (!_initialized)
        {
            if (!initializeParser())
                return false;
            _initialized = true;
        }
        _parser.opts.strict_json = _strictJson;
        _parser.opts.indent_step = _indentStep;

        return GenerateText(_parser, buffer, &output);
    }

public:
    static std::shared_ptr<TJsonParser<T, Types ...>> get() { return _instance; }
};

template<class T, class... Types>
std::shared_ptr<TJsonParser<T, Types ...>> TJsonParser<T, Types ...>::_instance = std::make_shared<TJsonParser<T, Types ...>>();

}

#endif
