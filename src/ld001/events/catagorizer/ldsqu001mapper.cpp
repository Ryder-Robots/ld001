#include <ld001/environment/ld001_env_processor.hpp>

using namespace rrobot;

dlib::logger dlog_mapper("rr_mapper");

using json = nlohmann::json;

/**
 * Create LD001 environment.
 */
Environment LdSqu001Mapper::createEnvironment(std::string content) {
    if (!json::accept(content)) {
        throw new InvalidFormat("could not read content of manifest");
    }

    return Ld001EnvProcessor::createEnvironment(json::parse(content));
}

/*
 * Performs inilization.
 */
void LdSqu001Mapper::init(Environment* environment, StateIface* state, StateManagerIface* statusProcessor) {
    CatagorizerMapperBase::init(environment, state, statusProcessor);
    dlog_mapper.set_level(environment->getLogging().getLogLevel());
}


/*
 * returns list of supported queue names
 */
std::vector<RRP_QUEUES> LdSqu001Mapper::queueNames() {
    std::vector<RRP_QUEUES> queues = {CATEGORIZER, STATUS, USER_INTERFACE, MICROCONTROLLER, AI_ENGINE,};
    return queues;
}

/*
 * create event handlers. 
 */
std::vector<EventHandler*> LdSqu001Mapper::createEventHandlers() {
    RrStatusHandler* statusHandler = new RrStatusHandler();
    UiHandler* uiHandler = new UiHandler();
    statusHandler->init(_state, _environment,  _statusProcessor);

    rrobot::Jserializer* jserializer = new rrobot::Jserializer();
    RrServer* server = new RrServer();
    uiHandler->init(server, _environment, _state, jserializer);

    rrobot::HbridgeController* hbridge = new rrobot::HbridgeController();
    hbridge->init(_state, _environment);

    EaiHandler* aiHandler = new EaiHandler();
    AiVirtualPredictor* aiVirtualPredictor = new AiVirtualPredictor();
    aiVirtualPredictor->init(_state, _environment, _statusProcessor);
    aiHandler->init(_state, _environment, _statusProcessor, aiVirtualPredictor);

    _statusProcessor->addHandler(statusHandler);
    _statusProcessor->addHandler(uiHandler);
    _statusProcessor->addHandler(hbridge);
    _statusProcessor->addHandler(aiHandler);

    return _statusProcessor->getHandlers();
}

RRP_QUEUES LdSqu001Mapper::mapQueue(Event* eventRef) {

    if (eventRef == nullptr) {
        dlog_mapper << dlib::LERROR << "null event was recieved";
        throw InvalidMessageSent("null event was recieved");
    }

    Event event = *eventRef;
    // send all external out to user interface.
    if (event.getDirection() == MSPDIRECTION::EXTERNAL_OUT) {
        return RRP_QUEUES::USER_INTERFACE;
    }
    RRP_QUEUES queue = RRP_QUEUES::NONE;
    switch (event.getCommand()) {
        case MSPCOMMANDS::MSP_AUTHKEY:
        case MSPCOMMANDS::MSP_IDENT:
        case MSPCOMMANDS::MSP_STATUS:
            queue = RRP_QUEUES::STATUS;
            break;
        case MSPCOMMANDS::MSP_MODE:
            if (!event.hasPayload()) {
                dlog_mapper << dlib::LERROR << "no mode was given";
                throw MissingRequiredAttributeException("no mode was given");
            }
            {
                msp_mode mspMode = event.getPayload<msp_mode>();
                dlog_mapper << dlib::LINFO << "mode change has been requested to " <<  mspMode.get_mode();
                _statusProcessor->setMode(mspMode.get_mode());
                msp_mode* payload = new msp_mode();
                payload->set_mode(_statusProcessor->getMode());
                eventRef = new Event(MSPCOMMANDS::MSP_MODE, MSPDIRECTION::EXTERNAL_OUT, payload);
            }
            queue = RRP_QUEUES::USER_INTERFACE;
            break;

        case MSPCOMMANDS::MSP_MOTOR:
             if (_statusProcessor->getMode() == RR_CMODES::CMODE_MANUAL_FLIGHT) {
                queue = RRP_QUEUES::AI_ENGINE;
                break;
            } 
            {
                dlog_mapper << dlib::LWARN << "attempt to send flight commmands but not in correct mode";
                msp_error* payload = new msp_error();
                payload->set_message("unsupported command for this drone"); 
                eventRef = new Event(MSPCOMMANDS::MSP_ERROR, MSPDIRECTION::EXTERNAL_OUT, payload);
                queue = RRP_QUEUES::USER_INTERFACE;
                break;
            }           

        // TODO deprecate this once the new command works
        case MSPCOMMANDS::MSP_SET_MOTOR_HBRIDGE:
            if (_statusProcessor->getMode() == RR_CMODES::CMODE_MANUAL_FLIGHT) {
                queue = RRP_QUEUES::MICROCONTROLLER;
                break;
            } 
            {
                dlog_mapper << dlib::LWARN << "attempt to send flight commmands but not in correct mode";
                msp_error* payload = new msp_error();
                payload->set_message("unsupported command for this drone"); 
                eventRef = new Event(MSPCOMMANDS::MSP_ERROR, MSPDIRECTION::EXTERNAL_OUT, payload);
                queue = RRP_QUEUES::USER_INTERFACE;
                break;
            }
    }
    return queue;
}