//
// Created by Alex on 2020/1/28.
//

#ifndef LSP_CLIENT_H
#define LSP_CLIENT_H
#include "transport.h"
#include "protocol.h"
#include "windows.h"

class LanguageClient : public JsonTransport {
public:
    explicit LanguageClient(const char *fCommandLine) : fCommandLine(fCommandLine) {
        SECURITY_ATTRIBUTES sa = {0};
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.bInheritHandle = true;
        if (!CreatePipe(&fReadIn, &fWriteIn, &sa, 0)) {
            printf("Create In Pipe error\n");
        }
        if (!CreatePipe(&fReadOut, &fWriteOut, &sa, 0)) {
            printf("Create Out Pipe error\n");
        }
        STARTUPINFO si = {0};
        si.cb = sizeof(si);
        si.hStdInput = fReadIn;
        si.hStdOutput = fWriteOut;
        si.dwFlags = STARTF_USESTDHANDLES;
        PROCESS_INFORMATION pi = {nullptr};
        if (!CreateProcessA(0, (char *) fCommandLine, 0, 0, TRUE, CREATE_NO_WINDOW, 0, 0, (LPSTARTUPINFOA)&si, &pi)){
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

    RequestID Initialize(option<DocumentUri> rootUri) {
        InitializeParams params;
        params.processId = GetCurrentProcessId();
        params.rootUri = rootUri;
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
    void Initialized() {
        SendNotify("initialized");
    }
    RequestID RegisterCapability() {
        return SendRequest("client/registerCapability");
    }
    void DidOpen(DocumentUri uri, string_ref text) {
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
    void DidChange(DocumentUri uri, std::vector<TextDocumentContentChangeEvent> &changes,
                   option<bool> wantDiagnostics = {}) {
        DidChangeTextDocumentParams params;
        params.textDocument.uri = std::move(uri);
        params.contentChanges = std::move(changes);
        params.wantDiagnostics = wantDiagnostics;
        SendNotify("textDocument/didChange", params);
    }
    RequestID RangeFomatting(DocumentUri uri, Range range) {
        DocumentRangeFormattingParams params;
        params.textDocument.uri = std::move(uri);
        params.range = range;
        return SendRequest("textDocument/rangeFormatting", params);
    }
    RequestID OnTypeFormatting(DocumentUri uri, Position position, string_ref ch) {
        DocumentOnTypeFormattingParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        params.ch = std::move(ch);
        return SendRequest("textDocument/onTypeFormatting", std::move(params));
    }
    RequestID Formatting(DocumentUri uri) {
        DocumentFormattingParams params;
        params.textDocument.uri = std::move(uri);
        return SendRequest("textDocument/formatting", std::move(params));
    }
    RequestID CodeAction(DocumentUri uri, Range range, CodeActionContext context) {
        CodeActionParams params;
        params.textDocument.uri = std::move(uri);
        params.range = range;
        params.context = std::move(context);
        return SendRequest("textDocument/codeAction", std::move(params));
    }
    RequestID Completion(DocumentUri uri, Position position, option<CompletionContext> context = option<CompletionContext>()) {
        CompletionParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        params.context = context;
        return SendRequest("textDocument/completion", params);
    }
    RequestID SignatureHelp(DocumentUri uri, Position position) {
        TextDocumentPositionParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        return SendRequest("textDocument/signatureHelp", std::move(params));
    }
    RequestID GoToDefinition(DocumentUri uri, Position position) {
        TextDocumentPositionParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        return SendRequest("textDocument/definition", std::move(params));
    }
    RequestID GoToDeclaration(DocumentUri uri, Position position) {
        TextDocumentPositionParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        return SendRequest("textDocument/declaration", std::move(params));
    }
    RequestID References(DocumentUri uri, Position position) {
        ReferenceParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        return SendRequest("textDocument/references", std::move(params));
    }
    RequestID SwitchSourceHeader(DocumentUri uri) {
        TextDocumentIdentifier params;
        params.uri = std::move(uri);
        return SendRequest("textDocument/references", std::move(params));
    }
    RequestID Rename(DocumentUri uri, Position position, string_ref newName) {
        RenameParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        params.newName = newName;
        return SendRequest("textDocument/rename", std::move(params));
    }
    RequestID Hover(DocumentUri uri, Position position) {
        TextDocumentPositionParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        return SendRequest("textDocument/hover", std::move(params));
    }

    RequestID DocumentSymbol(DocumentUri uri) {
        DocumentSymbolParams params;
        params.textDocument.uri = std::move(uri);

        return SendRequest("textDocument/documentSymbol", std::move(params));
    }
    RequestID DocumentHighlight(DocumentUri uri, Position position) {
        TextDocumentPositionParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        return SendRequest("textDocument/documentHighlight", std::move(params));
    }
    RequestID SymbolInfo(DocumentUri uri, Position position) {
        TextDocumentPositionParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        return SendRequest("textDocument/symbolInfo", std::move(params));
    }
    RequestID TypeHierarchy(DocumentUri uri, Position position, TypeHierarchyDirection direction, int resolve) {
        TypeHierarchyParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        params.direction = direction;
        params.resolve = resolve;
        return SendRequest("textDocument/typeHierarchy", std::move(params));
    }
    RequestID WorkspaceSymbol(string_ref query) {
        WorkspaceSymbolParams params;
        params.query = query;
        return SendRequest("workspace/symbol", std::move(params));
    }
    RequestID ExecuteCommand(string_ref cmd, option<TweakArgs> tweakArgs = {}, option<WorkspaceEdit> workspaceEdit = {}) {
        ExecuteCommandParams params;
        params.tweakArgs = tweakArgs;
        params.workspaceEdit = workspaceEdit;
        params.command = cmd;
        return SendRequest("workspace/executeCommand", std::move(params));
    }
    RequestID DidChangeWatchedFiles(std::vector<FileEvent> &changes) {
        DidChangeWatchedFilesParams params;
        params.changes = std::move(changes);
        return SendRequest("workspace/didChangeWatchedFiles", std::move(params));
    }
    RequestID DidChangeConfiguration(ConfigurationSettings &settings) {
        DidChangeConfigurationParams params;
        params.settings = std::move(settings);
        return SendRequest("workspace/didChangeConfiguration", std::move(params));
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
    void request(string_ref method, value &params, RequestID id) override {
        json rpc = {{"jsonrpc", jsonrpc},
                    {"id",      id},
                    {"method",  method},
                    {"params",  params}};
        writeJson(rpc);
    }
    void notify(string_ref method, value &params) override {
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
