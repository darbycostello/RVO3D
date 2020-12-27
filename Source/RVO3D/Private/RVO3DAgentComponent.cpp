
#include "RVO3DAgentComponent.h"
#include "RVO3DSimulatorComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/MovementComponent.h"

URVO3DAgentComponent::URVO3DAgentComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

    bAutoRegisterMovementComponent = true;

    MaxNeighbours = 10;
    NeighbourDistance = 15.f;
    TimeHorizon = 10.f;
    AgentRadius = 1.5f;

	AvoidanceGroup.bGroup0 = true;
	GroupsToAvoid.Packed = 0xFFFFFFFF;
	GroupsToIgnore.Packed = 0;

    AvoidanceVelocity = FVector::ZeroVector;
    PreferredVelocity = FVector::ZeroVector;
    PreferredVelocityLockTimer = -1.f;
    GoalAdjustment = FVector::ZeroVector;
    GoalAdjustmentLockTimer = -1.f;
    bRequireGoalAdjustment = false;
}

//BEGIN UActorComponent Interface

void URVO3DAgentComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (! MovementComponent && bAutoRegisterMovementComponent)
	{
		// Auto-register owner's root component if found.
		if (AActor* MyActor = GetOwner())
		{
            UMovementComponent* NewMovementComponent = MyActor->FindComponentByClass<UMovementComponent>();

			if (NewMovementComponent)
			{
				SetMovementComponent(NewMovementComponent);
			}
		}
	}
}

void URVO3DAgentComponent::OnRegister()
{
	Super::OnRegister();

	if (! MovementComponent && bAutoRegisterMovementComponent)
	{
		// Auto-register owner's root component if found.
		if (AActor* MyActor = GetOwner())
		{
            UMovementComponent* NewMovementComponent = MyActor->FindComponentByClass<UMovementComponent>();

			if (NewMovementComponent)
			{
				SetMovementComponent(NewMovementComponent);
			}
		}
	}
}

void URVO3DAgentComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    ResetSimulatorComponent();
}

//END UActorComponent interface.	

void URVO3DAgentComponent::SetMovementComponent(UMovementComponent* InMovementComponent)
{
    // Remove tick prerequisite from existing movement component
	if (MovementComponent && MovementComponent != InMovementComponent)
	{
        UnregisterSimulatorDependency();
	}

	MovementComponent = IsValid(InMovementComponent) && !InMovementComponent->IsPendingKill() ? InMovementComponent : NULL;

    // Add tick prerequisite to new movement component
	if (MovementComponent)
	{
        RegisterSimulatorDependency();
	}
}

bool URVO3DAgentComponent::HasSimulator() const
{
    return SimulatorComponent != nullptr;
}

void URVO3DAgentComponent::SetSimulatorActor(AActor* InSimulatorActor)
{
    URVO3DSimulatorComponent* NewSimulatorComponent = nullptr;

    if (InSimulatorActor)
    {
        NewSimulatorComponent = InSimulatorActor->FindComponentByClass<URVO3DSimulatorComponent>();
    }

    SetSimulatorComponent(NewSimulatorComponent);
}

void URVO3DAgentComponent::SetSimulatorComponent(URVO3DSimulatorComponent* InSimulatorComponent)
{
    // New component is the same as the old one, abort
    if (SimulatorComponent == InSimulatorComponent)
    {
        return;
    }

    if (SimulatorComponent)
    {
        ResetSimulatorComponent();
    }

    if (InSimulatorComponent)
    {
        SimulatorComponent = InSimulatorComponent;
        SimulatorComponent->AddAgentComponent(this);
        RegisterSimulatorDependency();
    }
}

void URVO3DAgentComponent::RegisterSimulatorDependency()
{
    if (SimulatorComponent && MovementComponent)
    {
        MovementComponent->AddTickPrerequisiteComponent(SimulatorComponent);
    }
}

void URVO3DAgentComponent::UnregisterSimulatorDependency()
{
    if (SimulatorComponent && MovementComponent)
    {
        MovementComponent->RemoveTickPrerequisiteComponent(SimulatorComponent);
    }
}

void URVO3DAgentComponent::ResetSimulatorComponent()
{
    if (SimulatorComponent)
    {
        UnregisterSimulatorDependency();

        URVO3DSimulatorComponent* OldSimulatorComponent = SimulatorComponent;
        SimulatorComponent = nullptr;

        OldSimulatorComponent->RemoveAgentComponent(this);
    }
}

// ~ Group Controls

void URVO3DAgentComponent::SetAvoidanceGroupMask(const FNavAvoidanceMask& GroupMask)
{
	AvoidanceGroup.SetFlagsDirectly(GroupMask.Packed);
}

void URVO3DAgentComponent::SetGroupsToAvoidMask(const FNavAvoidanceMask& GroupMask)
{
	GroupsToAvoid.SetFlagsDirectly(GroupMask.Packed);
}

void URVO3DAgentComponent::SetGroupsToIgnoreMask(const FNavAvoidanceMask& GroupMask)
{
	GroupsToIgnore.SetFlagsDirectly(GroupMask.Packed);
}

// ~ Agent Exclusions

void URVO3DAgentComponent::UpdateIgnoredAgents()
{
    if (HasSimulator())
    {
        SimulatorComponent->UpdateIgnoredAgents(this);
    }
}

void URVO3DAgentComponent::AddIgnoredAgent(const URVO3DAgentComponent* AgentComponent)
{
    if (IsValid(AgentComponent))
    {
        IgnoredAgents.Emplace(AgentComponent);

        if (HasSimulator())
        {
            SimulatorComponent->AddIgnoredAgent(this, AgentComponent);
        }
    }
}

void URVO3DAgentComponent::RemoveIgnoredAgent(const URVO3DAgentComponent* AgentComponent)
{
    IgnoredAgents.Remove(AgentComponent);

    if (HasSimulator())
    {
        SimulatorComponent->RemoveIgnoredAgent(this, AgentComponent);
    }
}

void URVO3DAgentComponent::ClearIgnoredAgents(bool bAllowShrinking)
{
    IgnoredAgents.Reset();

    if (bAllowShrinking)
    {
        IgnoredAgents.Shrink();
    }

    if (HasSimulator())
    {
        SimulatorComponent->ClearIgnoredAgents(this, bAllowShrinking);
    }
}

// ~ Lock Controls

void URVO3DAgentComponent::TickLockTimer(float DeltaTime)
{
    if (HasLockedPreferredVelocity())
    {
        PreferredVelocityLockTimer -= DeltaTime;
    }

    if (HasLockedGoalAdjustment())
    {
        GoalAdjustmentLockTimer -= DeltaTime;
    }
}

void URVO3DAgentComponent::SetAvoidanceVelocity(const FVector& Velocity, bool bInRequireGoalAdjustment)
{
    AvoidanceVelocity = Velocity;
    bRequireGoalAdjustment = bInRequireGoalAdjustment;

    // Lock a new goal adjustment if no current one exist
    if (bRequireGoalAdjustment && ! HasLockedGoalAdjustment())
    {
        LockGoalAdjustment();
    }
}

// ~ Preferred Velocity Functions

bool URVO3DAgentComponent::HasLockedPreferredVelocity() const
{
    return PreferredVelocityLockTimer > 0.f;
}

void URVO3DAgentComponent::LockPreferredVelocity(const FVector& InPreferredVelocity, float LockDuration = 0.0f)
{
    if (SimulatorComponent) {
        PreferredVelocity = InPreferredVelocity;
        if (LockDuration > 0.0f) {
            PreferredVelocityLockTimer = LockDuration;    
        }
    }
    
}

void URVO3DAgentComponent::UnlockPreferredVelocity()
{
    PreferredVelocityLockTimer = -1.f;
}

// ~ Goal Adjustment Functions

bool URVO3DAgentComponent::IsGoalAdjustmentRequired() const
{
    return bRequireGoalAdjustment;
}

bool URVO3DAgentComponent::HasLockedGoalAdjustment() const
{
    return GoalAdjustmentLockTimer > 0.f;
}

void URVO3DAgentComponent::LockGoalAdjustment()
{
    if (SimulatorComponent)
    {
        const FVector Direction = GetPreferredVelocity().GetSafeNormal();
        const float Angle = FMath::Max(SimulatorComponent->GoalAdjustmentAngleOffset, .001f);

        GoalAdjustment = FMath::VRandCone(Direction, PI*Angle);
        GoalAdjustmentLockTimer = SimulatorComponent->LockTimeAfterGoalAdjustment;
    }
}

void URVO3DAgentComponent::UnlockGoalAdjustment()
{
    GoalAdjustmentLockTimer = -1.f;
}

// ~ Velocity Accessors

bool URVO3DAgentComponent::HasMovementComponent() const
{
    return MovementComponent != nullptr;
}

bool URVO3DAgentComponent::HasUpdatedComponent() const
{
    return MovementComponent && MovementComponent->UpdatedComponent != nullptr;
}

FVector URVO3DAgentComponent::GetPreferredVelocity() const
{
    const FVector V( HasLockedPreferredVelocity() ? PreferredVelocity : GetAgentVelocity() );

    // Modify velocity direction if goal adjustment have been made
    if (HasLockedGoalAdjustment())
    {
        return (V.GetSafeNormal()+GoalAdjustment).GetUnsafeNormal() * V.Size();
    }

    return V;
}

FVector URVO3DAgentComponent::GetPreferredDirection() const
{
    return PreferredVelocity.GetSafeNormal();
}

FVector URVO3DAgentComponent::GetAvoidanceVelocity() const
{
    return AvoidanceVelocity;
}

FVector URVO3DAgentComponent::GetAgentVelocity() const
{
    return MovementComponent ? MovementComponent->Velocity : FVector::ZeroVector;
}

FVector URVO3DAgentComponent::GetAgentLocation() const
{
    return HasUpdatedComponent() ? MovementComponent->UpdatedComponent->GetComponentLocation() : FVector::ZeroVector;
}

float URVO3DAgentComponent::GetAgentMaxSpeed() const
{
    return MovementComponent ? MovementComponent->GetMaxSpeed() : 1.f;
}
