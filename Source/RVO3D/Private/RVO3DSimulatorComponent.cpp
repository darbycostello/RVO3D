
#include "RVO3DSimulatorComponent.h"
#include "RVO3DAgentComponent.h"
#include "RVO.h"

URVO3DSimulatorComponent::URVO3DSimulatorComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
	bTickInEditor = false;
	bAutoActivate = true;

    LockTimeAfterAvoid = .2f;
    LockTimeAfterGoalAdjustment = .066f;
    GoalAdjustmentAngleOffset = .125f;
}

void URVO3DSimulatorComponent::BeginPlay()
{
    Super::BeginPlay();

    if (! HasSimulator())
    {
        Simulator = MakeShareable( new RVO::RVOSimulator() );
    }
}

void URVO3DSimulatorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Clear simulator
    if (HasSimulator())
    {
        Simulator.Reset();
    }

    // Clear Agent entries
    for (auto& Elem : AgentMap)
    {
        Elem.Key->ResetSimulatorComponent();
    }
    AgentMap.Empty();

    Super::EndPlay(EndPlayReason);
}

void URVO3DSimulatorComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    check(HasSimulator());

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Prepares agent RVO properties
    for (auto& Elem : AgentMap)
    {
        URVO3DAgentComponent* AgentComponent = Elem.Key;
        const int32 AgentID = Elem.Value;

        if (AgentComponent->HasUpdatedComponent())
        {
            const FVector Position(AgentComponent->GetAgentLocation());
            const FVector Velocity(AgentComponent->GetAgentVelocity());
            const FVector Preferred(AgentComponent->GetPreferredVelocity());

            RVO::Vector3 RVO_Pos(Position.X, Position.Y, Position.Z);
            RVO::Vector3 RVO_Vel(Velocity.X, Velocity.Y, Velocity.Z);
            RVO::Vector3 RVO_Prf(Preferred.X, Preferred.Y, Preferred.Z);

            // Set agent position and preferred velocity
            // for current RVO simulation step
            Simulator->setAgentPosition(AgentID, RVO_Pos);
            Simulator->setAgentVelocity(AgentID, RVO_Vel);
            Simulator->setAgentPrefVelocity(AgentID, RVO_Prf);
        }
    }

    // Perform RVO simulation step
    Simulator->setTimeStep(DeltaTime);
    Simulator->doStep();

    // Applies agent computed RVO results
    for (auto& Elem : AgentMap)
    {
        URVO3DAgentComponent* AgentComponent = Elem.Key;
        const int32 AgentID = Elem.Value;

        if (AgentComponent->HasUpdatedComponent())
        {
            RVO::Vector3 RVO_Vel(Simulator->getAgentVelocity(AgentID));
            FVector Vel(RVO_Vel.x(), RVO_Vel.y(), RVO_Vel.z());
            AgentComponent->SetAvoidanceVelocity(Vel, !Simulator->isAgentValid(AgentID));
            AgentComponent->TickLockTimer(DeltaTime);
        }
    }
}

void URVO3DSimulatorComponent::AddAgentActor(AActor* AgentActor)
{
    if (AgentActor)
    {
        URVO3DAgentComponent* AgentComponent = AgentActor->FindComponentByClass<URVO3DAgentComponent>();

        if (AgentComponent)
        {
            AddAgentComponent(AgentComponent);
        }
    }
}

void URVO3DSimulatorComponent::RemoveAgentActor(AActor* AgentActor)
{
    if (AgentActor)
    {
        URVO3DAgentComponent* AgentComponent = AgentActor->FindComponentByClass<URVO3DAgentComponent>();

        if (AgentComponent)
        {
            RemoveAgentComponent(AgentComponent);
        }
    }
}

void URVO3DSimulatorComponent::AddAgentComponent(URVO3DAgentComponent* AgentComponent)
{
    if (! IsValid(AgentComponent) || AgentComponent->IsPendingKill())
    {
        return;
    }

    if (HasSimulator() && ! AgentMap.Contains(AgentComponent))
    {
        const FVector pos(AgentComponent->GetAgentLocation());
        int32 AgentID = Simulator->addAgent(
            RVO::Vector3(pos.X, pos.Y, pos.Z),
            AgentComponent->GetNeighbourDistance(),
            AgentComponent->GetMaxNeighbourCount(),
            AgentComponent->GetTimeHorizon(),
            AgentComponent->GetAgentRadius(),
            AgentComponent->GetAgentMaxSpeed(),
            AgentComponent->GetAvoidanceGroupMask(),
            AgentComponent->GetGroupsToAvoidMask(),
            AgentComponent->GetGroupsToIgnoreMask()
        );

        check(AgentID != RVO::RVO_ERROR);

        //UE_LOG(LogTemp,Warning, TEXT("Agent %d: Max Neighbour Count %d, NeightbourDist %f, Time Horizon %f, Radius %f, Max Speed %f, Group Mask: %d, Groups To Avoid Mask: %d, Groups To Ignore Mask: %d"),
        //    AgentID,
        //    AgentComponent->GetMaxNeighbourCount(),
        //    AgentComponent->GetNeighbourDistance(),
        //    AgentComponent->GetTimeHorizon(),
        //    AgentComponent->GetAgentRadius(),
        //    AgentComponent->GetAgentMaxSpeed(),
        //    AgentComponent->GetAvoidanceGroupMask(),
        //    AgentComponent->GetGroupsToAvoidMask(),
        //    AgentComponent->GetGroupsToIgnoreMask()
        //);

        AgentMap.Emplace(AgentComponent, AgentID);
        AgentComponent->SetSimulatorComponent(this);
    }
}

void URVO3DSimulatorComponent::RemoveAgentComponent(URVO3DAgentComponent* AgentComponent)
{
    if (HasSimulator() && AgentComponent && AgentMap.Contains(AgentComponent))
    {
        const int32 AgentID = AgentMap.FindAndRemoveChecked(AgentComponent);
        Simulator->removeAgent(AgentID);
        AgentComponent->ResetSimulatorComponent();
    }
}

void URVO3DSimulatorComponent::UpdateIgnoredAgents(const URVO3DAgentComponent* AgentComponent)
{
    if (! HasSimulator() || ! IsValid(AgentComponent))
    {
        return;
    }

    if (AgentMap.Contains(AgentComponent))
    {
        const int32 AgentID = AgentMap.FindChecked(AgentComponent);
        TSet<int> IgnoredAgentIDs;

        const TSet<const URVO3DAgentComponent*>& IgnoredAgents(AgentComponent->IgnoredAgents);

        for (const URVO3DAgentComponent* IgnoredAgent : IgnoredAgents)
        {
            if (AgentMap.Contains(IgnoredAgent))
            {
                IgnoredAgentIDs.Emplace(AgentMap.FindChecked(IgnoredAgent));
            }
        }

        Simulator->setAgentIgnoredNeighbors(AgentID, IgnoredAgentIDs);
    }
}

void URVO3DSimulatorComponent::AddIgnoredAgent(const URVO3DAgentComponent* AgentComponent, const URVO3DAgentComponent* IgnoredAgent)
{
    if (! HasSimulator() || ! IsValid(AgentComponent) || ! IsValid(IgnoredAgent))
    {
        return;
    }

    if (AgentMap.Contains(AgentComponent) && AgentMap.Contains(IgnoredAgent))
    {
        const int32 AgentID = AgentMap.FindChecked(AgentComponent);
        const int32 IgnoredID = AgentMap.FindChecked(IgnoredAgent);
        Simulator->addAgentNeighborToIgnore(AgentID, IgnoredID);
    }
}

void URVO3DSimulatorComponent::RemoveIgnoredAgent(const URVO3DAgentComponent* AgentComponent, const URVO3DAgentComponent* IgnoredAgent)
{
    if (! HasSimulator() || ! IsValid(AgentComponent) || ! IsValid(IgnoredAgent))
    {
        return;
    }

    if (AgentMap.Contains(AgentComponent) && AgentMap.Contains(IgnoredAgent))
    {
        const int32 AgentID = AgentMap.FindChecked(AgentComponent);
        const int32 IgnoredID = AgentMap.FindChecked(IgnoredAgent);
        Simulator->removeAgentNeighborToIgnore(AgentID, IgnoredID);
    }
}

void URVO3DSimulatorComponent::ClearIgnoredAgents(const URVO3DAgentComponent* AgentComponent, bool bAllowShrinking)
{
    if (! HasSimulator() || ! IsValid(AgentComponent))
    {
        return;
    }

    if (AgentMap.Contains(AgentComponent))
    {
        const int32 AgentID = AgentMap.FindChecked(AgentComponent);
        Simulator->clearAgentIgnoredNeighbors(AgentID, bAllowShrinking);
    }
}
