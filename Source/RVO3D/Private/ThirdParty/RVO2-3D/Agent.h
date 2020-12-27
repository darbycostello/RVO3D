#ifndef RVO_AGENT_H_
#define RVO_AGENT_H_

#include <cstddef>
#include <utility>
#include <vector>

#include "RVOSimulator.h"
#include "Vector3.h"
#include "Containers/Set.h"

namespace RVO {
	/**
	 * \brief   Defines an agent in the simulation.
	 */
	class Agent {
	private:
		/**
		 * \brief   Constructs an agent instance.
		 * \param   sim  The simulator instance.
		 */
		explicit Agent(RVOSimulator *sim);

		/**
		 * \brief   Computes the neighbors of this agent.
		 */
		void computeNeighbors();

		/**
		 * \brief   Computes the new velocity of this agent.
		 */
		void computeNewVelocity();

		/**
		 * \brief   Inserts an agent neighbor into the set of neighbors of this agent.
		 * \param   agent    A pointer to the agent to be inserted.
		 * \param   rangeSq  The squared range around this agent.
		 */
		void insertAgentNeighbor(const Agent *agent, float &rangeSq);

		/**
		 * \brief   Checks whether a group mask should be considered on agent velocity calculation.
		 * \param   otherGroupMask  Other group mask.
		 * \return  Group ignore flag.
		 */
        bool shouldIgnoreGroup(int otherGroupMask) const;

		/**
		 * \brief   Updates the three-dimensional position and three-dimensional velocity of this agent.
		 */
		void update();

		Vector3 newVelocity_;
		Vector3 position_;
		Vector3 prefVelocity_;
		Vector3 velocity_;
		RVOSimulator *sim_;
		size_t id_;
		size_t maxNeighbors_;
		float maxSpeed_;
		float neighborDist_;
		float radius_;
		float timeHorizon_;
		std::vector<std::pair<float, const Agent *> > agentNeighbors_;
		std::vector<Plane> orcaPlanes_;

        //bool debug_;
        bool valid_;

        int avoidanceGroup_;
        int groupsToAvoid_;
        int groupsToIgnore_;
        TSet<int> agentsToIgnore_;

		friend class KdTree;
		friend class RVOSimulator;
	};
}

#endif /* RVO_AGENT_H_ */
