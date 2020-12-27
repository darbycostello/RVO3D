#pragma once

#include "CoreMinimal.h"
#include "RVO3DSimulatorComponent.generated.h"

namespace RVO
{
    class RVOSimulator;
}

class URVO3DAgentComponent;

/** 
 * RVO3D Simulator actor component. This component coordinates a pool of RVO3D agents 
 */
UCLASS(BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class RVO3D_API URVO3DSimulatorComponent final : public UActorComponent
{
	GENERATED_UCLASS_BODY()

    TSharedPtr<RVO::RVOSimulator> Simulator;
    TMap<URVO3DAgentComponent*, int32> AgentMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=RVO3D)
    float LockTimeAfterAvoid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=RVO3D)
    float LockTimeAfterGoalAdjustment;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=RVO3D)
    float GoalAdjustmentAngleOffset;

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category="RVO3D|Simulator")
    void AddAgentActor(AActor* AgentActor);

    UFUNCTION(BlueprintCallable, Category="RVO3D|Simulator")
    void RemoveAgentActor(AActor* AgentActor);

    UFUNCTION(BlueprintCallable, Category="RVO3D|Simulator")
    void AddAgentComponent(URVO3DAgentComponent* AgentComponent);

    UFUNCTION(BlueprintCallable, Category="RVO3D|Simulator")
    void RemoveAgentComponent(URVO3DAgentComponent* AgentComponent);

	UFUNCTION(BlueprintCallable, Category="RVO3D|Simulator")
    void UpdateIgnoredAgents(const URVO3DAgentComponent* AgentComponent);

	UFUNCTION(BlueprintCallable, Category="RVO3D|Simulator")
    void AddIgnoredAgent(const URVO3DAgentComponent* AgentComponent, const URVO3DAgentComponent* IgnoredAgent);

	UFUNCTION(BlueprintCallable, Category="RVO3D|Simulator")
    void RemoveIgnoredAgent(const URVO3DAgentComponent* AgentComponent, const URVO3DAgentComponent* IgnoredAgent);

	UFUNCTION(BlueprintCallable, Category="RVO3D|Simulator")
    void ClearIgnoredAgents(const URVO3DAgentComponent* AgentComponent, bool bAllowShrinking = false);

	UFUNCTION(BlueprintCallable, Category="RVO3D|Simulator")
	bool HasSimulator() const
	{
		return Simulator.IsValid();
	}

};
