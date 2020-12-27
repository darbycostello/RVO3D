
#pragma once

#include "CoreMinimal.h"

class FRVO3D : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

