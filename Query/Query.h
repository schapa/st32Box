/*
 * Query.h
 *
 *  Created on: Apr 19, 2016
 *      Author: shapa
 */

#ifndef QUERY_H_
#define QUERY_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
	STEP_INIT,
	STEP_WAITING,
	STEP_ENDED,
	STEP_FAILED,
	STEP_LAST
} StepState_t;

typedef enum {
	QUERY_INIT,
	QUERY_PROCESSING,
	QUERY_WAITING,
	QUERY_DONE,
	QUERY_FAILED,
	QUERY_LAST
} QueryState_t;

typedef struct RequestNode Request_t, *Request_p;

typedef struct {
	_Bool (*start) (Request_p req);
	_Bool (*success) (Request_p req);
	_Bool (*fail) (Request_p req);
	StepState_t state;
	uint32_t timeout;
} Step_t, *Step_p;

struct RequestNode {
	uint32_t handle;
	QueryState_t state;
	Step_p steps;
	size_t stepsCount;
	size_t stepCurrent;
	const char *buffer;
	const size_t bufferSize;
	size_t bufferOccupied;
};

void QueryProcess(Request_p req);

#endif /* QUERY_H_ */
