#include <lsp/protocol.h>

namespace LspCore {

JSON_Convert_Definition(URIForFile, {j = value.file;}, {value.file = j.get<std::string>();})
JSON_Convert_Definition(Position, MAP_JSON(MAP_KEY(line), MAP_KEY(character)), {FROM_KEY(line);FROM_KEY(character)})
JSON_Convert_Definition(Range, MAP_JSON(MAP_KEY(start), MAP_KEY(end)), {FROM_KEY(start);FROM_KEY(end)})
JSON_Convert_Definition(TextDocumentItem, MAP_JSON(MAP_KEY(uri), MAP_KEY(languageId), MAP_KEY(version), MAP_KEY(text)), {})
JSON_Convert_Definition(TextDocumentIdentifier, MAP_JSON(MAP_KEY(uri)), {})
JSON_Convert_Definition(VersionedTextDocumentIdentifier, MAP_JSON(MAP_KEY(uri), MAP_KEY(version)), {})
JSON_Convert_Definition(TextDocumentPositionParams, MAP_JSON(MAP_KEY(textDocument), MAP_KEY(position)), {})
JSON_Convert_Definition(TextEdit, MAP_JSON(MAP_KEY(range), MAP_KEY(newText)), { FROM_KEY(range); FROM_KEY(newText); });
JSON_Convert_Definition(Location, MAP_JSON(MAP_KEY(uri), MAP_KEY(range)), { FROM_KEY(uri); FROM_KEY(range); });
JSON_Convert_Definition(DiagnosticRelatedInformation, MAP_JSON(MAP_KEY(location), MAP_KEY(message)), { FROM_KEY(location); FROM_KEY(message); });
JSON_Convert_Definition(Diagnostic, {/*NOT REQUIRED*/ }, { FROM_KEY(range); FROM_KEY(code); FROM_KEY(source); FROM_KEY(message);
                FROM_KEY(relatedInformation); FROM_KEY(category); FROM_KEY(codeActions); });
JSON_Convert_Definition(MarkupContent, {}, { FROM_KEY(kind); FROM_KEY(value) });
JSON_Convert_Definition(WorkDoneProgressParams, MAP_JSON(MAP_KEY(workDoneToken)), {});
JSON_Convert_Definition(PartialResultParams, MAP_JSON(MAP_KEY(partialResultToken)), {});
JSON_Convert_Definition(ClientCapabilities,MAP_JSON(
                                       MAP_KV("textDocument",
                                              MAP_KV("publishDiagnostics", // PublishDiagnosticsClientCapabilities
                                                     MAP_TO("categorySupport", DiagnosticCategory),
                                                     MAP_TO("codeActionsInline", DiagnosticFixes),
                                                     MAP_TO("relatedInformation", DiagnosticRelatedInformation),
                                                     ),
                                              MAP_KV("completion", // CompletionClientCapabilities
                                                     MAP_KV("completionItem",
                                                            MAP_TO("snippetSupport", CompletionSnippets),
                                                            MAP_TO("deprecatedSupport", CompletionDeprecated)),
                                                     MAP_KV("completionItemKind", MAP_TO("valueSet", CompletionItemKinds)),
                                                     MAP_TO("editsNearCursor", CompletionFixes)
                                                     ),
                                              MAP_KV("codeAction", MAP_TO("codeActionLiteralSupport", CodeActionStructure)),
                                              MAP_KV("documentSymbol", MAP_TO("hierarchicalDocumentSymbolSupport", HierarchicalDocumentSymbol)),
                                              MAP_KV("hover",  //HoverClientCapabilities
                                                     MAP_TO("contentFormat", HoverContentFormat)),
                                              MAP_KV("signatureHelp", MAP_KV("signatureInformation", MAP_KV("parameterInformation", MAP_TO("labelOffsetSupport", OffsetsInSignatureHelp))))),
                                       MAP_KV("workspace", // WorkspaceEditClientCapabilities
                                              MAP_KV("symbol", // WorkspaceSymbolClientCapabilities
                                                     MAP_KV("symbolKind",
                                                            MAP_TO("valueSet", WorkspaceSymbolKinds))),
                                              MAP_TO("applyEdit", ApplyEdit),
                                              MAP_KV("workspaceEdit", // WorkspaceEditClientCapabilities
                                                     MAP_TO("documentChanges", DocumentChanges))),
                                       MAP_TO("offsetEncoding", offsetEncoding)), {});
JSON_Convert_Definition(ServerCapabilities, {}, {
    value.capabilities = j;
    FROM_KEY(textDocumentSync);
    j["documentOnTypeFormattingProvider"]["firstTriggerCharacter"].get_to(value.formattingTrigger);
    j["completionProvider"]["resolveProvider"].get_to(value.resolveProvider);
    j["completionProvider"]["triggerCharacters"].get_to(value.completionTrigger);
    j["executeCommandProvider"]["commands"].get_to(value.executeCommands);
});
JSON_Convert_Definition(ClangdCompileCommand,MAP_JSON(
                                         MAP_KEY(workingDirectory), MAP_KEY(compilationCommand)), {});
JSON_Convert_Definition(ConfigurationSettings, MAP_JSON(MAP_KEY(compilationDatabaseChanges)), {});
JSON_Convert_Definition(InitializationOptions, MAP_JSON(
                                          MAP_KEY(configSettings),
                                          MAP_KEY(compilationDatabasePath),
                                          MAP_KEY(fallbackFlags),
                                          MAP_KEY(clangdFileStatus)), {});
JSON_Convert_Definition(InitializeParams, MAP_JSON(
                                     MAP_KEY(processId),
                                     MAP_KEY(capabilities),
                                     MAP_KEY(rootUri),
                                     MAP_KEY(initializationOptions),
                                     MAP_KEY(rootPath)), {});
JSON_Convert_Definition(ShowMessageParams, {}, {FROM_KEY(type); FROM_KEY(message)});
JSON_Convert_Definition(Registration, MAP_JSON(MAP_KEY(id), MAP_KEY(method)), {});
JSON_Convert_Definition(RegistrationParams, MAP_JSON(MAP_KEY(registrations)), {});
JSON_Convert_Definition(UnregistrationParams, MAP_JSON(MAP_KEY(unregisterations)), {});
JSON_Convert_Definition(DidOpenTextDocumentParams, MAP_JSON(MAP_KEY(textDocument)), {});
JSON_Convert_Definition(DidCloseTextDocumentParams, MAP_JSON(MAP_KEY(textDocument)), {});
JSON_Convert_Definition(TextDocumentContentChangeEvent, MAP_JSON(MAP_KEY(range), MAP_KEY(rangeLength), MAP_KEY(text)), {});
JSON_Convert_Definition(DidChangeTextDocumentParams, MAP_JSON(MAP_KEY(textDocument), MAP_KEY(contentChanges), MAP_KEY(wantDiagnostics)), {});
JSON_Convert_Definition(FileEvent, MAP_JSON(MAP_KEY(uri), MAP_KEY(type)), {});
JSON_Convert_Definition(DidChangeWatchedFilesParams, MAP_JSON(MAP_KEY(changes)), {});
JSON_Convert_Definition(DidChangeConfigurationParams, MAP_JSON(MAP_KEY(settings)), {});
JSON_Convert_Definition(DocumentRangeFormattingParams, MAP_JSON(MAP_KEY(textDocument), MAP_KEY(range)), {});
JSON_Convert_Definition(DocumentOnTypeFormattingParams, MAP_JSON(MAP_KEY(textDocument), MAP_KEY(position), MAP_KEY(ch)), {});
JSON_Convert_Definition(DeclarationParams, MAP_JSON(MAP_KEY(textDocument), MAP_KEY(position), MAP_KEY(workDoneToken), MAP_KEY(partialResultToken)), {});
JSON_Convert_Definition(DefinitionParams, MAP_JSON(MAP_KEY(textDocument), MAP_KEY(position), MAP_KEY(workDoneToken), MAP_KEY(partialResultToken)), {});
JSON_Convert_Definition(TypeDefinitionParams, MAP_JSON(MAP_KEY(textDocument), MAP_KEY(position), MAP_KEY(workDoneToken), MAP_KEY(partialResultToken)), {});
JSON_Convert_Definition(ImplementationParams, MAP_JSON(MAP_KEY(textDocument), MAP_KEY(position), MAP_KEY(workDoneToken), MAP_KEY(partialResultToken)), {});
JSON_Convert_Definition(ReferenceParams, MAP_JSON(MAP_KEY(textDocument), MAP_KEY(position), MAP_KEY(workDoneToken), MAP_KEY(partialResultToken)),{});
JSON_Convert_Definition(HoverParam, MAP_JSON(MAP_KEY(textDocument), MAP_KEY(position), MAP_KEY(workDoneToken)), {});
JSON_Convert_Definition(Hover, {}, { FROM_KEY(contents); FROM_KEY(range) });
JSON_Convert_Definition(FoldingRangeParams, MAP_JSON(MAP_KEY(textDocument)), {});
JSON_Convert_Definition(FoldingRange, {}, {
    FROM_KEY(startLine);
    FROM_KEY(startCharacter);
    FROM_KEY(endLine);
    FROM_KEY(endCharacter);
    FROM_KEY(kind);
});
JSON_Convert_Definition(SelectionRangeParams, MAP_JSON(MAP_KEY(textDocument), MAP_KEY(positions)), {});
JSON_Convert_Definition(SelectionRange, {}, {
    FROM_KEY(range);
    if (j.contains("parent")) {
        value.parent = std::make_unique<SelectionRange>();
        j.at("parent").get_to(*value.parent);
    }
});
JSON_Convert_Definition(SemanticTokensParams, MAP_JSON(MAP_KEY(textDocument), MAP_KEY(workDoneToken), MAP_KEY(partialResultToken)), {});
JSON_Convert_Definition(DocumentFormattingParams, MAP_JSON(MAP_KEY(textDocument)), {});
JSON_Convert_Definition(DocumentSymbolParams, MAP_JSON(MAP_KEY(textDocument)), {});
JSON_Convert_Definition(PublishDiagnosticsParams, {}, {FROM_KEY(uri);FROM_KEY(diagnostics);});
JSON_Convert_Definition(CodeActionContext, MAP_JSON(MAP_KEY(diagnostics)), {});
JSON_Convert_Definition(WorkspaceEdit, MAP_JSON(MAP_KEY(changes)), {});
JSON_Convert_Definition(TweakArgs, MAP_JSON(MAP_KEY(file), MAP_KEY(selection), MAP_KEY(tweakID)),{})
JSON_Convert_Definition(ExecuteCommandParams, MAP_JSON(MAP_KEY(command), MAP_KEY(workspaceEdit), MAP_KEY(tweakArgs)),{})
JSON_Convert_Definition(CodeActionParams, MAP_JSON(MAP_KEY(textDocument), MAP_KEY(range), MAP_KEY(context)), {});
JSON_Convert_Definition(CodeAction, MAP_JSON(MAP_KEY(title), MAP_KEY(kind), MAP_KEY(diagnostics), MAP_KEY(edit), MAP_KEY(command)),
               {FROM_KEY(title);FROM_KEY(kind);FROM_KEY(diagnostics);FROM_KEY(edit);FROM_KEY(command)});
JSON_Convert_Definition(SymbolInformation, MAP_JSON(MAP_KEY(name), MAP_KEY(kind), MAP_KEY(location), MAP_KEY(containerName)), {FROM_KEY(name);FROM_KEY(kind);FROM_KEY(location);FROM_KEY(containerName)});
JSON_Convert_Definition(WorkspaceSymbolParams, MAP_JSON(MAP_KEY(query)), {});
JSON_Convert_Definition(ApplyWorkspaceEditParams, MAP_JSON(MAP_KEY(edit)), {});
JSON_Convert_Definition(CompletionContext, MAP_JSON(MAP_KEY(triggerKind), MAP_KEY(triggerCharacter)), {});
JSON_Convert_Definition(CompletionParams, MAP_JSON(MAP_KEY(context), MAP_KEY(textDocument), MAP_KEY(position)), {});
JSON_Convert_Definition(CompletionItem, {}, {
    FROM_KEY(label);
    FROM_KEY(kind);
    FROM_KEY(detail);
    FROM_KEY(documentation);
    FROM_KEY(sortText);
    FROM_KEY(filterText);
    FROM_KEY(insertText);
    FROM_KEY(insertTextFormat);
    FROM_KEY(textEdit);
    FROM_KEY(additionalTextEdits);
});
JSON_Convert_Definition(CompletionList, {}, {
    FROM_KEY(isIncomplete);
    FROM_KEY(items);
});
JSON_Convert_Definition(ParameterInformation, {}, {
    FROM_KEY(labelString);
    FROM_KEY(labelOffsets);
    FROM_KEY(documentation);
});
JSON_Convert_Definition(SignatureInformation, {}, {
    FROM_KEY(label);
    FROM_KEY(documentation);
    FROM_KEY(parameters);
});
JSON_Convert_Definition(SignatureHelp, {}, {
    FROM_KEY(signatures);
    FROM_KEY(activeParameter);
    FROM_KEY(argListStart);
});
JSON_Convert_Definition(RenameParams, MAP_JSON(MAP_KEY(textDocument), MAP_KEY(position), MAP_KEY(newName)), {});
JSON_Convert_Definition(TypeHierarchyParams, MAP_JSON(MAP_KEY(resolve), MAP_KEY(direction), MAP_KEY(textDocument), MAP_KEY(position)), {});

}

