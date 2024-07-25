//
// Created by Alex on 2020/1/28.
//

#ifndef LSP_CLIENT_H
#define LSP_CLIENT_H

#include <lsp/transport.h>
#include <lsp/protocol.h>

namespace LspCore {

class LanguageClient : public JsonTransport {
public:
    LanguageClient(MapMessageHandler& msgHandler, JsonIOLayer& jsonIO) : JsonTransport(msgHandler, jsonIO){}
    virtual ~LanguageClient(){}
public:
    RequestID Initialize(option<DocumentUri> rootUri = {}) {
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
    //=============文档同步相关API=========================================
    void DidOpen(DocumentUri uri, string_ref text, string_ref languageId = "cpp") {
        DidOpenTextDocumentParams params;
        params.textDocument.uri = std::move(uri);
        params.textDocument.text = text;
        params.textDocument.languageId = languageId;
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
    //=============语言特性相关API=========================================
    // 跳转
    RequestID GoToDefinition(DocumentUri uri, Position position) {
        TextDocumentPositionParams params;
        params.textDocument.uri = std::move(uri);
        params.position = position;
        return SendRequest("textDocument/definition", std::move(params));
    }
    // 跳转
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
    // 获取文档的词语列表
    RequestID SemanticTokensALL(DocumentUri uri, ProgressToken token = "") {
        SemanticTokensParams params;
        params.textDocument.uri = uri;
        return SendRequest(METHOD_SemanticTokensFull, std::move(params));
    }
    RequestID RangeFomatting(DocumentUri uri, Range range) {
        DocumentRangeFormattingParams params;
        params.textDocument.uri = std::move(uri);
        params.range = range;
        return SendRequest("textDocument/rangeFormatting", params);
    }
    RequestID FoldingRange(DocumentUri uri) {
        FoldingRangeParams params;
        params.textDocument.uri = std::move(uri);
        return SendRequest("textDocument/foldingRange", params);
    }
    RequestID SelectionRange(DocumentUri uri, std::vector<Position> &positions) {
        SelectionRangeParams params;
        params.textDocument.uri = std::move(uri);
        params.positions = std::move(positions);
        return SendRequest("textDocument/selectionRange", params);
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
    RequestID Completion(DocumentUri uri, Position position, option<CompletionContext> context = {}) {
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
    RequestID DocumentColor(DocumentUri uri) {
        DocumentSymbolParams params;
        params.textDocument.uri = std::move(uri);
        return SendRequest("textDocument/documentColor", std::move(params));
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


    //=============工作区相关API=========================================
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
    RequestID SendRequest(string_ref method, value params = json()) {
        RequestID id = method.str();
        request(method, params, id);
        return id;
    }
    void SendNotify(string_ref method, value params = json()) {
        notify(method, params);
    }
};


}



#endif //LSP_CLIENT_H
