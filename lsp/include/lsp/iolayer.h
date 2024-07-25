#ifndef IOLAYER_H
#define IOLAYER_H

#include <string>
#include <mutex>
#include <lsp/uri.h>
#include "windows.h"

namespace LspCore {
using value = json;

class JsonIOLayer{
public:
    virtual ~JsonIOLayer(){}

    virtual bool readJson(value &) = 0;
    virtual bool writeJson(value &) = 0;
    virtual void close() = 0;
    virtual bool isClosed() = 0;
};

class PipJsonIO : public JsonIOLayer{
private:
    HANDLE fReadIn = nullptr, fWriteIn = nullptr;
    HANDLE fReadOut = nullptr, fWriteOut = nullptr;
    PROCESS_INFORMATION fProcess = {nullptr};

    bool m_isClosed{false};
    std::mutex m_closeLock;
public:
    explicit PipJsonIO(const char *program, const char *arguments = "");
    virtual ~PipJsonIO();
private:
    void SkipLine();
    int ReadLength();
    void Read(int length, std::string &out);
    bool Write(std::string &in);
public:
    void close() override;
    bool isClosed() override;
    bool readJson(json &json) override;
    bool writeJson(json &json) override;
};

}
#endif // IOLAYER_H
