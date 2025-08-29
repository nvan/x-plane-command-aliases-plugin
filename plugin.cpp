#define IBM 1
#define XPLM200 1
#define XPLM_MSG_AIRCRAFT_LOADED 103

#include "XPLMPlugin.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"
#include "XPLMMenus.h"
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

struct Command {
    XPLMCommandRef original{};
    int state{0};
    XPLMCommandRef beginRef{};
    XPLMCommandRef endRef{};
    XPLMDataRef pressDR{};
};

static std::vector<Command *> commands;

static int getDataRef(void *ref) {
    return static_cast<Command *>(ref)->state;
}

static void setDataRef(void *ref, int v) {
    auto *c = static_cast<Command *>(ref);
    if (v != c->state) {
        c->state = v;
        if (v) XPLMCommandBegin(c->original);
        else XPLMCommandEnd(c->original);
    }
}

static int beginCommand(XPLMCommandRef, XPLMCommandPhase phase, void *ref) {
    auto *c = static_cast<Command *>(ref);
    if (phase != xplm_CommandBegin)
        return 0;

    XPLMCommandBegin(c->original);
    return 0;
}

static int endCommand(XPLMCommandRef, XPLMCommandPhase phase, void *ref) {
    auto *c = static_cast<Command *>(ref);
    if (phase != xplm_CommandBegin)
        return 0;

    XPLMCommandEnd(c->original);
    return 0;
}

static void removeRegisteredCommands() {
    for (auto *c: commands) {
        if (c->beginRef)
            XPLMUnregisterCommandHandler(c->beginRef, beginCommand, 0, c);
        if (c->endRef)
            XPLMUnregisterCommandHandler(c->endRef, endCommand, 0, c);
        if (c->pressDR)
            XPLMUnregisterDataAccessor(c->pressDR);
        delete c;
    }
    commands.clear();
}

static std::string getPluginDir() {
    char filePath[600];
    XPLMGetPluginInfo(XPLMGetMyID(), nullptr, filePath, nullptr, nullptr);

    return std::filesystem::path(filePath).parent_path().parent_path().string();
}

static void registerCommands() {
    removeRegisteredCommands();
    XPLMDebugString("[NVAN] Registering commands...\n");

    std::ifstream file((getPluginDir() + "/commands.txt").c_str());
    if (!file.is_open()) {
        XPLMDebugString("[NVAN] Cannot open commands.txt\n");
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        auto trim = [](std::string &s) {
            size_t a = s.find_first_not_of(" \t\r\n");
            size_t b = s.find_last_not_of(" \t\r\n");
            if (a == std::string::npos) {
                s.clear();
                return;
            }
            s = s.substr(a, b - a + 1);
        };

        trim(line);
        if (line.empty() || line[0] == '#') continue;

        XPLMCommandRef original = XPLMFindCommand(line.c_str());

        if (!original) {
            XPLMDebugString("[NVAN] Command not found: ");
            XPLMDebugString(line.c_str());
            XPLMDebugString("\n");
            continue;
        }

        auto *command = new Command();
        command->original = original;

        char buf[600];

        snprintf(buf, sizeof(buf), "%s_press", line.c_str());
        command->pressDR = XPLMRegisterDataAccessor(
            buf, xplmType_Int, 1,
            getDataRef, setDataRef,
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
            nullptr, nullptr, nullptr, nullptr,
            command, command
        );

        snprintf(buf, sizeof(buf), "%s_begin", line.c_str());
        command->beginRef = XPLMCreateCommand(buf, buf);
        XPLMRegisterCommandHandler(command->beginRef, beginCommand, 0, command);

        snprintf(buf, sizeof(buf), "%s_end", line.c_str());
        command->endRef = XPLMCreateCommand(buf, buf);
        XPLMRegisterCommandHandler(command->endRef, endCommand, 0, command);

        commands.push_back(command);
        XPLMDebugString("[NVAN] Registered command: ");
        XPLMDebugString(line.c_str());
        XPLMDebugString("\n");
    }

    XPLMDebugString("[NVAN] Finished registering commands!");
}


void menuHandler(void* inMenuRef, void* inItemRef) {
    if (inItemRef == (void*)1) {
        registerCommands();
    }
}

extern "C" {
PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc) {
    strcpy(outName, "NVAN X-Plane Command Aliases");
    strcpy(outSig, "es.nvan.xPlaneCommandAliasesPlugin");
    strcpy(outDesc, "Registers _press/_begin/_end aliases for commands to implement press and hold via UDP");

    XPLMMenuID pluginsMenu = XPLMFindPluginsMenu();
    int menuItem = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "NVAN Command Aliases", nullptr, 0);
    XPLMMenuID menu = XPLMCreateMenu("NVAN Command Aliases", pluginsMenu, menuItem, menuHandler, nullptr);
    XPLMAppendMenuItem(menu, "Reload commands.txt", (void*)1, 0);
    return 1;
}

PLUGIN_API int XPluginEnable(void) {
    registerCommands();
    return 1;
}

PLUGIN_API void XPluginDisable(void) {
    removeRegisteredCommands();
}

PLUGIN_API void XPluginStop(void) {
    XPluginDisable();
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID, int msg, void *) {
    if (msg == XPLM_MSG_AIRCRAFT_LOADED) {
        registerCommands();
    }
}
}
