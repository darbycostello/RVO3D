
#include "RVOSimulator.h"
#include "Agent.h"
#include "KdTree.h"

namespace RVO {
	RVOSimulator::RVOSimulator() : defaultAgent_(NULL), kdTree_(NULL), globalTime_(0.0f), timeStep_(0.0f)
	{
		kdTree_ = new KdTree(this);
	}

	RVOSimulator::RVOSimulator(float timeStep, float neighborDist, size_t maxNeighbors, float timeHorizon, float radius, float maxSpeed, const Vector3 &velocity) : defaultAgent_(NULL), kdTree_(NULL), globalTime_(0.0f), timeStep_(timeStep)
	{
		kdTree_ = new KdTree(this);
		defaultAgent_ = new Agent(this);

		defaultAgent_->maxNeighbors_ = maxNeighbors;
		defaultAgent_->maxSpeed_ = maxSpeed;
		defaultAgent_->neighborDist_ = neighborDist;
		defaultAgent_->radius_ = radius;
		defaultAgent_->timeHorizon_ = timeHorizon;
		defaultAgent_->velocity_ = velocity;
	}

	RVOSimulator::~RVOSimulator()
	{
        agentMap_.Empty();

		if (defaultAgent_ != NULL) {
			delete defaultAgent_;
		}

		for (size_t i = 0; i < agents_.size(); ++i) {
			delete agents_[i];
		}

		if (kdTree_ != NULL) {
			delete kdTree_;
		}
	}

	bool RVOSimulator::hasAgent(size_t agentNo) const
	{
		return agentMap_.Contains(agentNo);
	}

	size_t RVOSimulator::getAgentNumNeighbors(size_t agentNo) const
	{
		return agentMap_.FindChecked(agentNo)->agentNeighbors_.size();
	}

	size_t RVOSimulator::getAgentNeighbour(size_t agentNo, size_t neighborNo) const
	{
		return agentMap_.FindChecked(agentNo)->agentNeighbors_[neighborNo].second->id_;
	}

	size_t RVOSimulator::getAgentNumORCAPlanes(size_t agentNo) const
	{
		return agentMap_.FindChecked(agentNo)->orcaPlanes_.size();
	}

	const Plane &RVOSimulator::getAgentORCAPlane(size_t agentNo, size_t planeNo) const
	{
		return agentMap_.FindChecked(agentNo)->orcaPlanes_[planeNo];
	}

	void RVOSimulator::removeAgent(size_t agentNo)
	{
        // Delete agent
		delete agents_[agentNo];
        // RemoveAtSwap()
		agents_[agentNo] = agents_.back();
		agents_.pop_back();
        // Remove agentMap_ entry
        agentMap_.Remove(agentNo);
	}

	size_t RVOSimulator::addAgent(const Vector3 &position)
	{
		if (defaultAgent_ == NULL) {
			return RVO_ERROR;
		}

		Agent *agent = new Agent(this);
        size_t agentID = agentUID_++;

		agent->position_ = position;
		agent->maxNeighbors_ = defaultAgent_->maxNeighbors_;
		agent->maxSpeed_ = defaultAgent_->maxSpeed_;
		agent->neighborDist_ = defaultAgent_->neighborDist_;
		agent->radius_ = defaultAgent_->radius_;
		agent->timeHorizon_ = defaultAgent_->timeHorizon_;
		agent->avoidanceGroup_ = defaultAgent_->avoidanceGroup_;
		agent->groupsToAvoid_ = defaultAgent_->groupsToAvoid_;
		agent->groupsToIgnore_ = defaultAgent_->groupsToIgnore_;
		agent->velocity_ = defaultAgent_->velocity_;

		agent->id_ = agentID;

		agents_.push_back(agent);
		agentMap_.Emplace(agentID, agent);

		return agentID;
	}

	size_t RVOSimulator::addAgent(const Vector3 &position, float neighborDist, size_t maxNeighbors, float timeHorizon, float radius, float maxSpeed, int avoidanceGroup, int groupsToAvoid, int groupsToIgnore, const Vector3 &velocity)
	{
		Agent *agent = new Agent(this);
        size_t agentID = agentUID_++;

		agent->position_ = position;
		agent->maxNeighbors_ = maxNeighbors;
		agent->maxSpeed_ = maxSpeed;
		agent->neighborDist_ = neighborDist;
		agent->radius_ = radius;
		agent->timeHorizon_ = timeHorizon;
		agent->avoidanceGroup_ = avoidanceGroup;
		agent->groupsToAvoid_ = groupsToAvoid;
		agent->groupsToIgnore_ = groupsToIgnore;
		agent->velocity_ = velocity;

		agent->id_ = agentID;

		agents_.push_back(agent);
		agentMap_.Emplace(agentID, agent);

		return agentID;
	}

	void RVOSimulator::doStep()
	{
		kdTree_->buildAgentTree();

		for (int i = 0; i < static_cast<int>(agents_.size()); ++i) {
			agents_[i]->computeNeighbors();
			agents_[i]->computeNewVelocity();
		}

		for (int i = 0; i < static_cast<int>(agents_.size()); ++i) {
			agents_[i]->update();
		}

		globalTime_ += timeStep_;
	}

	size_t RVOSimulator::getAgentMaxNeighbors(size_t agentNo) const
	{
		return agentMap_.FindChecked(agentNo)->maxNeighbors_;
	}

	float RVOSimulator::getAgentMaxSpeed(size_t agentNo) const
	{
		return agentMap_.FindChecked(agentNo)->maxSpeed_;
	}

	float RVOSimulator::getAgentNeighborDist(size_t agentNo) const
	{
		return agentMap_.FindChecked(agentNo)->neighborDist_;
	}

	const Vector3 &RVOSimulator::getAgentPosition(size_t agentNo) const
	{
		return agentMap_.FindChecked(agentNo)->position_;
	}

	const Vector3 &RVOSimulator::getAgentPrefVelocity(size_t agentNo) const
	{
		return agentMap_.FindChecked(agentNo)->prefVelocity_;
	}

	float RVOSimulator::getAgentRadius(size_t agentNo) const
	{
		return agentMap_.FindChecked(agentNo)->radius_;
	}

	float RVOSimulator::getAgentTimeHorizon(size_t agentNo) const
	{
		return agentMap_.FindChecked(agentNo)->timeHorizon_;
	}

	const Vector3 &RVOSimulator::getAgentVelocity(size_t agentNo) const
	{
		return agentMap_.FindChecked(agentNo)->velocity_;
	}

	bool RVOSimulator::isAgentValid(size_t agentNo) const
	{
		return agentMap_.FindChecked(agentNo)->valid_;
	}

	float RVOSimulator::getGlobalTime() const
	{
		return globalTime_;
	}

	size_t RVOSimulator::getNumAgents() const
	{
		return agents_.size();
	}

	float RVOSimulator::getTimeStep() const
	{
		return timeStep_;
	}

	void RVOSimulator::setAgentDefaults(float neighborDist, size_t maxNeighbors, float timeHorizon, float radius, float maxSpeed, int avoidanceGroup, int groupsToAvoid, int groupsToIgnore, const Vector3 &velocity)
	{
		if (defaultAgent_ == NULL) {
			defaultAgent_ = new Agent(this);
		}

		defaultAgent_->maxNeighbors_ = maxNeighbors;
		defaultAgent_->maxSpeed_ = maxSpeed;
		defaultAgent_->neighborDist_ = neighborDist;
		defaultAgent_->radius_ = radius;
		defaultAgent_->timeHorizon_ = timeHorizon;
		defaultAgent_->avoidanceGroup_ = avoidanceGroup;
		defaultAgent_->groupsToAvoid_ = groupsToAvoid;
		defaultAgent_->groupsToIgnore_ = groupsToIgnore;
		defaultAgent_->velocity_ = velocity;
	}

	void RVOSimulator::setAgentMaxNeighbors(size_t agentNo, size_t maxNeighbors)
	{
		agentMap_.FindChecked(agentNo)->maxNeighbors_ = maxNeighbors;
	}

	void RVOSimulator::setAgentMaxSpeed(size_t agentNo, float maxSpeed)
	{
		agentMap_.FindChecked(agentNo)->maxSpeed_ = maxSpeed;
	}

	void RVOSimulator::setAgentNeighborDist(size_t agentNo, float neighborDist)
	{
		agentMap_.FindChecked(agentNo)->neighborDist_ = neighborDist;
	}

	void RVOSimulator::setAgentPosition(size_t agentNo, const Vector3 &position)
	{
		agentMap_.FindChecked(agentNo)->position_ = position;
	}

	void RVOSimulator::setAgentPrefVelocity(size_t agentNo, const Vector3 &prefVelocity)
	{
		agentMap_.FindChecked(agentNo)->prefVelocity_ = prefVelocity;
	}

	void RVOSimulator::setAgentRadius(size_t agentNo, float radius)
	{
		agentMap_.FindChecked(agentNo)->radius_ = radius;
	}

	void RVOSimulator::setAgentTimeHorizon(size_t agentNo, float timeHorizon)
	{
		agentMap_.FindChecked(agentNo)->timeHorizon_ = timeHorizon;
	}

	void RVOSimulator::setAgentVelocity(size_t agentNo, const Vector3 &velocity)
	{
		agentMap_.FindChecked(agentNo)->velocity_ = velocity;
	}

    // Avoidance Group

	void RVOSimulator::setAgentAvoidanceGroup(size_t agentNo, int mask)
	{
		agentMap_.FindChecked(agentNo)->avoidanceGroup_ = mask;
	}

	void RVOSimulator::setAgentGroupsToAvoid(size_t agentNo, int mask)
	{
		agentMap_.FindChecked(agentNo)->groupsToAvoid_ = mask;
	}

	void RVOSimulator::setAgentGroupsToIgnore(size_t agentNo, int mask)
	{
		agentMap_.FindChecked(agentNo)->groupsToIgnore_ = mask;
	}

    // Agent Exclusions

	void RVOSimulator::addAgentNeighborToIgnore(size_t agentNo, size_t ignoredNo)
	{
		agentMap_.FindChecked(agentNo)->agentsToIgnore_.Emplace(ignoredNo);
	}

	void RVOSimulator::removeAgentNeighborToIgnore(size_t agentNo, size_t ignoredNo)
	{
		agentMap_.FindChecked(agentNo)->agentsToIgnore_.Remove(ignoredNo);
	}

	void RVOSimulator::setAgentIgnoredNeighbors(size_t agentNo, const TArray<int>& ignoredArr)
	{
		agentMap_.FindChecked(agentNo)->agentsToIgnore_.Append(ignoredArr);
	}

	void RVOSimulator::setAgentIgnoredNeighbors(size_t agentNo, const TSet<int>& ignoredSet)
	{
		agentMap_.FindChecked(agentNo)->agentsToIgnore_ = ignoredSet;
	}

	void RVOSimulator::clearAgentIgnoredNeighbors(size_t agentNo, bool bAllowShrinking)
	{
        Agent* Agent( agentMap_.FindChecked(agentNo) );

        Agent->agentsToIgnore_.Reset();

        if (bAllowShrinking)
        {
            Agent->agentsToIgnore_.Shrink();
        }
	}

	//void RVOSimulator::setAgentDebug(size_t agentNo, bool debug)
	//{
	//    agentMap_.FindChecked(agentNo)->debug_ = debug;
	//}

	void RVOSimulator::setTimeStep(float timeStep)
	{
		timeStep_ = timeStep;
	}
}
