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
    virtual bool outputDefaultValues() const = 0;
    virtual void setOutputDefaultValues(const bool outputDefaultValues) = 0;

    virtual bool parse(const char* json) = 0;
    virtual std::string error() const = 0;

    virtual const std::uint8_t* buffer() const = 0;
    virtual std::size_t size() const = 0;

    virtual const void* root() const = 0;
    virtual bool isValid() const = 0;

    /**
     * Clear the parser and reset the config
     */
    virtual void reset() = 0;
    /** Only reset the config. */
    virtual void resetConfig() = 0;

    /**
     * \brief Generate a json formatted string from a row flatbuffer
     * If you need to dump a raw flatbuffer, use generateTextFromTable.
     *
     * Dump a flatbuffers::FlatBuffersBuilder :
     * \code
     * // ...
     * fbb.Finish();
     *
     * std::string json;
     * auto success = parser->generateText(fbb.GetBufferPointer(), json);
     * if(success)
     *    std::cout << "json : " << json << std::endl;
     * else
     *    std::cerr << "fail to generate json : " << parser->error() << std::endl;
     * \endcode
     *
     * \warning There is no check that flatbuffer is valid buffer, call with care
     *
     * \note The output of this function can be customized by changing
     * - strictJson: if true key are written in between "" otherwise key are raw string. (default true)
     * - indentStep: If -1, no line break. Otherwise line break + indentation in space. (default -1)
     * - outputDefaultValues: Values that are equals to default are going to be write. (default true)
     *
     * \param[in] flatbuffer pointer to the flatbuffer. This is the raw data not the table!!
     * So it's the data received from the network or from fbb.GetBufferPointer()
     * \param[out] output json output string
     * \return true if json generation is a success, false otherwise.
     * You can get any error using error()
     */
    virtual bool generateText(const void* flatbuffer, std::string& output) = 0;

    /**
     * \brief Generate a json formatted string from flatbuffer table.
     * If you need to dump a raw flatbuffer, use generateText.
     *
     * Dump a Monster.fbs table
     * \code
     * // ...
     * auto monster = GetMonster(buffer);
     *
     * std::string json;
     * auto success = parser->generateTextFromTable(monster, json);
     * if(success)
     *    std::cout << "json : " << json << std::endl;
     * else
     *    std::cerr << "fail to generate json : " << parser->error() << std::endl;
     * \endcode
     *
     * \warning There is no check that flatbuffer is valid buffer, call with care
     *
     * \note The output of this function can be customized by changing
     * - strictJson: if true key are written in between "" otherwise key are raw string. (default true)
     * - indentStep: If -1, no line break. Otherwise line break + indentation in space. (default -1)
     * - outputDefaultValues: Values that are equals to default are going to be write. (default true)
     *
     * \note This function require for the generated flatbuffer code to use --reflect-types or --reflect-names
     *
     * \param[in] table pointer to the flatbuffer. This is the raw data not the table!!
     * So it's the data received from the network or from fbb.GetBufferPointer()
     * \param[out] output json output string
     * \return true if json generation is a success, false otherwise.
     * You can get any error using error()
     */
    virtual bool generateTextFromTable(
        const void* table, std::string& output) = 0;
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
    static_assert(sizeof...(Types) >= 1,
        "TJsonParser required at least 1 generated resource class to parser T");

public:
    TJsonParser() : _parser(std::make_unique<Parser>()) {}

private:
    std::unique_ptr<Parser> _parser;
    bool _initialized = false;
    bool _strictJson = true;
    bool _outputDefaultValues = true;
    int _indentStep = -1;
    static std::shared_ptr<TJsonParser<T, Types...>> _instance;

private:
    bool initializeParser()
    {
        if(_initialized)
            return true;

        constexpr auto size = sizeof...(Types);

        static const char* data[size] = {Types::data()...};
        static const char* path[size] = {Types::path()...};

        for(std::size_t i = 0; i < size; ++i)
        {
            if(!_parser->Parse(data[i], nullptr, path[i]))
            {
                // This flag mean the parser will never be able to work again, it need a fix in the code
                // Program should crash and parser need to be fixed by developer !!!
                // Check value of _parser.error_
                assert(false);
                return false;
            }
        }
        _initialized = true;
        return true;
    }

public:
    bool strictJson() const override final { return _strictJson; }

    void setStrictJson(const bool strictJson) override final
    {
        _strictJson = strictJson;
    }

    int indentStep() const override final { return _indentStep; }

    void setIndentStep(const int indentStep) override final
    {
        _indentStep = indentStep < 0 ? -1 : indentStep;
    }

    bool outputDefaultValues() const override final
    {
        return _outputDefaultValues;
    }

    void setOutputDefaultValues(const bool outputDefaultValues) override final
    {
        _outputDefaultValues = outputDefaultValues;
    }

    bool parse(const char* json) override final
    {
        // Init parser
        if(!initializeParser())
            return false;

        // Set parser options
        _parser->opts.strict_json = _strictJson;

        // Parse the json string
        return _parser->Parse(json, nullptr);
    }

    std::string error() const override final { return _parser->error_; }

    const std::uint8_t* buffer() const override final
    {
        return _parser->builder_.GetBufferPointer();
    }
    std::size_t size() const override final
    {
        return _parser->builder_.GetSize();
    }

    const void* root() const override final
    {
        return buffer() ? flatbuffers::GetRoot<T>(buffer()) : nullptr;
    }
    const T* flatbuffer() const
    {
        return buffer() ? flatbuffers::GetRoot<T>(buffer()) : nullptr;
    }
    bool isValid() const override final
    {
        return Verifier(buffer(), size()).VerifyBuffer<T>(nullptr);
    }

    void reset() override final
    {
        _parser = std::make_unique<Parser>();
        _initialized = false;
        resetConfig();
    }

    void resetConfig() override final
    {
        _strictJson = true;
        _outputDefaultValues = true;
        _indentStep = -1;
    }

private:
    bool initGenerateText()
    {
        // Init parser
        if(!initializeParser())
            return false;

        // Set parser options
        _parser->opts.strict_json = _strictJson;
        _parser->opts.indent_step = _indentStep;
        _parser->opts.output_default_scalars_in_json = _outputDefaultValues;
        return true;
    }

public:
    bool generateText(
        const void* flatbuffer, std::string& output) override final
    {
        return initGenerateText() &&
               GenerateText(*_parser, flatbuffer, &output);
    }
    bool generateTextFromTable(
        const void* table, std::string& output) override final
    {
        return initGenerateText() && GenerateTextFromTable(*_parser, table,
                                         T::GetFullyQualifiedName(), &output);
    }

    static std::shared_ptr<TJsonParser<T, Types...>> make()
    {
        return std::make_shared<TJsonParser<T, Types...>>();
    }
    static std::shared_ptr<TJsonParser<T, Types...>> get()
    {
        // Always return an instance that isn't used in another thread or another context.
        // This will avoid thread safe issue
        if(_instance.use_count() > 1)
            _instance = make();
        return _instance;
    }
};

template<class T, class... Types>
std::shared_ptr<TJsonParser<T, Types...>> TJsonParser<T, Types...>::_instance =
    TJsonParser<T, Types...>::make();

}

#endif
