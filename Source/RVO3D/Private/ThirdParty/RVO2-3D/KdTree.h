
#ifndef RVO_KD_TREE_H_
#define RVO_KD_TREE_H_

#include <cstddef>
#include <vector>

#include "Vector3.h"

namespace RVO {
	class Agent;
	class RVOSimulator;

	/**
	 * \brief   Defines <i>k</i>d-trees for agents in the simulation.
	 */
	class KdTree {
	private:
		/**
		 * \brief   Defines an agent <i>k</i>d-tree node.
		 */
		class AgentTreeNode {
		public:
			/**
			 * \brief   The beginning node number.
			 */
			size_t begin;

			/**
			 * \brief   The ending node number.
			 */
			size_t end;

			/**
			 * \brief   The left node number.
			 */
			size_t left;

			/**
			 * \brief   The right node number.
			 */
			size_t right;

			/**
			 * \brief   The maximum coordinates.
			 */
			Vector3 maxCoord;

			/**
			 * \brief   The minimum coordinates.
			 */
			Vector3 minCoord;
		};

		/**
		 * \brief   Constructs a <i>k</i>d-tree instance.
		 * \param   sim  The simulator instance.
		 */
		explicit KdTree(RVOSimulator *sim);

		/**
		 * \brief   Builds an agent <i>k</i>d-tree.
		 */
		void buildAgentTree();

		void buildAgentTreeRecursive(size_t begin, size_t end, size_t node);

		/**
		 * \brief   Computes the agent neighbors of the specified agent.
		 * \param   agent    A pointer to the agent for which agent neighbors are to be computed.
		 * \param   rangeSq  The squared range around the agent.
		 */
		void computeAgentNeighbors(Agent *agent, float rangeSq) const;

		void queryAgentTreeRecursive(Agent *agent, float &rangeSq, size_t node) const;

		std::vector<Agent *> agents_;
		std::vector<AgentTreeNode> agentTree_;
		RVOSimulator *sim_;

		friend class Agent;
		friend class RVOSimulator;
	};
}

#endif /* RVO_KD_TREE_H_ */
