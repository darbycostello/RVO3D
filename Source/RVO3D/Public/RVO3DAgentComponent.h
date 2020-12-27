
#pragma once

#include "CoreMinimal.h"
#include "AI/Navigation/NavigationAvoidanceTypes.h"
#include "RVO3DAgentComponent.generated.h"

class URVO3DSimulatorComponent;
class UMovementComponent;

/** 
 * RVO3D Agent actor component. This component manages the avoidance profile for the owning actor
 */
UCLASS(BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class RVO3D_API URVO3DAgentComponent final : public UActorComponent
{
	GENERATED_UCLASS_BODY()

    // Velocity result of RVO simulation
	UPROPERTY(BlueprintReadOnly, Transient, Category=RVO3D)
    FVector AvoidanceVelocity;

	// Locked preferred velocity
	UPROPERTY(BlueprintReadOnly, Transient, Category=RVO3D)
    float PreferredVelocityLockTimer;

	// The preferred velocity of the agent
	UPROPERTY(BlueprintReadOnly, Transient, Category=RVO3D)
    FVector PreferredVelocity;

	UPROPERTY(BlueprintReadOnly, Transient, Category=RVO3D)
    bool bRequireGoalAdjustment;

	UPROPERTY(BlueprintReadOnly, Transient, Category=RVO3D)
    float GoalAdjustmentLockTimer;

	UPROPERTY(BlueprintReadOnly, Transient, Category=RVO3D)
    FVector GoalAdjustment;

    // Agents to ignore
	UPROPERTY(BlueprintReadOnly, Category=RVO3D)
    TSet<const URVO3DAgentComponent*> IgnoredAgents;

	// Enabled by default. This will find and register the owner's movement component
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=RVO3D)
    bool bAutoRegisterMovementComponent;

	// The default maximal number of other agents a new agent takes into account in the navigation. The larger this number, the longer the running time of the simulation. If the number is too low, the simulation will not be safe.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=RVO3D)
    int32 MaxNeighbours;

	// The default maximal distance (center point to center point) to other agents a new agent takes into account in the navigation. The larger this number, the longer he running time of the simulation. If the number is too low, the simulation will not be safe. Must be non-negative.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=RVO3D)
    float NeighbourDistance;

	// The default minimal amount of time for which a new agent's velocities that are computed by the simulation are safe with respect to other agents. The larger this number, the sooner an agent will respond to the presence of other agents, but the less freedom the agent has in choosing its velocities. Must be positive.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=RVO3D)
    float TimeHorizon;

	// The default radius of a new agent. Must be non-negative.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=RVO3D)
    float AgentRadius;

	// Moving actor's group mask
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=RVO3D, AdvancedDisplay)
	FNavAvoidanceMask AvoidanceGroup;

	// Will avoid other agents if they are in one of specified groups
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=RVO3D, AdvancedDisplay)
	FNavAvoidanceMask GroupsToAvoid;

	// Will NOT avoid other agents if they are in one of specified groups, higher priority than GroupsToAvoid
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=RVO3D, AdvancedDisplay)
	FNavAvoidanceMask GroupsToIgnore;

    void RegisterSimulatorDependency();
    void UnregisterSimulatorDependency();

protected:

	UPROPERTY(BlueprintReadOnly, Transient, DuplicateTransient, Category=RVO3D)
    URVO3DSimulatorComponent* SimulatorComponent;

	UPROPERTY(BlueprintReadOnly, Transient, DuplicateTransient, Category=RVO3D)
    UMovementComponent* MovementComponent;

public:

	virtual void InitializeComponent() override;
	virtual void OnRegister() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
    void SetMovementComponent(UMovementComponent* InMovementComponent);

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
    bool HasSimulator() const;

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
    void SetSimulatorActor(AActor* InSimulatorActor);

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
    void SetSimulatorComponent(URVO3DSimulatorComponent* InSimulatorComponent);

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
    void ResetSimulatorComponent();

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	void SetAvoidanceGroupMask(const FNavAvoidanceMask& GroupMask);

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	void SetGroupsToAvoidMask(const FNavAvoidanceMask& GroupMask);

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	void SetGroupsToIgnoreMask(const FNavAvoidanceMask& GroupMask);

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	void UpdateIgnoredAgents();

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	void AddIgnoredAgent(const URVO3DAgentComponent* AgentComponent);

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	void RemoveIgnoredAgent(const URVO3DAgentComponent* AgentComponent);

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	void ClearIgnoredAgents(bool bAllowShrinking = true);

	// Avoidance velocity presents when preferred direction and avoidance direction is not coincident
	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	bool HasAvoidanceVelocity() const {
		return ! FVector::Coincident(GetPreferredVelocity().GetSafeNormal(), GetAvoidanceVelocity().GetSafeNormal());
	}

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	void SetAvoidanceVelocity(const FVector& Velocity, bool bInRequireGoalAdjustment);

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	void TickLockTimer(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
    bool HasLockedPreferredVelocity() const;
	
    UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	void LockPreferredVelocity(const FVector& InPreferredVelocity, float LockDuration);
	
    UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	void UnlockPreferredVelocity();
	
    UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	bool IsGoalAdjustmentRequired() const;

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
    bool HasLockedGoalAdjustment() const;
    
	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
    void LockGoalAdjustment();

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
    void UnlockGoalAdjustment();

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
    bool HasMovementComponent() const;

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
    bool HasUpdatedComponent() const;

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
    FVector GetPreferredVelocity() const;

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
    FVector GetPreferredDirection() const;

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
    FVector GetAvoidanceVelocity() const;

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
    FVector GetAgentVelocity() const;

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
    FVector GetAgentLocation() const;

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
    float GetAgentMaxSpeed() const;

    UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	int32 GetMaxNeighbourCount() const
    {
        return MaxNeighbours;
    }

    UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	float GetNeighbourDistance() const
    {
        return NeighbourDistance;
    }

    UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	float GetTimeHorizon() const
    {
        return TimeHorizon;
    }

    UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	float GetAgentRadius() const
    {
        return AgentRadius;
    }

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	int32 GetAvoidanceGroupMask() const {
        return AvoidanceGroup.Packed;
    }

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	int32 GetGroupsToAvoidMask() const {
        return GroupsToAvoid.Packed;
    }

	UFUNCTION(BlueprintCallable, Category="RVO3D|Agent")
	int32 GetGroupsToIgnoreMask() const {
        return GroupsToIgnore.Packed;
    }
};
