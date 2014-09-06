/**
 * @file pp.cpp
 * @date Jan 23, 2014
 *
 * @author Milos Subotic <milos.subotic.sm@gmail.com>
 * @license LGPLv3
 *
 * @brief Pool pointer.
 *
 * @version 1.0
 * Changelog:
 * 1.0 - Initial version.
 *
 */

///////////////////////////////////////////////////////////////////////////////

#include "pp.h"

#include <map>
#include <list>

#include <pthread.h>
#include <signal.h>

#include "thread.h"

#include "log.h"

///////////////////////////////////////////////////////////////////////////////

static mutex _gcMutex;
typedef std::list<void*> SameThreadPools;
typedef std::map<pthread_t, SameThreadPools> ThreadToPoolsMap;
static ThreadToPoolsMap _threadToPoolMap;

void _pp_registerNewPool(void* pointerToPool) {
	unique_lock<mutex> l(_gcMutex);

	pthread_t id = pthread_self();

	if_in_map(id, _threadToPoolMap, iter){
		SameThreadPools& pools = iter->second;
		pools.push_back(pointerToPool);
	}else{
		SameThreadPools pools;
		pools.push_back(pointerToPool);
		_threadToPoolMap[id] = pools;
	}

}

void pp_gc() {
	typedef pp<int>::_Node* NodeP;
	typedef pp<int>::_Node** Pool;

	unique_lock<mutex> l(_gcMutex);

	std::list<pthread_t> forErase;

	for(auto kv: _threadToPoolMap){
		pthread_t id = kv.first;
		SameThreadPools& pools = kv.second;

		// Check if thread is dead.
		int ret = pthread_kill(id, 0);
		if(ret == ESRCH){
			// It is dead.

			for(void* p: pools){
				Pool pool = reinterpret_cast<Pool>(p);

				// Clean up pool.
				NodeP node = *pool;
				while(node){
					NodeP t = node;
					node = t->pool.next;
					delete t->pool.p;
					delete t;
				}
				*pool = nullptr;
			}

		}
	}

	for(pthread_t id: forErase){
		_threadToPoolMap.erase(id);
	}
}

///////////////////////////////////////////////////////////////////////////////
