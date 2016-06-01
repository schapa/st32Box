/*****************************************************************************
* Model: query.qm
* File:  ./Engine.c
*
* This code has been generated by QM tool (see state-machine.com/qm).
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*****************************************************************************/
/*${System::.::Engine.c} ...................................................*/
#include "qp_port.h"
#include "Engine.h"
#include "QueryEngine.h"
#include <stdint.h>
#include <string.h>
#include "requests.h"
#include "bsp.h"
#include "dbg_base.h"
#if 01
#include "dbg_trace.h"
#endif

static void HandleSysEvt(SystemEvent *evt);

static Engine s_engine;
static QActive * s_pEngine = NULL;

QActive * AO_system(void) {
    if (!s_pEngine) {
        Engine_ctor(&s_engine);
        s_pEngine = (QActive*)&s_engine;
    }
    return s_pEngine;
}

/*${System::Engine} ........................................................*/
/*${System::Engine::ctor} ..................................................*/
void Engine_ctor(Engine * const me) {
    DBGMSG_M("Create");
    QActive_ctor(&me->super, Q_STATE_CAST(&Engine_initial));
    QTimeEvt_ctorX(&me->timeEvt, &me->super, TIMEOUT_SIG, 0U);
}
/*${System::Engine::SM} ....................................................*/
QState Engine_initial(Engine * const me, QEvt const * const e) {
    /* ${System::Engine::SM::initial} */
    (void)e;
    //QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_SECOND/2U, BSP_TICKS_PER_SECOND/2U);
    return Q_TRAN(&Engine_Init);
}
/*${System::Engine::SM::Working} ...........................................*/
QState Engine_Working(Engine * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${System::Engine::SM::Working} */
        case Q_ENTRY_SIG: {
            DBGMSG_M("Q_ENTRY");
            status_ = Q_HANDLED();
            break;
        }
        /* ${System::Engine::SM::Working} */
        case Q_EXIT_SIG: {
            DBGMSG_M("Q_EXIT");
            status_ = Q_HANDLED();
            break;
        }
        /* ${System::Engine::SM::Working::SYSTEM} */
        case SYSTEM_SIG: {
            HandleSysEvt((SystemEvent*)e);
            status_ = Q_HANDLED();
            break;
        }
        /* ${System::Engine::SM::Working::TIMEOUT} */
        case TIMEOUT_SIG: {
            status_ = Q_TRAN(&Engine_Init);
            break;
        }
        /* ${System::Engine::SM::Working::QUERY_FAILED} */
        case QUERY_FAILED_SIG: {
            DBGMSG_ERR("QUERY_FAILED");
            status_ = Q_TRAN(&Engine_Working);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*${System::Engine::SM::Working::Init} .....................................*/
QState Engine_Init(Engine * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${System::Engine::SM::Working::Init} */
        case Q_ENTRY_SIG: {
            DBGMSG_M("Entry");
            RequestEvent *evt = Q_NEW(RequestEvent, NEW_REQUEST_SIG);
            evt->request = Request_GetInitial();
            QACTIVE_POST(AO_QueryEngine(), evt, NULL);
            status_ = Q_HANDLED();
            break;
        }
        /* ${System::Engine::SM::Working::Init} */
        case Q_EXIT_SIG: {
            DBGMSG_M("Exit");
            status_ = Q_HANDLED();
            break;
        }
        /* ${System::Engine::SM::Working::Init::QUERY_DONE} */
        case QUERY_DONE_SIG: {
            status_ = Q_TRAN(&Engine_Connect);
            break;
        }
        default: {
            status_ = Q_SUPER(&Engine_Working);
            break;
        }
    }
    return status_;
}
/*${System::Engine::SM::Working::Connect} ..................................*/
QState Engine_Connect(Engine * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${System::Engine::SM::Working::Connect} */
        case Q_ENTRY_SIG: {
            DBGMSG_M("Entry");
            systemConfig_p config = SystemConfig_get();
            nonVolatileSettings_p settings = SystemConfig_get()->settings;
            if (config->discoveredApnsSize && settings->knownApnsCount) {
                size_t i, j;
                size_t matchCount = 0;
                size_t matchIdx = -1;
                DBGMSG_M("Credentials/Found %d/%d", settings->knownApnsCount, config->discoveredApnsSize);
                for (i = 0; i < settings->knownApnsCount; i++) {
                    for (j = 0; j < config->discoveredApnsSize; j++) {
                        DBGMSG_H("[%s] == [%s]", settings->knownApns[i].ssid, config->discoveredApns[j].ssid);
                        if (!strcmp(settings->knownApns[i].ssid, config->discoveredApns[j].ssid)) {
                            DBGMSG_H(" Match %d [%s]", i, settings->knownApns[i].ssid);
                            matchIdx = i;
                            matchCount++;
                        }
                    }
                }
                if (matchCount) {
                    if (matchCount == 1) {
                       RequestEvent *evt = Q_NEW(RequestEvent, NEW_REQUEST_SIG);
                       evt->request = Request_GetConnect(settings->knownApns[matchIdx].ssid, settings->knownApns[matchIdx].passwd);
                       DBGMSG_H("Connect To <%s>:<%s>", settings->knownApns[matchIdx].ssid, settings->knownApns[matchIdx].passwd);
                       QACTIVE_POST(AO_QueryEngine(), evt, NULL);
                    } else {
                        DBGMSG_ERR("TODO: implement multiple credentials");
                    }
                }
            }
            status_ = Q_HANDLED();
            break;
        }
        /* ${System::Engine::SM::Working::Connect} */
        case Q_EXIT_SIG: {
            DBGMSG_M("Exit");
            status_ = Q_HANDLED();
            break;
        }
        /* ${System::Engine::SM::Working::Connect::QUERY_DONE} */
        case QUERY_DONE_SIG: {
            status_ = Q_TRAN(&Engine_UPnPDiscoverer);
            break;
        }
        default: {
            status_ = Q_SUPER(&Engine_Working);
            break;
        }
    }
    return status_;
}
/*${System::Engine::SM::Working::UPnP} .....................................*/
QState Engine_UPnP(Engine * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${System::Engine::SM::Working::UPnP} */
        case Q_ENTRY_SIG: {
            DBGMSG_M("Entry");
            status_ = Q_HANDLED();
            break;
        }
        /* ${System::Engine::SM::Working::UPnP} */
        case Q_EXIT_SIG: {
            DBGMSG_M("Exit");
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&Engine_Working);
            break;
        }
    }
    return status_;
}
/*${System::Engine::SM::Working::UPnP::GetRootXml} .........................*/
QState Engine_GetRootXml(Engine * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${System::Engine::SM::Working::UPnP::GetRootXml} */
        case Q_ENTRY_SIG: {
            DBGMSG_M("Entry");
            RequestEvent *evt = Q_NEW(RequestEvent, NEW_REQUEST_SIG);
            evt->request = Request_GetRootXml();
            QACTIVE_POST(AO_QueryEngine(), evt, NULL);
            status_ = Q_HANDLED();
            break;
        }
        /* ${System::Engine::SM::Working::UPnP::GetRootXml} */
        case Q_EXIT_SIG: {
            DBGMSG_M("Exit");
            status_ = Q_HANDLED();
            break;
        }
        /* ${System::Engine::SM::Working::UPnP::GetRootXml::QUERY_DONE} */
        case QUERY_DONE_SIG: {
            DBGMSG_M("DONE");
            /* ${System::Engine::SM::Working::UPnP::GetRootXml::QUERY_DONE::[ok]} */
            if (SystemConfig_get()->igd.isUPnPActive) {
                status_ = Q_TRAN(&Engine_CheckConnection);
            }
            /* ${System::Engine::SM::Working::UPnP::GetRootXml::QUERY_DONE::[no]} */
            else {
                DBGMSG_WARN("UPnP no Ctrl Url");
                status_ = Q_HANDLED();
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&Engine_UPnP);
            break;
        }
    }
    return status_;
}
/*${System::Engine::SM::Working::UPnP::CheckConnection} ....................*/
QState Engine_CheckConnection(Engine * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${System::Engine::SM::Working::UPnP::CheckConnection} */
        case Q_ENTRY_SIG: {
            DBGMSG_M("Entry");
            RequestEvent *evt = Q_NEW(RequestEvent, NEW_REQUEST_SIG);
            evt->request = Request_GetCheckConnection();
            QACTIVE_POST(AO_QueryEngine(), evt, NULL);
            status_ = Q_HANDLED();
            break;
        }
        /* ${System::Engine::SM::Working::UPnP::CheckConnection} */
        case Q_EXIT_SIG: {
            DBGMSG_M("Exit");
            status_ = Q_HANDLED();
            break;
        }
        /* ${System::Engine::SM::Working::UPnP::CheckConnection::QUERY_DONE} */
        case QUERY_DONE_SIG: {
            status_ = Q_TRAN(&Engine_GetExtIP);
            break;
        }
        default: {
            status_ = Q_SUPER(&Engine_UPnP);
            break;
        }
    }
    return status_;
}
/*${System::Engine::SM::Working::UPnP::GetExtIP} ...........................*/
QState Engine_GetExtIP(Engine * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${System::Engine::SM::Working::UPnP::GetExtIP} */
        case Q_ENTRY_SIG: {
            DBGMSG_M("Entry");
            RequestEvent *evt = Q_NEW(RequestEvent, NEW_REQUEST_SIG);
            evt->request = Request_GetExternalIp();
            QACTIVE_POST(AO_QueryEngine(), evt, NULL);
            status_ = Q_HANDLED();
            break;
        }
        /* ${System::Engine::SM::Working::UPnP::GetExtIP} */
        case Q_EXIT_SIG: {
            DBGMSG_M("Exit");
            status_ = Q_HANDLED();
            break;
        }
        /* ${System::Engine::SM::Working::UPnP::GetExtIP::QUERY_DONE} */
        case QUERY_DONE_SIG: {
            status_ = Q_TRAN(&Engine_OpenPort);
            break;
        }
        default: {
            status_ = Q_SUPER(&Engine_UPnP);
            break;
        }
    }
    return status_;
}
/*${System::Engine::SM::Working::UPnP::OpenPort} ...........................*/
QState Engine_OpenPort(Engine * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${System::Engine::SM::Working::UPnP::OpenPort} */
        case Q_ENTRY_SIG: {
            DBGMSG_M("Entry");
            RequestEvent *evt = Q_NEW(RequestEvent, NEW_REQUEST_SIG);
            evt->request = Request_GetOpenPort();
            QACTIVE_POST(AO_QueryEngine(), evt, NULL);
            status_ = Q_HANDLED();
            break;
        }
        /* ${System::Engine::SM::Working::UPnP::OpenPort} */
        case Q_EXIT_SIG: {
            DBGMSG_M("Exit");
            status_ = Q_HANDLED();
            break;
        }
        /* ${System::Engine::SM::Working::UPnP::OpenPort::QUERY_DONE} */
        case QUERY_DONE_SIG: {
            status_ = Q_TRAN(&Engine_Ok);
            break;
        }
        default: {
            status_ = Q_SUPER(&Engine_UPnP);
            break;
        }
    }
    return status_;
}
/*${System::Engine::SM::Working::UPnP::UPnPDiscoverer} .....................*/
QState Engine_UPnPDiscoverer(Engine * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /* ${System::Engine::SM::Working::UPnP::UPnPDiscoverer} */
        case Q_ENTRY_SIG: {
            DBGMSG_M("Entry");
            RequestEvent *evt = Q_NEW(RequestEvent, NEW_REQUEST_SIG);
            evt->request = Request_GetUPnPDiscoverer();
            QACTIVE_POST(AO_QueryEngine(), evt, NULL);
            status_ = Q_HANDLED();
            break;
        }
        /* ${System::Engine::SM::Working::UPnP::UPnPDiscoverer} */
        case Q_EXIT_SIG: {
            DBGMSG_M("Exit");
            status_ = Q_HANDLED();
            break;
        }
        /* ${System::Engine::SM::Working::UPnP::UPnPDiscoverer::QUERY_DONE} */
        case QUERY_DONE_SIG: {
            /* ${System::Engine::SM::Working::UPnP::UPnPDiscoverer::QUERY_DONE::[upnp?]} */
            if (SystemConfig_get()->igd.isUPnPActive) {
                status_ = Q_TRAN(&Engine_GetRootXml);
            }
            /* ${System::Engine::SM::Working::UPnP::UPnPDiscoverer::QUERY_DONE::[no]} */
            else {
                DBGMSG_WARN("UPnP not Found IGD");
                status_ = Q_HANDLED();
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&Engine_UPnP);
            break;
        }
    }
    return status_;
}
/*${System::Engine::SM::Working::Ok} .......................................*/
QState Engine_Ok(Engine * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        default: {
            status_ = Q_SUPER(&Engine_Working);
            break;
        }
    }
    return status_;
}


static void HandleSysEvt(SystemEvent *evt) {

    Event_p event = &evt->event;
    switch (event->type) {
        case EVENT_EXTI: {
            DBGMSG_L("[Exti] pin %d act %d", event->data, event->subType.exti);
            char *text = "hello !";
            CanTxMsgTypeDef txMsg = {
                    0x22, 0,
                    CAN_ID_STD,
                    CAN_RTR_DATA,
                    8,
                    { text[0], text[1], text[2], text[3], text[4], text[5], text[6], text[7] }
            };
            USB_ACM_write((uint8_t*)text, strlen(text));
            CAN_write(&txMsg);
            } break;
        case EVENT_UART:
            UART_handleEvent(event);
            break;
        case EVENT_UxART_Buffer: {
            switch (event->subType.uxart) {
                case ES_UxART_RX: {
                    SystemEvent *evt = Q_NEW(SystemEvent, NEW_DATA_SIG);
                    evt->event = *event;
                    QACTIVE_POST(AO_QueryEngine(), evt, NULL);
                    } break;
                default:
                    break;
            }
        } break;
        case EVENT_CAN:
            CAN_handleEvent(event);
            break;
        case EVENT_SYSTICK:
            if (event->subType.systick == ES_SYSTICK_SECOND_ELAPSED) {
                Timer_makeTick();
            }
        case EVENT_USART:
            USART_handleEvent(event);
            break;
        case EVENT_DUMMY:
            break;
        default:
            DBGMSG_WARN("Unhandled Event type %p data %p!", event->type, event->data);
            break;
    }
}
