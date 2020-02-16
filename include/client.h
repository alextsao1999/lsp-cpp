//
// Created by Alex on 2020/1/28.
//

#ifndef LSP_CLIENT_H
#define LSP_CLIENT_H
#include <utility>

#include "transport.h"
#include "windows.h"
#include "protocol.h"

class LanguageClient : public JsonTransport {
public:
    explicit LanguageClient(const char *fCommandLine) : fCommandLine(fCommandLine) {
        SECURITY_ATTRIBUTES sa = {0};
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.bInheritHandle = true;
        if (!CreatePipe(&fReadIn, &fWriteIn, &sa, 1024 * 1024)) {
            printf("Create In Pipe error\n");
        }
        if (!CreatePipe(&fReadOut, &fWriteOut, &sa, 1024 * 1024)) {
            printf("Create Out Pipe error\n");
        }
        STARTUPINFO si = {0};
        si.cb = sizeof(si);
        si.hStdInput = fReadIn;
        si.hStdOutput = fWriteOut;
        si.dwFlags = STARTF_USESTDHANDLES;
        PROCESS_INFORMATION pi = {nullptr};
        if (!CreateProcessA(0, (char *) fCommandLine, 0, 0, TRUE, CREATE_NO_WINDOW, 0, 0, &si, &pi)){
            printf("Create Process error\n");
        }
        fProcess = pi.hProcess;
    }
    ~LanguageClient() {
        CloseHandle(fReadIn);
        CloseHandle(fWriteIn);
        CloseHandle(fReadOut);
        CloseHandle(fWriteOut);
        CloseHandle(fProcess);
    }
    RequestID Initialize() {
        InitializeParams params;
        params.processId = GetCurrentProcessId();
        return SendRequest("initialize", params);
    }
    RequestID Shutdown() {
        return SendRequest("shutdown");
    }
    RequestID Sync() {
        return SendRequest("sync");
    }
    void Exit() {
        SendNotify("exit");
    }
    RequestID RegisterCapability() {
        return SendRequest("client/registerCapability");
    }
    void DidOpen(DocumentUri uri, std::string &text) {
        DidOpenTextDocumentParams params;
        params.textDocument.uri = std::move(uri);
        params.textDocument.languageId = "cpp";
        params.textDocument.text = text;
        SendNotify("textDocument/didOpen", params);
    }
    void DidClose(DocumentUri uri) {
        DidCloseTextDocumentParams params;
        params.textDocument.uri = std::move(uri);
        SendNotify("textDocument/didClose", params);
    }
    RequestID RangeFomatting() {

        return SendRequest("sync");
    }

public:
    void SkipLine() {
        char read;
        DWORD hasRead;
        while (ReadFile(fReadOut, &read, 1, &hasRead, NULL)) {
            if (read == '\n') {
                break;
            }
        }
    }
    int ReadLength() {
        // "Content-Length: "
        char szReadBuffer[255];
        DWORD hasRead;
        int length = 0;
        while (ReadFile(fReadOut, &szReadBuffer[length], 1, &hasRead, NULL)) {
            if (szReadBuffer[length] == '\n') {
                break;
            }
            length++;
        }
        return atoi(szReadBuffer + 16);
    }
    void Read(int length, std::string &out) {
        int readSize = 0;
        DWORD hasRead;
        out.resize(length);
        while (ReadFile(fReadOut, &out[readSize], length, &hasRead, NULL)) {
            readSize += hasRead;
            if (readSize >= length) {
                break;
            }
        }
    }
    bool Write(std::string &in) {
        DWORD hasWritten;
        int writeSize = 0;
        int totalSize = in.length();
        while (WriteFile(fWriteIn, &in[writeSize], totalSize, &hasWritten, 0)) {
            writeSize += hasWritten;
            if (writeSize >= totalSize) {
                break;
            }
        }
        return true;
    }
    bool readJson(json &json) override {
        printf("\n-------------------Listening-----------\n");
        int length = ReadLength();
        SkipLine();
        std::string read;
        Read(length, read);
        json = json::parse(read);
        printf("message:\n %s\n", read.c_str());
        return true;
    }
    bool writeJson(json &json) override {
        std::string content = json.dump();
        std::string header = "Content-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
        printf("send:\n%s\n", content.c_str());
        return Write(header);
    }

public:
    void request(string_ref method, value &params, RequestID id) {
        json rpc = {{"jsonrpc", jsonrpc},
                    {"id",      id},
                    {"method",  method},
                    {"params",  params}};
        writeJson(rpc);
    }
    void notify(string_ref method, value &params) {
        json value = {{"jsonrpc", jsonrpc},
                      {"method",  method},
                      {"params",  params}};
        writeJson(value);
    }
    RequestID SendRequest(string_ref method, value params = json()) {
        static RequestID id = 0;
        request(method, params, id);
        return id++;
    }
    void SendNotify(string_ref method, value params = json()) {
        notify(method, params);
    }

private:
    LPCSTR fCommandLine;
    HANDLE fReadIn = nullptr, fWriteIn = nullptr;
    HANDLE fReadOut = nullptr, fWriteOut = nullptr;
    HANDLE fProcess = nullptr;

};


#endif //LSP_CLIENT_H
