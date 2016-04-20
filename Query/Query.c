/*
 * Query.c
 *
 *  Created on: Apr 19, 2016
 *      Author: shapa
 */

#include "Query.h"

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

static _Bool makeStep(Request_p req);

void QueryProcess(Request_p req) {
	do {
		if (!req || !req->steps || !req->buffer)
			break;
		if (req->stepCurrent >= req->stepsCount) {
			req->state = QUERY_DONE;
			break;
		}
		while (makeStep(req) && req->state == QUERY_PROCESSING);
		switch (req->state) {
			case QUERY_WAITING:
				trace_printf("[Waiting on step: %u]\n\r", req->stepCurrent);
				break;
			case QUERY_DONE:
				trace_printf("[Query done]\n\r");
				break;
			case QUERY_FAILED:
				trace_printf("[Query failed! on step %u]\n\r", req->stepCurrent);
				break;
			default:
				break;
		}
	} while (0);
}

static _Bool makeStep(Request_p req) {
	_Bool continueStepping = false;
	Step_p step = NULL;
	do {
		if (!req || !req->steps || !req->buffer)
			break;
		if (req->stepCurrent >= req->stepsCount) {
			req->state = QUERY_DONE;
			break;
		}
		step = &(req->steps[req->stepCurrent]);
		continueStepping = true;
		req->state = QUERY_PROCESSING;
		switch (step->state) {
			case STEP_INIT:
				if (step->start) {
					continueStepping = step->start(req);
					req->state = QUERY_WAITING;
					trace_printf("[Starting: %u] <%s>\n\r", req->stepCurrent, req->buffer);
				}
				break;
			case STEP_WAITING:
				if (step->success) {
					trace_printf("[Successed: %u] <%s>\n\r", req->stepCurrent, req->buffer);
					continueStepping = step->success(req);
				}
				break;
			case STEP_FAILED:
				if (step->fail && step->fail(req)) {
					step->state = STEP_WAITING;
				} else {
					continueStepping = false;
					req->state = QUERY_FAILED;
				}
				break;
			default:
				break;
		}
		if (continueStepping && ++step->state == STEP_ENDED) {
			req->bufferOccupied = 0;
			req->stepCurrent++;
		}
	} while (0);

	return continueStepping;
}


