/*
 * Query.c
 *
 *  Created on: Apr 19, 2016
 *      Author: shapa
 */

#include "Query.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "misc.h"

static _Bool makeStep(Request_p req);
static _Bool isStepAck(Request_p req, _Bool *isOk);

void QueryProcess(Request_p req) {
	do {
		if (!req || !req->steps || !req->tx.buff)
			break;
		if (req->stepCurrent >= req->stepsCount) {
			req->state = QUERY_DONE;
			break;
		}
		while (makeStep(req) && req->state == QUERY_PROCESSING);
		switch (req->state) {
			case QUERY_WAITING:
				trace_printf("[Query] Waiting on step: %u\n\r", req->stepCurrent);
				break;
			case QUERY_DONE:
				trace_printf("[Query] done\n\r");
				break;
			case QUERY_FAILED:
				trace_printf("[Query] failed! on step %u]\n\r", req->stepCurrent);
				break;
			default:
				break;
		}
	} while (0);
}

static _Bool makeStep(Request_p req) {
	_Bool continueStepping = false;
	Step_p step = NULL;
	_Bool isOk = false;
	do {
		if (!req || !req->steps || !req->tx.buff)
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
					trace_printf("[Query] Starting: %u %s", req->stepCurrent, req->tx.buff);
				}
				break;
			case STEP_WAITING:
				req->tx.occupied = 0;
				if (isStepAck(req, &isOk)) {
					continueStepping = true;
					if (step->success) {
						trace_printf("[Query] Successed: %u <%s>\n\r", req->stepCurrent, req->tx.buff);
						if (!step->success(req))
							req->state = QUERY_FAILED;
					}
					trace_printf("[Query] Free RX BUFF \n\r");
					MEMMAN_free(req->rx.buff);
				} else
					continueStepping = false;
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
			req->rx.occupied = 0;
			req->tx.occupied = 0;
			req->stepCurrent++;
		}
	} while (0);

	return continueStepping;
}

static _Bool isStepAck(Request_p req, _Bool *isOk) {
	_Bool result = false;
	static const char *const ackError = "ERROR";
	const size_t ackErrorSize = strlen(ackError);
	do {
		if (!req || !isOk || !req->rx.occupied)
			break;
		const char *const ackOk = req->steps[req->stepCurrent].acknowledge ? req->steps[req->stepCurrent].acknowledge : "OK";
		const size_t ackOkSize = strlen(ackOk);
		*isOk = false;
		if (req->rx.occupied >= ackErrorSize) {
			char *ptr = &req->rx.buff[req->rx.occupied - ackErrorSize];
			if (!strcmp(ptr, ackError)) {
				result = true;
				break;
			}
		}
		if (req->rx.occupied >= ackOkSize) {
			char *ptr = &req->rx.buff[req->rx.occupied - ackOkSize];
			if (!strcmp(ptr, ackOk)) {
				*isOk = true;
				result = true;
				break;
			}
		}

	} while (0);
	return result;
}

