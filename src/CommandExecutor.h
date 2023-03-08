#pragma once

#include <WString.h>

class CommandExecutor {
	protected:
        const char CommandSeparatorChar = ':';
		virtual void ParseCommand(String* command) = 0;
    
    public:
        virtual void ExecuteCommand(String* command) = 0;
};