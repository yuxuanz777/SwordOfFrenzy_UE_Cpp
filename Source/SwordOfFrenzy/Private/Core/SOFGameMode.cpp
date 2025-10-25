#include "Core/SOFGameMode.h"
#include "Characters/PlayerCharacter.h"   

ASOFGameMode::ASOFGameMode()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
}
