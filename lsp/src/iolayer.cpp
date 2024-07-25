#include <lsp/iolayer.h>
#include <iostream>

namespace LspCore {

PipJsonIO::PipJsonIO(const char *program, const char *arguments)
    :JsonIOLayer()
{
    SECURITY_ATTRIBUTES sa = {0};
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = true;
    if (!CreatePipe(&fReadIn, &fWriteIn, &sa, 1024 * 1024)) {
        printf("Create In Pipe error\n");
    }
    if (!CreatePipe(&fReadOut, &fWriteOut, &sa, 1024 * 1024)) {
        printf("Create Out Pipe error\n");
    }

    // 设置非阻塞
    DWORD mode = PIPE_NOWAIT;
    if (!SetNamedPipeHandleState(fReadOut, &mode, NULL, NULL)) {

        std::cerr << "Failed to set non-blocking mode for readout pipe." << std::endl;
    }

    STARTUPINFO si = {0};
    si.cb = sizeof(si);
    si.hStdInput = fReadIn;
    si.hStdOutput = fWriteOut;
    si.dwFlags = STARTF_USESTDHANDLES;
    if (!CreateProcessA(program, (char *) arguments, 0, 0, TRUE,
                        CREATE_NO_WINDOW, 0, 0, (LPSTARTUPINFOA) &si, &fProcess)) {
        printf("Create Process error\n");
    }

    //m_exec.start(program, arguments);
    //m_exec.set_wait_timeout(exec_stream_t::s_child, INFINITE);
}

PipJsonIO::~PipJsonIO(){
    this->PipJsonIO::close();
}


void PipJsonIO::SkipLine() {
    char read;
    DWORD hasRead;
    while (ReadFile(fReadOut, &read, 1, &hasRead, NULL)) {
        if (read == '\n') {
            break;
        }
    }
}

int PipJsonIO::ReadLength() {
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

void PipJsonIO::Read(int length, std::string &out)
{
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
bool PipJsonIO::Write(std::string &in) {
    DWORD hasWritten;
    int writeSize = 0;
    size_t totalSize = in.length();
    while (WriteFile(fWriteIn, &in[writeSize], totalSize, &hasWritten, 0)) {
        writeSize += hasWritten;
        if (writeSize >= totalSize) {
            break;
        }
    }
    return true;
}

void PipJsonIO::close()
{
    std::lock_guard<std::mutex> _guard(m_closeLock);

    if(m_isClosed){
        return;
    }
    CloseHandle(fReadIn);
    CloseHandle(fWriteIn);
    CloseHandle(fReadOut);
    CloseHandle(fWriteOut);
    if (!TerminateProcess(fProcess.hProcess, 0)) {
        printf("teminate process error!\n");
    }
    if (!TerminateThread(fProcess.hThread, 0)) {
        printf("teminate thread error!\n");
    }
    CloseHandle(fProcess.hThread);
    CloseHandle(fProcess.hProcess);

    m_isClosed = true;
}

bool PipJsonIO::isClosed()
{
    std::lock_guard<std::mutex> _guard(m_closeLock);
    return m_isClosed;
}

bool PipJsonIO::readJson(json &json) {
    json.clear();
    int length = ReadLength();
    SkipLine();
    std::string read;
    Read(length, read);
    if(read.empty()){
        return false;
    }
    try {
        json = json::parse(read);
    } catch (nlohmann::detail::exception& e) {
        return false;
        printf("read error -> %s\nread -> %s\n ", e.what(), read.c_str());
    }
    return true;
}
bool PipJsonIO::writeJson(json &json)
{
    std::string content = json.dump(-1, ' ', false, nlohmann::detail::error_handler_t::ignore);
    std::string header = "Content-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
    return Write(header);
}

}

